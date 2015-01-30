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

#include "config.h"

#include <assert.h>
#include <stdlib.h>

#include <Objectively/MutableArray.h>

#define _Class _MutableArray

#define ARRAY_CHUNK_SIZE 64

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Array *this = (Array *)  self;

	MutableArray *copy = $(alloc(MutableArray), initWithCapacity, this->count);

	$(copy, addObjectsFromArray, this);

	return (Object *) copy;
}

#pragma mark - MutableArrayInterface

/**
 * @see MutableArrayInterface::addObject(MutableArray *, const id)
 */
static void addObject(MutableArray *self, const id obj) {

	Array *array = (Array *) self;
	if (array->count == self->capacity) {

		self->capacity += ARRAY_CHUNK_SIZE;

		if (array->elements) {
			array->elements = realloc(array->elements, self->capacity * sizeof(id));
		} else {
			array->elements = malloc(self->capacity * sizeof(id));
		}

		assert(array->elements);
	}

	array->elements[array->count++] = retain(obj);
}

/**
 * @see MutableArrayInterface::addObjectsFromArrayy
 */
static void addObjectsFromArray(MutableArray *self, const Array *array) {

	if (array) {
		for (size_t i = 0; i < array->count; i++) {
			$(self, addObject, array->elements[i]);
		}
	}
}

/**
 * @see MutableArrayInterface::array(void)
 */
static MutableArray *array(void) {

	return $(alloc(MutableArray), init);
}

/**
 * @see MutableArrayInterface::arrayWithCapacity(size_t)
 */
static MutableArray *arrayWithCapacity(size_t capacity) {

	return $(alloc(MutableArray), initWithCapacity, capacity);
}

/**
 * @see MutableArrayInterface::init(MutableArray *)
 */
static MutableArray *init(MutableArray *self) {

	return $(self, initWithCapacity, 0);
}

/**
 * @see MutableArrayInterface::initWithCapacity(MutableArray *, size_t)
 */
static MutableArray *initWithCapacity(MutableArray *self, size_t capacity) {

	self = (MutableArray *) super(Object, self, init);
	if (self) {

		self->capacity = capacity;
		if (self->capacity) {

			self->array.elements = malloc(self->capacity * sizeof(id));
			assert(self->array.elements);
		}
	}

	return self;
}

/**
 * @see MutableArrayInterface::removeAllObjects(MutableArray *)
 */
static void removeAllObjects(MutableArray *self) {

	for (size_t i = self->array.count; i > 0; i--) {
		$(self, removeObjectAtIndex, i - 1);
	}
}

/**
 * @see MutableArrayInterface::removeObject(MutableArray *, const id)
 */
static void removeObject(MutableArray *self, const id obj) {

	int index = $((Array *) self, indexOfObject, obj);
	if (index != -1) {
		$(self, removeObjectAtIndex, index);
	}
}

/**
 * @see MutableArrayInterface::removeObjectAtIndex(MutableArray *, const int)
 */
static void removeObjectAtIndex(MutableArray *self, const int index) {

	assert(index > -1);
	assert(index < self->array.count);

	release(self->array.elements[index]);

	for (size_t i = index; i < self->array.count; i++) {
		self->array.elements[i] = self->array.elements[i + 1];
	}

	self->array.count--;
}

/**
 * @see MutableArrayInterface::setObjectAtIndex(MutableArray *, const id, const int)
 */
static void setObjectAtIndex(MutableArray *self, const id obj, const int index) {

	assert(index > -1);
	assert(index < self->array.count);

	retain(obj);

	release(self->array.elements[index]);

	self->array.elements[index] = obj;
}

/**
 * @see MutableArrayInterface::sort(MutableArray *, Comparator)
 */
static void sort(MutableArray *self, Comparator comparator) {
	typedef int (*QsortComparator)(const void *a, const void *b);

	QsortComparator qsortComparator = (QsortComparator) comparator;

	qsort(self->array.elements, self->array.count, sizeof(id), qsortComparator);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;

	MutableArrayInterface *mutableArray = (MutableArrayInterface *) clazz->interface;

	mutableArray->addObject = addObject;
	mutableArray->addObjectsFromArray = addObjectsFromArray;
	mutableArray->array = array;
	mutableArray->arrayWithCapacity = arrayWithCapacity;
	mutableArray->init = init;
	mutableArray->initWithCapacity = initWithCapacity;
	mutableArray->removeAllObjects = removeAllObjects;
	mutableArray->removeObject = removeObject;
	mutableArray->removeObjectAtIndex = removeObjectAtIndex;
	mutableArray->setObjectAtIndex = setObjectAtIndex;
	mutableArray->sort = sort;
}

Class _MutableArray = {
	.name = "MutableArray",
	.superclass = &_Array,
	.instanceSize = sizeof(MutableArray),
	.interfaceOffset = offsetof(MutableArray, interface),
	.interfaceSize = sizeof(MutableArrayInterface),
	.initialize = initialize,
};

#undef _Class
