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

/**
 * @file
 *
 * @brief The Class structure.
 *
 * Classes are the bridge between Objects and their interfaces. They provide
 * an entry point to the framework via the library function <code>_alloc</code>.
 *
 * Each Class describes a type and initializes an interface. Each instance of
 * that type will reference the Class and, in turn, its interface.
 */

/**
 * @brief Undefine interface for MinGW-64 / Windows.
 */
#undef interface

/**
 * @brief Initialized Classes reflect this value as their first word.
 */
#define CLASS_MAGIC 0xabcdef

typedef struct Class Class;

/**
 * @brief The Class type.
 *
 * @ingroup Core
 */
struct Class {

	/**
	 * @private
	 *
	 * @brief For internal use only.
	 */
	struct {

		/**
		 * @brief Identifies this structure as an initialized Class.
		 */
		volatile int magic;

		/**
		 * @brief Provides chaining of initialized Classes.
		 */
		Class *next;

	} locals;

	/**
	 * @brief The Class destructor (optional).
	 *
	 * This method is run for initialized Classes when your application exits.
	 */
	void (*destroy)(Class *clazz);

	/**
	 * @brief The Class initializer (optional).
	 *
	 * This method is run when your class is first initialized.
	 *
	 * If your Class defines an interface, you *must* implement this method
	 * and initialize that interface here.
	 *
	 * @remarks The `interface` property of the Class is copied from the
	 * superclass before this method is called. Method overrides are achieved
	 * by simply overwriting the function pointers here.
	 */
	void (*initialize)(Class *clazz);

	/**
	 * @brief The instance size (required).
	 */
	const size_t instanceSize;

	/**
	 * @brief The interface handle (do *not* provide).
	 */
	ident interface;

	/**
	 * @brief The interface offset (required).
	 */
	ptrdiff_t interfaceOffset;

	/**
	 * @brief The interface size (required).
	 */
	const size_t interfaceSize;

	/**
	 * @brief The Class name (required).
	 */
	const char *name;

	/**
	 * @brief The superclass (required). e.g. `&_Object`.
	 */
	Class *superclass;
};

/**
 * @brief Initializes the given Class.
 */
extern Class *_initialize(Class *clazz);

/**
 * @brief Instantiate a type through the given Class.
 */
extern ident _alloc(Class *clazz);

/**
 * @brief Perform a type-checking cast.
 */
extern ident _cast(Class *clazz, const ident obj);

/**
 * @return The Class with the given name, or `NULL` if no such Class has been initialized.
 */
extern Class *classForName(const char *name);

/**
 * @brief Atomically decrement the given Object's reference count. If the
 * resulting reference count is `0`, the Object is deallocated.
 */
extern void release(ident obj);

/**
 * @brief Atomically increment the given Object's reference count.
 *
 * @return The Object.
 *
 * @remarks By calling this, the caller is expressing ownership of the Object,
 * and preventing it from being released. Be sure to balance calls to `retain`
 * with calls to `release`.
 */
extern ident retain(ident obj);

/**
 * @brief The page size, in bytes, of the target host.
 */
extern size_t _pageSize;

/**
 * @brief The last Object allocated via `_alloc`.
 */
extern __thread ident _last_alloc;

/**
 * @brief Allocate and initialize and instance of `type`.
 */
#define alloc(type, initializer, ...) \
    ( \
        _last_alloc = _alloc(&_##type), \
        ((type *) _last_alloc)->interface->initializer((type *) _last_alloc, ## __VA_ARGS__) \
    )

/**
 * @brief Safely cast to a type.
 */
#define cast(type, obj) \
	((type *) _cast(&_##type, (const ident) obj))

/**
 * @brief Resolve the Class of an Object instance.
 */
#define classof(obj) \
	((Object *) obj)->clazz

/**
 * @brief Resolve the typed interface of a Class.
 */
#define interfaceof(type, clazz) \
	((type##Interface *) (clazz)->interface)

/**
 * @brief Invoke an instance method.
 */
#define $(obj, method, ...) \
	(obj)->interface->method(obj, ## __VA_ARGS__)

/**
 * @brief Invoke a Class method.
 */
#define $$(type, method, ...) \
	interfaceof(type, _initialize(&_##type))->method(__VA_ARGS__)

/**
 * @brief Invoke a Superclass instance method.
 */
#define super(type, obj, method, ...) \
	interfaceof(type, _Class.superclass)->method(cast(type, obj), ## __VA_ARGS__)
