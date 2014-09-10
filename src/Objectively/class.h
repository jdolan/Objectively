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
#include <stdio.h>

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
	 *
	 * @param self The Class.
	 */
	void (*destroy)(Class *self);

	/**
	 * @brief The Class initializer (optional).
	 *
	 * This method is run when your class is first initialized.
	 *
	 * If your Class defines an interface, you *must* implement this method
	 * and initialize that interface here.
	 *
	 * @param self The Class.
	 *
	 * @remark The `interface` property of the Class is copied from the
	 * superclass before this method is called. Method overrides are achieved
	 * by simply overwriting the function pointers here.
	 */
	void (*initialize)(Class *self);

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
extern id __new(Class *class, ...);

/**
 * @brief Perform a type-checking cast.
 */
extern id __cast(Class *class, const id obj);

/**
 * Atomically decrement the given Object's reference count. If the resulting
 * reference count is `0`, the Object is deallocated.
 */
extern void release(id obj);

/**
 * Atomically increment the given Object's reference count.
 *
 * @remark By calling this, the caller is expressing ownership of the Object,
 * and preventing it from being released. Be sure to balance calls to `retain`
 * with calls to `release`.
 */
extern void retain(id obj);

/**
 * @brief Instantiate a type with a `NULL`-terminated arguments list.
 */
#define new(type, ...) \
	__new((Class *) &__##type, ## __VA_ARGS__, NULL)

/**
 * @brief Safely cast to a type.
 */
#define cast(type, instance) \
	__cast((Class *) &__##type, (const id) instance)

/**
 * @brief Resolve the Class of an instance.
 */
#define classof(instance) \
	((Object *) instance)->class

/**
 * @brief Resolve the Superclass of an instance.
 */
#define superclassof(instance) \
	classof(instance)->superclass

/**
 * @brief Apply a selector to an instance.
 */
#define $(instance, method, ...) \
	(instance)->interface->method(instance, ## __VA_ARGS__)

/**
 * @brief Apply a Superclass selector to an instance.
 */
#define super(type, instance, method, ...) \
	((type##Interface *) superclassof(instance)->interface)->method((type *) instance, ## __VA_ARGS__)

/**
 * @brief Take an initializer parameter.
 */
#define arg(args, type, def) \
	(va_arg(*args, type) ?: def)

/**
 * @brief Log a message.
 */
#define log(fmt, ...) \
	fprintf(stderr, fmt, ## __VA_ARGS__)

/**
 * @return The maximum of the two parameters.
 */
#define max(a, b) \
   ({ typeof(a) _a = (a); typeof(b) _b = (b); _a > _b ? _a : _b; })

/**
 * @return The minimum of the two parameters.
 */
#define min(a, b) \
   ({ typeof(a) _a = (a); typeof(b) _b = (b); _a < _b ? _a : _b; })

#endif
