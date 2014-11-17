/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
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

#ifndef _Objectively_Class_h_
#define _Objectively_Class_h_

#include <stdarg.h>

#include <Objectively/Types.h>

/**
 * @file
 *
 * @brief The Class structure.
 *
 * Classes are the bridge between Objects and their interfaces. They provide
 * an entry point to the framework via the library function <code>new</code>.
 *
 * Each Class describes a type and initializes an interface. Each instance of
 * that type will reference the Class and, in turn, its interface.
 */

/**
 * @brief Initialized Classes reflect this value as their first word.
 */
#define CLASS_MAGIC 0xabcdef

typedef struct Class Class;

/**
 * @brief The Class type.
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
		int magic;

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
	 * @remark The `interface` property of the Class is copied from the
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
	id interface;

	/**
	 * @brief The interface size (required).
	 */
	const size_t interfaceSize;

	/**
	 * @brief The Class name (required).
	 */
	const char *name;

	/**
	 * @brief The superclass (required). e.g. `&__Object`.
	 */
	Class *superclass;
};

/**
 * @brief Instantiate a type through the given Class.
 */
extern id __alloc(Class *clazz);

/**
 * @brief Perform a type-checking cast.
 */
extern id __cast(Class *clazz, const id obj);

/**
 * @brief Atomically decrement the given Object's reference count. If the
 * resulting reference count is `0`, the Object is deallocated.
 */
extern void release(id obj);

/**
 * @brief Atomically increment the given Object's reference count.
 *
 * @remark By calling this, the caller is expressing ownership of the Object,
 * and preventing it from being released. Be sure to balance calls to `retain`
 * with calls to `release`.
 */
extern void retain(id obj);

/**
 * @brief Allocate a type.
 */
#define alloc(type) \
	(type *) __alloc(&__##type)

/**
 * @brief Safely cast to a type.
 */
#define cast(type, obj) \
	(type *) __cast((Class *) &__##type, (const id) obj)

/**
 * @brief Resolve the Class of an Object instance.
 */
#define classof(obj) \
	((Object *) obj)->clazz

/**
 * @brief Resolve the typed interface of an Object instance.
 */
#define interfaceof(type, obj) \
	(type##Interface *) classof(obj)->interface

/**
 * @brief Invoke an instance method.
 */
#define $(type, obj, method, ...) \
	(interfaceof(type, obj))->method(cast(type, obj), ## __VA_ARGS__)

/**
 * @brief Invoke a Superclass instance method.
 */
#define super(type, obj, method, ...) \
	((type##Interface *) __##type.interface)->method(cast(type, obj), ## __VA_ARGS__)

#endif
