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

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(_WIN32)
#include <Windows.h>
#include <TlHelp32.h>
#endif

#include "Class.h"
#include "Object.h"

size_t _pageSize;

static Class *_classes;

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
 * @brief The Class archetype constructor signature, e.g. `_Object`.
 */
typedef Class *(*Archetype)(void);

/**
 * @brief Resolves a Class archetype by its exported symbol name.
 * @remarks On ELF and Mach-O, `dlsym` against the global handle resolves symbols
 * in every loaded object. The Windows `dlfcn` shim only searches the executable
 * and explicitly `dlopen`'d libraries, so archetypes exported by implicitly linked
 * DLLs (e.g. ObjectivelyMVC) are invisible to it. Enumerate the process' loaded
 * modules and resolve the symbol directly.
 */
static Archetype archetypeForSymbol(const char *symbol) {

#if defined(_WIN32)
  Archetype archetype = NULL;

  HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, 0);
  if (snapshot != INVALID_HANDLE_VALUE) {

    MODULEENTRY32 module;
    module.dwSize = sizeof(module);

    if (Module32First(snapshot, &module)) {
      do {
        FARPROC proc = GetProcAddress(module.hModule, symbol);
        if (proc) {
          archetype = (Archetype) (void *) proc;
          break;
        }
      } while (Module32Next(snapshot, &module));
    }

    CloseHandle(snapshot);
  }

  return archetype;
#else
  return (Archetype) dlsym(RTLD_DEFAULT, symbol);
#endif
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

    char *s;
    if (asprintf(&s, "_%s", name) > 0) {
      Class *clazz = NULL;
      const Archetype archetype = archetypeForSymbol(s);
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
