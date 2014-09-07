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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <objectively/object.h>

#pragma mark - Object instance methods

static Object *copy(const Object *self) {
	return memcpy(calloc(1, self->class->size), self, self->class->size);
}

static void dealloc(Object *self) {
	free(self);
}

static BOOL isEqual(const Object *self, const Object *other) {
	return self == other;
}

static BOOL isKindOfClass(const Object *self, const Class *class) {

	const Class *c = self->class;
	while (c) {
		if (c == class) {
			return YES;
		}
		c = c->superclass;
	}

	return NO;
}

#pragma mark - Class methods

static id init(id obj, va_list *args) {
	assert(obj);
	return obj;
}

static const ObjectInterface interface = {
	.init = init,
	.copy = copy,
	.dealloc = dealloc,
	.isEqual = isEqual,
	.isKindOfClass = isKindOfClass,
};

Class __Object = {
	.name = "Object",
	.size = sizeof(Object),
	.interface = (const Interface *) &interface,
};
