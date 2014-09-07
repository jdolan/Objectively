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

#include <objectively/array.h>

#define ARRAY_CHUNK_SIZE 64

#pragma mark - Object

static void dealloc(Object *self) {

	free(((Array *) self)->elements);

	super(Object, self, dealloc);
}

#pragma mark - Array

static void addObject(Array *self, const id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	if (self->count == self->capacity) {
		self->elements = realloc(self->elements, self->capacity + ARRAY_CHUNK_SIZE * sizeof(id));
		assert(self->elements);
	}

	self->elements[self->count++] = object;
}

static BOOL containsObject(const Array *self, const id obj) {

	return $(Array, self, indexOfObject, obj) != -1;
}

static int indexOfObject(const Array *self, const id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	for (size_t i = 0; i < self->count; i++) {
		if ($(Object, object, isEqual, (Object * ) self->elements[i])) {
			return (int) i;
		}
	}

	return -1;
}

static void removeObject(Array *self, const id obj) {

	int index = $(Array, self, indexOfObject, obj);

	if (index != -1) {
		for (size_t i = index; i < self->count; i++) {
			self->elements[i] = self->elements[i + 1];
		}

		self->count--;
	}
}

static void removeAllObjects(Array *self) {
	self->count = 0;
}

#pragma mark - Initializers

static void initialize(Class *class) {

	ArrayClass *array = (ArrayClass *) class;

	array->object.dealloc = dealloc;

	array->addObject = addObject;
	array->containsObject = containsObject;
	array->indexOfObject = indexOfObject;
	array->removeObject = removeObject;
	array->removeAllObjects = removeAllObjects;
}

static id init(id obj, va_list *args) {

	Array *self = (Array *) superclassof(obj)->init(obj, args);
	if (self) {
		self->capacity = arg(args, size_t, ARRAY_CHUNK_SIZE);
		self->elements = malloc(self->capacity * sizeof(id));
	}

	return self;
}

ArrayClass __Array = {
	.object.class = {
		.superclass = (Class *) &__Object,
		.name = "Array",
		.size = sizeof(ArrayClass),
		.initialize = initialize,
		.instanceSize = sizeof(Array),
		.init = init,
	},
};
