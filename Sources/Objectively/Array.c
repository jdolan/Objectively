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

#include <Objectively/Array.h>

#define __Class __Array

#define ARRAY_CHUNK_SIZE 64

#pragma mark - Object instance methods

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Array *array = (Array *) super(Object, self, copy);

	for (size_t i = 0; i < array->count; i++) {
		retain(array->elements[i]);
	}

	return (Object *) array;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	$(Array, self, removeAllObjects);

	Array *this = (Array *) self;

	free(this->elements);

	super(Object, self, dealloc);
}

#pragma mark - Array instance methods

/**
 * @see ArrayInterface::addObject(Array *, const id)
 */
static void addObject(Array *self, const id obj) {

	if (self->count == self->capacity) {

		self->capacity += ARRAY_CHUNK_SIZE;
		self->elements = realloc(self->elements, self->capacity * sizeof(id));

		assert(self->elements);
	}

	retain(obj);

	self->elements[self->count++] = obj;
}

/**
 * @see ArrayInterface::containsObject(const Array *, const id)
 */
static BOOL containsObject(const Array *self, const id obj) {

	return $(Array, self, indexOfObject, obj) != -1;
}

/**
 * @see ArrayInterface::enumerateObjects(const Array *, ArrayEnumerator, id)
 */
static void enumerateObjects(const Array *self, ArrayEnumerator enumerator, id data) {

	assert(enumerator);

	for (size_t i = 0; i < self->count; i++) {
		if (enumerator(self, self->elements[i], data)) {
			return;
		}
	}
}

/**
 * @see ArrayInterface::filterObjects(const Array *, ArrayEnumerator, id)
 */
static Array *filterObjects(const Array *self, ArrayEnumerator enumerator, id data) {

	assert(enumerator);

	Array *array = alloc(Array);

	for (size_t i = 0; i < self->count; i++) {
		if (enumerator(self, self->elements[i], data)) {
			$(Array, array, addObject, self->elements[i]);
		}
	}

	return array;
}

/**
 * @see ArrayInterface::indexOfObject(const Array *, const id)
 */
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

/**
 * @see ArrayInterface::init(Array *)
 */
static Array *init(Array *self) {

	return $(Array, self, initWithCapacity, ARRAY_CHUNK_SIZE);
}

/**
 * @see ArrayInterface::initWithCapacity(Array *, size_t)
 */
static Array *initWithCapacity(Array *self, size_t capacity) {

	self = (Array *) super(Object, self, init);
	if (self) {
		self->capacity = self->initialCapacity = capacity;
		self->elements = malloc(self->capacity * sizeof(id));
		assert(self->elements);
	}
	return self;
}

/**
 * @see ArrayInterface::objectAtIndex(const Array *, const int)
 */
static id objectAtIndex(const Array *self, const int index) {

	assert(index > -1);
	assert(index < self->count);

	return self->elements[index];
}

/**
 * @see ArrayInterface::removeAllObjects(Array *)
 */
static void removeAllObjects(Array *self) {

	for (size_t i = self->count; i > 0; i--) {
		$(Array, self, removeObjectAtIndex, i - 1);
	}
}

/**
 * @see ArrayInterface::removeObject(Array *, const id)
 */
static void removeObject(Array *self, const id obj) {

	int index = $(Array, self, indexOfObject, obj);
	if (index != -1) {
		$(Array, self, removeObjectAtIndex, index);
	}
}

/**
 * @see ArrayInterface::removeObjectAtIndex(Array *, const int)
 */
static void removeObjectAtIndex(Array *self, const int index) {

	assert(index > -1);
	assert(index < self->count);

	release(self->elements[index]);

	for (size_t i = index; i < self->count; i++) {
		self->elements[i] = self->elements[i + 1];
	}

	self->count--;
}

/**
 * @see ArrayInterface::resize(Array *)
 */
static void resize(Array *self) {

	size_t chunks = (self->count / ARRAY_CHUNK_SIZE) + 1;
	size_t capacity = chunks * ARRAY_CHUNK_SIZE;

	capacity = max(capacity, self->initialCapacity);
	if (capacity != self->capacity) {

		self->capacity = capacity;
		self->elements = realloc(self->elements, self->capacity * sizeof(id));

		assert(self->elements);
	}
}

/**
 * @see ArrayInterface::setObjectAtIndex(Array *, const id, const int)
 */
static void setObjectAtIndex(Array *self, const id obj, const int index) {

	assert(index > -1);
	assert(index < self->count);

	retain(obj);

	release(self->elements[index]);

	self->elements[index] = obj;
}

#pragma mark - Array class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	ArrayInterface *array = (ArrayInterface *) clazz->interface;

	array->addObject = addObject;
	array->containsObject = containsObject;
	array->enumerateObjects = enumerateObjects;
	array->filterObjects = filterObjects;
	array->indexOfObject = indexOfObject;
	array->init = init;
	array->initWithCapacity = initWithCapacity;
	array->objectAtIndex = objectAtIndex;
	array->removeAllObjects = removeAllObjects;
	array->removeObject = removeObject;
	array->removeObjectAtIndex = removeObjectAtIndex;
	array->resize = resize;
	array->setObjectAtIndex = setObjectAtIndex;
}

Class __Array = {
	.name = "Array",
	.superclass = &__Object,
	.instanceSize = sizeof(Array),
	.interfaceSize = sizeof(ArrayInterface),
	.initialize = initialize,
};

#undef __Class
