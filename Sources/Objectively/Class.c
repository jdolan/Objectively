/*
 * Objectively: Ultra-lightweight object oriented framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include "Config.h"

#include <assert.h>
#include <dlfcn.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <Windows.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "Class.h"
#include "Object.h"

size_t _pageSize;

static Class *_classes;

/**
 * @brief Guards the structure of `_classes`. MUST NOT be held across `dlsym`,
 * `dlopen`, or a Class initializer, each of which can reenter `_initialize`.
 */
static pthread_mutex_t _classesLock = PTHREAD_MUTEX_INITIALIZER;

/**
 * @brief A registered image: the handle the application holds, and the base
 * address that the Classes it declares record in `Class::image`.
 */
typedef struct ClassImage ClassImage;
struct ClassImage {
  ident handle;
  ident image;
  ClassImage *next;
};

/**
 * @brief The registered images providing Classes, most recently added first.
 * Published atomically rather than under `_classesLock`, which cannot be held
 * across the `dlsym` this list exists for. A plain list rather than a
 * MutableArray, because Class is beneath the collections.
 */
static ClassImage *_images;

/**
 * @brief Called `atexit` to teardown Objectively.
 */
static void teardown(void) {
  Class *c;

  c = _classes;
  while (c) {
    if (c->def.destroy) {
      c->def.destroy(c);
    }

    c = c->next;
  }

  c = _classes;
  while (c) {

    Class *next = c->next;

    free(c->interface);
    free(c);

    c = next;
  }

  ClassImage *i = _images;
  while (i) {

    ClassImage *next = i->next;

    free(i);

    i = next;
  }
}

/**
 * @brief Called when initializing `Object` to setup Objectively.
 */
static void setup(void) {

  _classes = NULL;

#if !defined(_SC_PAGESIZE)
  _pageSize = 4096;
#else
  _pageSize = sysconf(_SC_PAGESIZE);
#endif

  atexit(teardown);
}

/**
 * @return The base address of the image containing `address`, or `NULL`.
 * @remarks The Windows dlfcn shim has no `dladdr`. `UNCHANGED_REFCOUNT` matters:
 * without it this would pin the very module the caller is about to release.
 */
static const ident imageForAddress(const ident address) {

  assert(address);

#if defined(_WIN32)
  HMODULE module;
  if (GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
      GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR) address, &module)) {
    return module;
  }
#else
  Dl_info info;
  if (dladdr(address, &info)) {
    return info.dli_fbase;
  }
#endif

  return NULL;
}

Class *_initialize(const ClassDef *def) {

  static Once once;
  do_once(&once, setup());

  assert(def);
  assert(def->name);
  assert(def->instanceSize);
  assert(def->interfaceSize);
  assert(def->interfaceOffset);

  Class *clazz = calloc(1, sizeof(Class));
  assert(clazz);

  clazz->def = *def;

  clazz->interface = calloc(1, def->interfaceSize);
  assert(clazz->interface);

  Class *superclass = clazz->def.superclass;
  if (superclass) {

    assert(superclass->def.instanceSize <= def->instanceSize);
    assert(superclass->def.interfaceSize <= def->interfaceSize);

    memcpy(clazz->interface, superclass->interface, superclass->def.interfaceSize);
  }

  if (clazz->def.initialize) {
    clazz->def.initialize(clazz);
  }

  /* def.name is a literal in the declaring image, where the ClassDef itself is
   * a compound literal with automatic storage. */
  clazz->image = imageForAddress((ident) def->name);

  /* Taken here rather than around the whole function: def.initialize above can
   * reach other archetypes, and so this, before that Class is published. */
  pthread_mutex_lock(&_classesLock);

  clazz->next = _classes;
  _classes = clazz;

  pthread_mutex_unlock(&_classesLock);

  return clazz;
}

ident _alloc(Class *clazz) {

  ident obj = calloc(1, clazz->def.instanceSize);
  assert(obj);

  Object *object = (Object *) obj;

  object->magic = OBJECTIVELY_MAGIC;
  object->clazz = clazz;
  object->referenceCount = 1;

  ident interface = clazz->interface;
  do {
    *(ident *) (obj + clazz->def.interfaceOffset) = interface;
  } while ((clazz = clazz->def.superclass));

  return obj;
}

ident _cast(const Class *clazz, const ident obj) {

  if (obj) {
    const Class *c = ((Object *) obj)->clazz;
    while (c) {

      // as a special case, we optimize for _Object
      if (c == clazz || clazz == _Object()) {
        break;
      }

      c = c->def.superclass;
    }
    assert(c);
  }

  return (ident) obj;
}

