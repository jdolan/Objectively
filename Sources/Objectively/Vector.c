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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Hash.h"
#include "Vector.h"

#define _Class _Vector

#define VECTOR_CHUNK_SIZE 64

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const Vector *this = (Vector *) self;

	Vector *copy = (Vector *) $(self, copy);

	copy->elements = malloc(this->capacity * this->size);
	memcpy(copy->elements, this->elements, this->count * this->size);

	return (Object *) copy;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Vector *this = (Vector *) self;

	free(this->elements);

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Vector *this = (Vector *) self;

	const Range range = {
		.location = 0,
		.length = this->count * this->size
	};

	return HashForBytes(HASH_SEED, this->elements, range);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _Vector())) {

		const Vector *this = (Vector *) self;
		const Vector *that = (Vector *) other;

		if (this->count == that->count) {
			return memcmp(this->elements, that->elements, this->count * this->size) == 0;
		}
	}

	return false;
}

#pragma mark - Vector

/**
 * @fn void Vector::addElement(Vector *self, const ident element)
 * @memberof Vector
 */
static void addElement(Vector *self, const ident element) {

	if (self->count == self->capacity) {

		self->capacity += VECTOR_CHUNK_SIZE;

		if (self->elements) {
			self->elements = realloc(self->elements, self->capacity * self->size);
		} else {
			self->elements = calloc(self->capacity, self->size);
		}

		assert(self->elements);
	}

	memcpy(self->elements + self->count * self->size, element, self->size);
	self->count++;
}

/**
 * @fn void Vector::enumerateObjects(const Vector *self, VectorEnumerator enumerator)
 * @memberof Vector
 */
static void enumerateElements(const Vector *self, VectorEnumerator enumerator, ident data) {

	for (size_t i = 0; i < self->count; i++) {
		enumerator(self, self->elements + i * self->size, data);
	}
}

/**
 * @fn ssize_t Vector::indexOfElement(const Vector *self, const ident element)
 * @memberof Vector
 */
static ssize_t indexOfElement(const Vector *self, const ident element) {

	for (size_t i = 0; i < self->count; i++) {
		if (memcmp(self->elements + i * self->size, element, self->size) == 0) {
			return i;
		}
	}

	return -1;
}

/**
 * @fn Vector *Vector::initWithElements(Vector *self, size_t size, size_t count, ident elements)
 * @memberof Vector
 */
static Vector *initWithElements(Vector *self, size_t size, size_t count, ident elements) {

	self = $(self, initWithSize, size);
	if (self) {
		self->count = count;
		self->elements = elements;
	}

	return self;
}

/**
 * @fn Vector *Vector::initWithSize(Vector *self, size_t size)
 * @memberof Vector
 */
static Vector *initWithSize(Vector *self, size_t size) {

	self = (Vector *) super(Object, self, init);
	if (self) {
		self->size = size;
		assert(self->size);
	}
	return self;
}

/**
 * @fn void Vector::insertElementAtIndex(Vector *self, const ident element, size_t index)
 * @memberof Vector
 */
static void insertElementAtIndex(Vector *self, const ident element, size_t index) {

	assert(index <= self->count);

	$(self, addElement, element);

	for (size_t i = self->count - 1; i > index; i--) {
		memcpy(self->elements + i * self->size, self->elements + (i - 1) * self->size, self->size);
	}

	memcpy(self->elements + index * self->size, element, self->size);
}

/**
 * @fn void Vector::removeElementAtIndex(Vector *self, size_t index)
 * @memberof Vector
 */
static void removeElementAtIndex(Vector *self, size_t index) {

	assert(index < self->count);

	const size_t size = (self->count - index) * self->size;

	memcpy(self->elements + index * self->size, self->elements + (index + 1) * self->size, size);

	self->count--;
}

/**
 * @fn void Vector::resize(Vector *self, size_t capacity)
 * @memberof Vetor
 */
static void resize(Vector *self, size_t capacity) {

	self->elements = realloc(self->elements, capacity * self->size);
	assert(self->elements);

	self->capacity = capacity;
	self->count = min(self->count, self->capacity);
}

/**
 * @fn Vector *Vector::vectorWithSize(size_t size)
 * @memberof Vector
 */
static Vector *vectorWithSize(size_t size) {
	return $(alloc(Vector), initWithSize, size);
}

/**
 * @fn Vector *Vector::vectorWithElements(size_t size, size_t count, ident elements)
 * @memberof Vector
 */
static Vector *vectorWithElements(size_t size, size_t count, ident elements) {
	return $(alloc(Vector), initWithElements, size, count, elements);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->hash = hash;
	((ObjectInterface *) clazz->interface)->isEqual = isEqual;

	((VectorInterface *) clazz->interface)->addElement = addElement;
	((VectorInterface *) clazz->interface)->enumerateElements = enumerateElements;
	((VectorInterface *) clazz->interface)->indexOfElement = indexOfElement;
	((VectorInterface *) clazz->interface)->initWithElements = initWithElements;
	((VectorInterface *) clazz->interface)->initWithSize = initWithSize;
	((VectorInterface *) clazz->interface)->insertElementAtIndex = insertElementAtIndex;
	((VectorInterface *) clazz->interface)->removeElementAtIndex = removeElementAtIndex;
	((VectorInterface *) clazz->interface)->resize = resize;
	((VectorInterface *) clazz->interface)->vectorWithSize = vectorWithSize;
	((VectorInterface *) clazz->interface)->vectorWithElements = vectorWithElements;
}

/**
 * @fn Class *Vector::_Vector(void)
 * @memberof Vector
 */
Class *_Vector(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Vector",
			.superclass = _Object(),
			.instanceSize = sizeof(Vector),
			.interfaceOffset = offsetof(Vector, interface),
			.interfaceSize = sizeof(VectorInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
