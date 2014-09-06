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

#define CLASS_MAGIC 0xabcdef

typedef struct Class Class;

/*
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
 * struct Foo {
 * 	   Object object;
 *     // ...
 * }
 *
 * extern const Class *Foo;
 *
 * // foo.c
 *
 * static id init(id self, va_list *args) {
 *
 *     self = super(Object, self, init, args);
 *     if (self) {
 *         override(Object, self, init, init);
 *
 *     	   struct Foo *this = cast(Foo, self);
 *         // ...
 *     }
 *     return self;
 * }
 *
 * static struct Foo foo;
 *
 * static struct Class class {
 *     .name = "Foo",
 *     .size = sizeof(struct Foo),
 *     .superclass = &Object,
 *     .archetype = &foo,
 *     .init = init,
 * };
 *
 * const Class *Foo = &class;
 *
 * @endcode
 */
struct Class {

	/*
	 * @brief The magic number identifying this structure as a class.
	 *
	 * @details Do not set this value; it is set by Objectively when your class
	 * is first encountered to signify initialization.
	 *
	 * @private
	 */
	int magic;

	/*
	 * @brief The class name (required).
	 */
	const char *name;

	/*
	 * @brief The instance size (required).
	 */
	const size_t size;

	/*
	 * @brief The superclass (required). For custom base types, use
	 * <code>Object</code>.
	 */
	const Class **superclass;

	/*
	 * @brief The statically allocated archetype instance (required).
	 */
	const id archetype;

	/*
	 * @brief The static initializer for the class (optional).
	 *
	 * @details This method is run once when your class is initialized.
	 */
	void (*initialize)(void);

	/*
	 * @brief The instance initializer.
	 *
	 * @param obj The newly allocated instance.
	 * @param args The constructor arguments (optional).
	 */
	id (*init)(id obj, va_list *args);
};

extern id __new(const Class *class, ...);

extern id __cast(const Class *class, const id obj);

extern void delete(id obj);

extern const Class *classof(const id obj);

extern const id archetypeof(const Class *class);

/*
 * @brief Instantiate a type.
 */
#define new(type, ...) \
	__new(__##type, ## __VA_ARGS__)

/*
 * @brief Safely cast to a type.
 */
#define cast(type, instance) \
	__cast(__##type, (const id) instance)

/*
 * @brief Call a method.
 */
#define $(instance, method, ...) \
	instance->method(instance, ## __VA_ARGS__)

/*
 * @brief Take a constructor parameter.
 */
#define arg(args, type, def) \
	(args ? va_arg(*args, type) : def)

/*
 * @brief Override a supertype method in your type.
 */
#define override(type, instance, method, implementation) \
	((type *) instance)->method = implementation;

/*
 * @brief Invoke a supertype method on an instance of your type.
 */
#define super(type, instance, method, ...) \
	((type *) archetypeof(__##type))->method((type *) instance, ## __VA_ARGS__)


#endif
