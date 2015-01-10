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
#include <stdarg.h>
#include <stdlib.h>

#include <Objectively/Array.h>
#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableString.h>

#define _Class _Array

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const Array *this = (Array *) self;

	return (Object *) $(alloc(Array), initWithArray, this);
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Array *this = (Array *) self;

	for (size_t i = 0; i < this->count; i++) {
		release(this->elements[i]);
	}

	free(this->elements);

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	const Array *this = (Array *) self;

	MutableString *desc = $(alloc(MutableString), init);
	$(desc, appendFormat, "[");

	for (size_t i = 0; i < this->count; i++) {
		String *objDesc = $((Object *) this->elements[i], description);

		$(desc, appendString, objDesc);
		release(objDesc);

		if (i < this->count - 1) {
			$(desc, appendFormat, ", ");
		}
	}

	$(desc, appendFormat, "]");
	return (String *) desc;
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	Array *this = (Array *) self;

	int hash = HashForInteger(HASH_SEED, this->count);

	for (size_t i = 0; i < this->count; i++) {
		hash = HashForObject(hash, this->elements[i]);
	}

	return hash;
}

/**
 * @see ObjectInterface::isEqual(const Object *, const Object *)
 */
static BOOL isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return YES;
	}

	if (other && $(other, isKindOfClass, &_Array)) {

		const Array *this = (Array *) self;
		const Array *that = (Array *) other;

		if (this->count == that->count) {

			for (size_t i = 0; i < this->count; i++) {

				const Object *thisObject = this->elements[i];
				const Object *thatObject = that->elements[i];

				if ($(thisObject, isEqual, thatObject) == NO) {
					return NO;
				}
			}

			return YES;
		}
	}

	return NO;
}

#pragma mark - ArrayInterface


/**
 * @see ArrayInterface::arrayWithArray(const Array *)
 */
static Array *arrayWithArray(const Array *array) {

	return $(alloc(Array), initWithArray, array);
}

/**
 * @see ArrayInterface::arrayWithObjects(id, ...)
 */
static Array *arrayWithObjects(id obj, ...) {

	Array *array = (Array *) super(Object, alloc(Array), init);
	if (array) {

		va_list args;
		va_start(args, obj);

		id object = obj;

		while (object) {
			array->count++;
			object = va_arg(args, id);
		}

		va_end(args);

		if (array->count) {

			array->elements = calloc(array->count, sizeof(id));
			assert(array->elements);

			va_start(args, obj);

			object = obj;
			id *elt = array->elements;

			while (object) {
				*elt++ = retain(object);
				object = va_arg(args, id);
			}

			va_end(args);
		}
	}

	return array;
}

/**
 * @see ArrayInterface::containsObject(const Array *, const id)
 */
static BOOL containsObject(const Array *self, const id obj) {

	return $(self, indexOfObject, obj) != -1;
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

	MutableArray *array = $(alloc(MutableArray), init);

	for (size_t i = 0; i < self->count; i++) {
		if (enumerator(self, self->elements[i], data)) {
			$(array, addObject, self->elements[i]);
		}
	}

	return (Array *) array;
}

/**
 * @see ArrayInterface::indexOfObject(const Array *, const id)
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
 * @see ArrayInterface::initWithArray(Array *, const Array *)
 */
static Array *initWithArray(Array *self, const Array *array) {

	self = (Array *) super(Object, self, init);
	if (self) {

		self->count = array->count;
		if (self->count) {

			self->elements = calloc(self->count, sizeof(id));
			assert(self->elements);

			for (size_t i = 0; i < self->count; i++) {
				self->elements[i] = retain(array->elements[i]);
			}
		}
	}

	return self;
}

/**
 * @see ArrayInterface::initWithObjects(Array *, ...)
 */
static Array *initWithObjects(Array *self, ...) {

	self = (Array *) super(Object, self, init);
	if (self) {

		va_list args;
		va_start(args, self);

		while (va_arg(args, id)) {
			self->count++;
		}

		va_end(args);

		if (self->count) {

			self->elements = calloc(self->count, sizeof(id));
			assert(self->elements);

			va_start(args, self);

			for (size_t i = 0; i < self->count; i++) {
				self->elements[i] = retain(va_arg(args, id));
			}

			va_end(args);
		}
	}

	return self;
}

/**
 * @see ArrayInterface::mutableCopy(const Array *)
 */
static MutableArray *mutableCopy(const Array *self) {

	MutableArray *copy = $(alloc(MutableArray), initWithCapacity, self->count);
	if (copy) {
		$(copy, addObjectsFromArray, self);
	}

	return copy;
}

/**
 * @see ArrayInterface::objectAtIndex(const Array *, const int)
 */
static id objectAtIndex(const Array *self, const int index) {

	assert(index > -1);
	assert(index < self->count);

	return self->elements[index];
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->isEqual = isEqual;

	ArrayInterface *array = (ArrayInterface *) clazz->interface;

	array->arrayWithArray = arrayWithArray;
	array->arrayWithObjects = arrayWithObjects;
	array->containsObject = containsObject;
	array->enumerateObjects = enumerateObjects;
	array->filterObjects = filterObjects;
	array->indexOfObject = indexOfObject;
	array->initWithArray = initWithArray;
	array->initWithObjects = initWithObjects;
	array->mutableCopy = mutableCopy;
	array->objectAtIndex = objectAtIndex;
}

Class _Array = {
	.name = "Array",
	.superclass = &_Object,
	.instanceSize = sizeof(Array),
	.interfaceOffset = offsetof(Array, interface),
	.interfaceSize = sizeof(ArrayInterface),
	.initialize = initialize,
};

#undef _Class
