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

#define ARRAY_CHUNK_SIZE 64

#pragma mark - Object instance methods

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Array *array = (Array *) super(Object, self, copy);

	for (size_t i = 0; i < array->count; i++) {
		retain(array->elements[i]);
	}

	return (Object *) array;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Array *this = (Array *) self;

	$(this, removeAllObjects);

	free(this->elements);

	super(Object, self, dealloc);
}

/**
 * @see Object::init(id, id, va_list *)
 */
static Object *init(id obj, id interface, va_list *args) {

	Array *self = (Array *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (ArrayInterface *) interface;

		self->capacity = self->initialCapacity = arg(args, size_t, ARRAY_CHUNK_SIZE);
		self->elements = malloc(self->capacity * sizeof(id));
	}

	return (Object *) self;
}

#pragma mark - Array instance methods

/**
 * @see addObject(Array *, const id)
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
 * @see Array::containsObject(const Array *, const id)
 */
static BOOL containsObject(const Array *self, const id obj) {

	return $(self, indexOfObject, obj) != -1;
}

/**
 * @see Array::enumerateObjects(const Array *, ArrayEnumerator, id)
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
 * @see Array::filterObjects(const Array *, ArrayEnumerator, id)
 */
static Array *filterObjects(const Array *self, ArrayEnumerator enumerator, id data) {

	assert(enumerator);

	Array *array = new(Array);

	for (size_t i = 0; i < self->count; i++) {
		if (enumerator(self, self->elements[i], data)) {
			$(array, addObject, self->elements[i]);
		}
	}

	return array;
}

/**
 * @see Array::indexOfObject(const Array *, const id)
 */
static int indexOfObject(const Array *self, const id obj) {

	Object *object = cast(Object, obj);

	assert(object);

	for (size_t i = 0; i < self->count; i++) {
		if ($(object, isEqual, (Object * ) self->elements[i])) {
			return (int) i;
		}
	}

	return -1;
}

/**
 * @see Array::objectAtIndex(const Array *, const int)
 */
static id objectAtIndex(const Array *self, const int index) {

	assert(index > -1);
	assert(index < self->count);

	return self->elements[index];
}

/**
 * @see Array::removeAllObjects(Array *)
 */
static void removeAllObjects(Array *self) {

	for (size_t i = self->count; i > 0; i--) {
		$(self, removeObjectAtIndex, i - 1);
	}
}

/**
 * @see Array::removeObject(Array *, const id)
 */
static void removeObject(Array *self, const id obj) {

	int index = $(self, indexOfObject, obj);
	if (index != -1) {
		$(self, removeObjectAtIndex, index);
	}
}

/**
 * @see Array::removeObjectAtIndex(Array *, const int)
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
 * @see Array::resize(Array *)
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
 * @see Array::setObjectAtIndex(Array *, const id, const int)
 */
static void setObjectAtIndex(Array *self, const id obj, const int index) {

	assert(index > -1);
	assert(index < self->count);

	release(self->elements[index]);

	retain(obj);

	self->elements[index] = obj;
}

#pragma mark - Array class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	ObjectInterface *object = (ObjectInterface *) self->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->init = init;

	ArrayInterface *array = (ArrayInterface *) self->interface;

	array->addObject = addObject;
	array->containsObject = containsObject;
	array->enumerateObjects = enumerateObjects;
	array->filterObjects = filterObjects;
	array->indexOfObject = indexOfObject;
	array->objectAtIndex = objectAtIndex;
	array->removeAllObjects = removeAllObjects;
	array->removeObject = removeObject;
	array->removeObjectAtIndex = removeObjectAtIndex;
	array->resize = resize;
	array->setObjectAtIndex = setObjectAtIndex;
}

Class __Array = {
	.name = "Array",
	.superclass = (Class *) &__Object,
	.instanceSize = sizeof(Array),
	.interfaceSize = sizeof(ArrayInterface),
	.initialize = initialize, };
