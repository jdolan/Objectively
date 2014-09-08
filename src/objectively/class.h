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

#ifndef _objectively_class_h_
#define _objectively_class_h_

#include <stdarg.h>

#include <objectively/types.h>

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
	 * @brief The magic number identifying this structure as a Class.
	 *
	 * *Do not* set this value; it is set when your Class is initialized.
	 */
	int magic;

	/**
	 * @brief The Class name (required).
	 */
	const char *name;

	/**
	 * @brief The superclass (required). e.g. `&__Object`.
	 */
	Class *superclass;

	/**
	 * @brief The instance size (required).
	 */
	const size_t instanceSize;

	/**
	 * @brief The interface size (required).
	 */
	const size_t interfaceSize;

	/**
	 * @brief The Class initializer (optional).
	 *
	 * This method is run _once_ when your class is first initialized.
	 *
	 * If your Class defines an interface, you *must* implement this method
	 * and initialize that interface here.
	 *
	 * @remark The `interface` property of the Class is copied from the
	 * superclass before this method is called. Method overrides are achieved
	 * by simply overwriting the function pointers here.
	 */
	void (*initialize)(Class *class);

	/**
	 * @brief The interface handle.
	 *
	 * Do not set this field. The interface is allocated by Objectively when
	 * your class is initialized.
	 */
	id interface;
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
 * @brief Delete (deallocate) an instance.
 *
 * @details You are responsible for deleting all Objects you create.
 */
extern void delete(id obj);

/**
 * @brief Instantiate a type.
 */
#define new(type, ...) \
	__new((Class *) &__##type, ## __VA_ARGS__)

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
	(args ? va_arg(*args, type) : def)

#endif
