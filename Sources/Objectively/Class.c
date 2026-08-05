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
#include <windows.h>
#endif

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "Class.h"
#include "Object.h"

size_t _pageSize;

static Class *_classes;

/**
 * @brief The registered ClassLoaders.
 * @remarks This is a plain array rather than a MutableArray because Class is
 * beneath the collections: initializing one here would reenter `_initialize`.
 * Loaders are added and removed as images are loaded and closed, not per lookup.
 */
#define MAX_CLASS_LOADERS 8
static ClassLoader _classLoaders[MAX_CLASS_LOADERS];
static size_t _classLoaderCount;

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

  clazz->next = __sync_lock_test_and_set(&_classes, clazz);

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
 * @return The base address of the image containing `address`, or `NULL`.
 * @remarks The Windows dlfcn shim has no `dladdr`. `UNCHANGED_REFCOUNT` matters:
 * without it this would pin the very module the caller is about to release.
 */
static const void *imageForAddress(const void *address) {

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

void removeClassesForImage(const void *address) {

  const void *image = imageForAddress(address);
  if (image == NULL) {
    return;
  }

  Class **link = &_classes;
  while (*link) {
    Class *clazz = *link;

    /* def.name is a literal in the image that declared the Class, where the
     * ClassDef itself is a compound literal with automatic storage. */
    if (imageForAddress(clazz->def.name) == image) {

      *link = clazz->next;

      if (clazz->def.destroy) {
        clazz->def.destroy(clazz);
      }

      free(clazz->interface);
      free(clazz);
    } else {
      link = &clazz->next;
    }
  }
}

void addClassLoader(ClassLoader loader) {

  assert(loader);
  assert(_classLoaderCount < MAX_CLASS_LOADERS);

  _classLoaders[_classLoaderCount++] = loader;
}

void removeClassLoader(ClassLoader loader) {

  assert(loader);

  for (size_t i = 0; i < _classLoaderCount; i++) {
    if (_classLoaders[i] == loader) {

      memmove(_classLoaders + i, _classLoaders + i + 1,
          (_classLoaderCount - i - 1) * sizeof(ClassLoader));

      _classLoaders[--_classLoaderCount] = NULL;
      return;
    }
  }
}

Class *classForName(const char *name) {

  if (name) {
    Class *c = _classes;
    while (c) {
      if (strcmp(name, c->def.name) == 0) {
        return c;
      }
      c = c->next;
    }

    for (size_t i = _classLoaderCount; i > 0; i--) {
      Class *clazz = _classLoaders[i - 1](name);
      if (clazz) {
        return clazz;
      }
    }

    char *s;
    if (asprintf(&s, "_%s", name) > 0) {
      Class *clazz = NULL;
#if defined(_WIN32)
      static Once once;
      static ident handle;
      do_once(&once, { handle = dlopen(NULL, RTLD_LAZY); });
      Class *(*archetype)(void) = handle ? dlsym(handle, s) : NULL;
#else
      Class *(*archetype)(void) = dlsym(RTLD_DEFAULT, s);
#endif
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

    if (__sync_add_and_fetch(&object->referenceCount, -1) == 0) {
      $(object, dealloc);
    }
  }

  return NULL;
}

ident retain(ident obj) {

  Object *object = cast(Object, obj);

  assert(object);

  __sync_add_and_fetch(&object->referenceCount, 1);

  return obj;
}
