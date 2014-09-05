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

typedef struct _Class Class;
struct _Class {
	Class *superclass;

	const char *name;
	size_t size;

	id archetype;

	/*
	 * @brief An optional static initializer for the Class.
	 */
	void (*initialize)(void);
	BOOL initialized;

	/*
	 * @brief The constructor.
	 */
	id (*init)(id self, va_list *args);
};

extern id new(Class *class, ...);

extern id cast(Class *class, const id obj);

extern id archetype(Class *class);

extern void delete(id obj);

extern id $(id obj, id selector, ...);

#endif
