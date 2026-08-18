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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined(_WIN32)
#include <Windows.h>
#elif defined(__APPLE__)
#include <mach-o/dyld.h>
#else
#include <link.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "Class.h"
#include "Object.h"

size_t _pageSize;

static Class *_classes;

/**
 * @brief The registered images providing Classes.
 */
#define MAX_CLASS_IMAGES 8
static ident _classImages[MAX_CLASS_IMAGES];
static size_t _classImageCount;

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

  /* Link before publishing, and relaxed on the way in: the CAS is a read-modify-
   * write, so it joins the release sequence of the push it displaces, and a
   * reader acquiring the head synchronizes with every publisher behind it. The
   * head is only copied here, never dereferenced, so there is nothing to acquire. */
  clazz->next = __atomic_load_n(&_classes, __ATOMIC_RELAXED);
  while (!__atomic_compare_exchange_n(&_classes, &clazz->next, clazz, 1, __ATOMIC_RELEASE, __ATOMIC_RELAXED)) ;

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

/**
 * @return The base address of the image behind `handle`, or `NULL`.
 * @remarks There is no one call for this. Windows hands out the module itself as
 * the handle, which `GetModuleFileName` confirms rather than assumes; glibc
 * answers from the link map; and macOS, which has neither, is left with matching
 * the handle against the loaded images.
 */
static ident imageForHandle(ident handle) {

  assert(handle);

#if defined(_WIN32)
  char path[MAX_PATH];
  if (GetModuleFileNameA((HMODULE) handle, path, sizeof(path))) {
    return handle;
  }
#elif defined(__APPLE__)
  for (uint32_t i = 0; i < _dyld_image_count(); i++) {

    /* RTLD_LOCAL is not the default on macOS, and dlopen of an image already
     * loaded promotes it to the global namespace, which would put every image in
     * the process there - undoing the isolation the caller loaded it for. */
    ident image = dlopen(_dyld_get_image_name(i), RTLD_LAZY | RTLD_NOLOAD | RTLD_LOCAL);
    if (image == NULL) {
      continue;
    }

    dlclose(image);

    if (image == handle) {
      return (ident) _dyld_get_image_header(i);
    }
  }
#else
  struct link_map *map;
  if (dlinfo(handle, RTLD_DI_LINKMAP, &map) == 0 && map) {
    return (ident) map->l_addr;
  }
#endif

  return NULL;
}

void removeClassImage(ident handle) {

  assert(handle);

  for (size_t i = 0; i < _classImageCount; i++) {
    if (_classImages[i] == handle) {
      memmove(_classImages + i, _classImages + i + 1, (_classImageCount - i - 1) * sizeof(ident));
      _classImages[--_classImageCount] = NULL;
      break;
    }
  }

  const ident image = imageForHandle(handle);
  if (image == NULL) {
    return;
  }

  Class **link = &_classes;
  while (*link) {
    Class *clazz = *link;

    if (clazz->image == image) {
      *link = clazz->next;
      clazz->next = NULL;
    } else {
      link = &clazz->next;
    }
  }
}

void addClassImage(ident handle) {

  assert(handle);
  assert(_classImageCount < MAX_CLASS_IMAGES);

  _classImages[_classImageCount++] = handle;
}

Class *classForName(const char *name) {

  if (name) {
    Class *c = __atomic_load_n(&_classes, __ATOMIC_ACQUIRE);
    while (c) {
      if (strcmp(name, c->def.name) == 0) {
        return c;
      }
      c = c->next;
    }

    char *s;
    if (asprintf(&s, "_%s", name) > 0) {
      Class *clazz = NULL;
      Class *(*archetype)(void) = NULL;

      for (size_t i = _classImageCount; i > 0 && archetype == NULL; i--) {
        archetype = dlsym(_classImages[i - 1], s);
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