void addClassImage(ident handle, const ident address) {

  assert(handle);
  assert(address);

  const ident image = imageForAddress(address);
  if (image == NULL) {
    fprintf(stderr, "%s: no image contains %p\n", __func__, address);
    abort();
  }

  ClassImage *classImage = calloc(1, sizeof(ClassImage));
  assert(classImage);

  classImage->handle = handle;
  classImage->image = image;

  /* Published the same way a Class is, and for the same reason: classForName
   * walks this list on any thread. */
  classImage->next = __atomic_load_n(&_images, __ATOMIC_RELAXED);
  while (!__atomic_compare_exchange_n(&_images, &classImage->next, classImage, 1,
      __ATOMIC_RELEASE, __ATOMIC_RELAXED)) ;
}

void removeClassImage(ident handle) {

  assert(handle);

  /* Held from the search to the last unlink, so that retiring an image and
   * dropping its Classes is one operation, and a second call for the same handle
   * finds it already gone. Nothing here reaches the loader. */
  pthread_mutex_lock(&_classesLock);

  ident image = NULL;

  /* Retired in place rather than unlinked, so that a concurrent classForName
   * parked on this node still has a next to follow, and never reads a node that
   * has been freed. Retiring is a single store of the handle it matches on, so
   * that walk sees this image or does not, and never half of it. Retired nodes
   * are freed at teardown; reusing one would put a newly registered image where
   * the retired one sat, and lookup order is newest first. */
  for (ClassImage *i = __atomic_load_n(&_images, __ATOMIC_ACQUIRE); i; i = i->next) {
    if (__atomic_load_n(&i->handle, __ATOMIC_ACQUIRE) == handle) {
      image = i->image;
      __atomic_store_n(&i->handle, NULL, __ATOMIC_RELEASE);
      break;
    }
  }

  if (image == NULL) {
    fprintf(stderr, "%s: %p was never registered\n", __func__, handle);
    abort();
  }

  Class **classes = &_classes;
  while (*classes) {
    Class *clazz = *classes;

    if (clazz->image == image) {
      *classes = clazz->next;
      clazz->next = NULL;
    } else {
      classes = &clazz->next;
    }
  }

  pthread_mutex_unlock(&_classesLock);
}

Class *classForName(const char *name) {

  if (name) {
    pthread_mutex_lock(&_classesLock);

    Class *c = _classes;
    while (c) {
      if (strcmp(name, c->def.name) == 0) {
        break;
      }
      c = c->next;
    }

    pthread_mutex_unlock(&_classesLock);

    if (c) {
      return c;
    }

    char *s;
    if (asprintf(&s, "_%s", name) > 0) {
      Class *clazz = NULL;
      Class *(*archetype)(void) = NULL;

      for (ClassImage *i = __atomic_load_n(&_images, __ATOMIC_ACQUIRE);
          i && archetype == NULL; i = i->next) {

        ident handle = __atomic_load_n(&i->handle, __ATOMIC_ACQUIRE);
        if (handle) {
          archetype = dlsym(handle, s);
        }
      }

      if (archetype == NULL) {
#if defined(_WIN32)
        static Once once;
        static ident handle;
        do_once(&once, { handle = dlopen(NULL, RTLD_LAZY); });
        archetype = handle ? dlsym(handle, s) : NULL;
#else
        archetype = dlsym(RTLD_DEFAULT, s);
#endif
      }

      if (archetype) {
        clazz = archetype();
      }

      free(s);
      return clazz;
    }
  }

  return NULL;
}

ident release(ident obj) {

  if (obj) {
    Object *object = cast(Object, obj);

    assert(object);

    if (__atomic_fetch_sub(&object->referenceCount, 1, __ATOMIC_RELEASE) == 1) {
      __atomic_thread_fence(__ATOMIC_ACQUIRE);
      $(object, dealloc);
    }
  }

  return NULL;
}

ident retain(ident obj) {

  Object *object = cast(Object, obj);

  assert(object);

  /* A reference count of zero means another thread is already inside dealloc,
   * and the caller is retaining memory that is about to be freed. */
  unsigned int referenceCount = __atomic_load_n(&object->referenceCount, __ATOMIC_RELAXED);
  do {
    if (referenceCount == 0) {
      fprintf(stderr, "%s: %p is being deallocated\n", __func__, object);
      abort();
    }
  } while (!__atomic_compare_exchange_n(&object->referenceCount, &referenceCount,
      referenceCount + 1, 1, __ATOMIC_RELAXED, __ATOMIC_RELAXED));

  return obj;
}
