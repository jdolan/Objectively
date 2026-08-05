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

#pragma once

#include <Objectively/Types.h>
#include <Objectively/Once.h>

/**
 * @file
 * @brief Classes describe the state and behavior of an Objectively type.
 */

typedef struct ClassDef ClassDef;
typedef struct Class Class;

/**
 * @brief ClassDefs are passed to `_initialize` via an _archetype_ to initialize a Class.
 * @ingroup Core
 */
struct ClassDef {

  /**
   * @brief The Class destructor (optional).
   * This method is run for initialized Classes when your application exits.
   */
  void (*destroy)(Class *clazz);

  /**
   * @brief The Class initializer (optional).
   * @details Objectively invokes your Class initializer the first time your Class is required;
   * either via `_alloc`, or via static method invocation. The Class initializer is responsible
   * for populating the Class' `interface` with valid method implementations:
   * ```
   * static void initialize(Class *clazz) {
   *     ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
   *     ((FooInterface *) clazz->interface)->bar = bar;
   * }
   * ```
   * @remarks Each Class' `interface` is copied from its superclass before this initializer is
   * called. Therefore, only overridden methods, and methods unique to the Class itself, must be
   * set via `initialize`.
   */
  void (*initialize)(Class *clazz);

  /**
   * @brief The instance size (required).
   */
  size_t instanceSize;

  /**
   * @brief The interface offset (required).
   */
  ptrdiff_t interfaceOffset;

  /**
   * @brief The interface size (required).
   */
  size_t interfaceSize;

  /**
   * @brief The Class name (required).
   */
  const char *name;

  /**
   * @brief The superclass (required). e.g. `_Object()`.
   */
  Class *superclass;
};

/**
 * @brief The runtime representation of a Class.
 */
struct Class {

  /**
   * @brief The Class definition.
   */
  ClassDef def;

  /**
   * @brief The interface of the Class.
   */
  ident interface;

  /**
   * @brief Provides chaining of initialized Classes.
   */
  Class *next;

  /**
   * @brief The base address of the image that declared this Class.
   * @details Recorded by `_initialize`, which is the only place that sees every
   * Class: one resolved through a registered image and one initialized by that
   * image calling `alloc` arrive here alike. `removeClassImage` matches on it.
   */
  ident image;
};

/**
 * @brief Initializes the given Class.
 * @param clazz The Class descriptor.
 * @return The initialized Class.
 */
OBJECTIVELY_EXPORT Class* _initialize(const ClassDef *clazz);

/**
 * @brief Instantiate a type through the given Class.
 */
OBJECTIVELY_EXPORT ident _alloc(Class *clazz);

/**
 * @brief Perform a type-checking cast.
 */
OBJECTIVELY_EXPORT ident _cast(const Class *clazz, const ident obj);

/**
 * @brief Registers an image that provides Classes, e.g. a plugin.
 * @param handle A handle from `dlopen`.
 * @details `classForName` resolves a name it has not yet initialized through the
 * process-wide namespace, which holds only images loaded `RTLD_GLOBAL`, and which
 * Windows does not have at all. An application that loads Classes from a plugin
 * registers it here instead, and may then load it `RTLD_LOCAL` - which is how it
 * keeps two plugins exporting the same symbols from coalescing.
 * @remarks Registered images are searched most recently added first, so a newly
 * loaded plugin answers ahead of the one it replaced.
 * @remarks Resolution is by Objectively's own convention, the Class name
 * prefixed with an underscore, so nothing about the image has to be declared.
 */
OBJECTIVELY_EXPORT void addClassImage(ident handle);

/**
 * @brief Unregisters an image, and every Class it declared.
 * @param handle The handle given to `addClassImage`.
 * @remarks Classes initialized from an image outlive it otherwise: they are
 * cached by name, and `classForName` answers from that cache ahead of any image.
 * On a platform where closing an image really unmaps it - which `dlclose` does on
 * Linux and `FreeLibrary` does on Windows - the next lookup would then read a
 * `ClassDef` that is no longer mapped. `classForName` compares the name of every
 * Class it walks, so one left behind breaks every lookup, not only its own.
 * @remarks The Classes are **not** destroyed, and this is not an oversight.
 * `dlclose` does not unmap on macOS, so an archetype that has already run keeps
 * answering from its own `static Class *` for as long as the process lives, and
 * would hand back whatever this freed. Unregistering is the only thing that is
 * safe whether the image goes away or stays: the Class becomes unreachable by
 * name, and remains valid for the archetype that owns it. The cost is the Class
 * and its interface, which are not reclaimed.
 * @remarks MUST be called while the handle is still open, and only once nothing
 * instantiated from that image survives.
 */
OBJECTIVELY_EXPORT void removeClassImage(ident handle);

/**
 * @return The Class with the given name, or `NULL` if no such Class has been initialized.
 * @remarks Classes already initialized are answered first, then each registered
 * image, then the process-wide namespace.
 */
OBJECTIVELY_EXPORT Class *classForName(const char *name);

/**
 * @brief Atomically decrement the given Object's reference count. If the
 * resulting reference count is `0`, the Object is deallocated.
 * @return This function always returns `NULL`.
 */
OBJECTIVELY_EXPORT ident release(ident obj);

/**
 * @brief Atomically increment the given Object's reference count.
 * @return The Object.
 * @remarks By calling this, the caller is expressing ownership of the Object,
 * and preventing it from being released. Be sure to balance calls to `retain`
 * with calls to `release`.
 */
OBJECTIVELY_EXPORT ident retain(ident obj);

/**
 * @brief The page size, in bytes, of the target host.
 */
OBJECTIVELY_EXPORT size_t _pageSize;

/**
 * @brief The header value identifying Objectively types.
 */
#define OBJECTIVELY_MAGIC 0xdeadbeef

/**
 * @brief Test if the given pointer is an Object.
 */
#define isobject(obj) \
  (obj && *((unsigned int *) obj) == OBJECTIVELY_MAGIC)

/**
 * @brief Test if the given pointer is an instance of the specified type.
 */
#define instanceof(type, obj) \
  (isobject(obj) && $((Object *) obj, isKindOfClass, _##type()))

/**
 * @brief Allocate and initialize and instance of `type`.
 */
#define alloc(type) \
  ((type *) _alloc(_##type()))

/**
 * @brief Safely cast `obj` to `type`.
 */
#define cast(type, obj) \
  ((type *) _cast(_##type(), (const ident) obj))

/**
 * @brief Resolve the Class of an Object instance.
 */
#define classof(obj) \
  ((Object *) obj)->clazz

/**
 * @brief Resolve the Class name of the given Object instance.
 */
#define classnameof(obj) \
  classof(obj)->def.name

/**
 * @brief Resolve the typed interface of a Class.
 */
#define interfaceof(type, clazz) \
  ((type##Interface *) (clazz)->interface)

/**
 * @brief Invoke an instance method.
 */
#define $(obj, method, ...) \
  ({ \
    typeof(obj) _obj = obj; \
    _obj->interface->method(_obj, ## __VA_ARGS__); \
  })

/// @brief Invoke a Class method.
#define $$(type, method, ...) \
  ({ \
    interfaceof(type, _##type())->method(__VA_ARGS__); \
  })

/**
 * @brief Invoke a Superclass instance method.
 */
#define super(type, obj, method, ...) \
  interfaceof(type, _Class()->def.superclass)->method(cast(type, obj), ## __VA_ARGS__)
