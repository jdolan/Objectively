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
 * @details Classes are the interface between the library functions
 * <code>new</code>, <code>cast</code>, <code>delete</code>, et. al. and
 * Objects. Each Class describes a type, and each instance of that type will
 * reference the Class.
 *
 * To implement a type, define its instance structure and its Class
 * descriptor:
 *
 * @code
 *
 * // foo.h
 *
 * typedef struct Foo {
 * 	   Object object;
 *     // ...
 * } Foo;
 *
 * extern const Class *__Foo;
 *
 * // foo.c
 *
 * static id init(id obj, va_list *args) {
 *
 *     Foo *self = super(Object, obj, init, args);
 *     if (self) {
 *         override(Object, self, init, init);
 *         // ...
 *     }
 *     return self;
 * }
 *
 * static Foo foo;
 *
 * static Class class {
 *     .name = "Foo",
 *     .size = sizeof(Foo),
 *     .superclass = &__Object,
 *     .archetype = &foo,
 *     .init = init,
 * };
 *
 * const Class *__Foo = &class;
 *
 * @endcode
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
	 * @brief The magic number identifying this structure as a class.
	 *
	 * @details Do not set this value; it is set by Objectively when your class
	 * is first encountered to signify initialization.
	 *
	 */
	int magic;

	/**
	 * @brief The class name (required).
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
	 * @brief The class initializer (required).
	 *
	 * @details This method is run once when your class is first initialized.
	 */
	void (*initialize)(Class *class);

	/**
	 * @private
	 *
	 * @brief The interface.
	 *
	 * @details Do not set this field. The interface is allocated by
	 * Objectively when your class is initialized.
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

/*
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
