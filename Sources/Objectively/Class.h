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

/**
 * @brief Initialized Classes reflect this value as their first word.
 */
#define CLASS_MAGIC 0xabcdef

typedef struct ClassDef ClassDef;
typedef struct Class Class;

/**
 * @brief Classes describe the state and behavior of an Objectively type.
 * @details Classes are the bridge between Objects and their interfaces. They provide an entry
 * point to the framework via the library function <code>_alloc</code>.
 * @details Each Class describes a type and initializes an interface. Each instance of that type
 * will reference the Class and, in turn, its interface.
 * @ingroup Core
 */
struct Class {

	/**
	 * @brief Initialized Classes will have `CLASS_MAGIC`.
	 */
	volatile int magic;

	/**
	 * @brief The dynamically allocated Class definition.
	 */
	ClassDef *def;

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
	 *     ((ObjectInterface *) clazz->def->interface)->dealloc = dealloc;
	 *     ((FooInterface *) clazz->def->interface)->bar = bar;
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
struct ClassDef {

	/**
	 * @brief A dynamically allocated copy of the Class descriptor.
	 */
	Class descriptor;

	/**
	 * @brief The interface of the Class.
	 */
	ident interface;

	/**
	 * @brief Provides chaining of initialized Classes.
	 */
	ClassDef *next;
};

/**
 * @brief Initializes the given Class.
 */
OBJECTIVELY_EXPORT void _initialize(Class *clazz);

/**
 * @brief Instantiate a type through the given Class.
 */
OBJECTIVELY_EXPORT ident _alloc(Class *clazz);

/**
 * @brief Perform a type-checking cast.
 */
OBJECTIVELY_EXPORT ident _cast(Class *clazz, const ident obj);

/**
 * @return The Class with the given name, or `NULL` if no such Class has been initialized.
 */
OBJECTIVELY_EXPORT Class *classForName(const char *name);

/**
 * @brief Atomically decrement the given Object's reference count. If the
 * resulting reference count is `0`, the Object is deallocated.
 */
OBJECTIVELY_EXPORT void release(ident obj);

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
 * @brief Allocate and initialize and instance of `type`.
 */
#define alloc(type) \
	((type *) _alloc(_##type()))

/**
 * @brief Safely cast to a type.
 */
#define cast(type, obj) \
	((type *) _cast(_##type(), (const ident) obj))

/**
 * @brief Resolve the Class of an Object instance.
 */
#define classof(obj) \
	((Object *) obj)->clazz

/**
 * @brief Resolve the typed interface of a Class.
 */
#define interfaceof(type, clazz) \
	((type##Interface *) (clazz)->def->interface)

/**
 * @brief Invoke an instance method.
 */
#define $(obj, method, ...) \
	({ \
		typeof(obj) _obj = obj; \
		_obj->interface->method(_obj, ## __VA_ARGS__); \
	})

/**
 * @brief Invoke a Class method.
 */
#define $$(type, method, ...) \
	({ \
		_initialize(_##type()); \
		interfaceof(type, _##type())->method(__VA_ARGS__); \
	})

/**
 * @brief Invoke a Superclass instance method.
 */
#define super(type, obj, method, ...) \
	interfaceof(type, _Class()->superclass)->method(cast(type, obj), ## __VA_ARGS__)
