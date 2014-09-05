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

#include <stdlib.h>
#include <string.h>

#include <objectively/object.h>

static id copy(const id self) {

	const struct Object *this = cast(Object, self);

	return memcpy(calloc(1, this->class->size), self, this->class->size);
}

static BOOL isKindOfClass(const id self, const Class *class) {

	const struct Object *this = cast(Object, self);

	Class *c = this->class;
	while (c) {
		if (c == class) {
			return YES;
		}
		c = class->superclass;
	}

	return NO;
}

static BOOL isEqual(const id self, const id other) {
	return self == other;
}

static void dealloc(id self) {
	free(self);
}

static id init(id self, va_list *args) {

	struct Object *this = cast(Object, self);
	if (this) {

		this->class = Object;

		this->init = init;
		this->copy = copy;

		this->isKindOfClass = isKindOfClass;
		this->isEqual = isEqual;

		this->dealloc = dealloc;
	}

	return this;
}

static struct Object archetype;

static Class class = {
	.name = "Object",
	.size = sizeof(Object),
	.init = init,
	.archetype = &archetype,
};

Class *Object = &class;
