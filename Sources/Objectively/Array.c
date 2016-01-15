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
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const Array *this = (Array *) self;

	return (Object *) $(alloc(Array), initWithArray, this);
}

/**
 * @see Object::dealloc(Object *)
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
 * @see Object::description(const Object *)
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
 * @see Object::hash(const Object *)
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
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, &_Array)) {

		const Array *this = (Array *) self;
		const Array *that = (Array *) other;

		if (this->count == that->count) {

			for (size_t i = 0; i < this->count; i++) {

				const Object *thisObject = this->elements[i];
				const Object *thatObject = that->elements[i];

				if ($(thisObject, isEqual, thatObject) == false) {
					return false;
				}
			}

			return true;
		}
	}

	return false;
}

#pragma mark - ArrayInterface


/**
 * @fn Array *Array::arrayWithArray(const Array *array)
 *
 * @memberof Array
 */
static Array *arrayWithArray(const Array *array) {

	return $(alloc(Array), initWithArray, array);
}

/**
 * @fn Array *Array::arrayWithObjects(ident obj, ...)
 *
 * @memberof Array
 */
static Array *arrayWithObjects(ident obj, ...) {

	Array *array = (Array *) super(Object, alloc(Array), init);
	if (array) {

		va_list args;
		va_start(args, obj);

		ident object = obj;

		while (object) {
			array->count++;
			object = va_arg(args, ident);
		}

		va_end(args);

		if (array->count) {

			array->elements = calloc(array->count, sizeof(ident));
			assert(array->elements);

			va_start(args, obj);

			object = obj;
			ident *elt = array->elements;

			while (object) {
				*elt++ = retain(object);
				object = va_arg(args, ident);
			}

			va_end(args);
		}
	}

	return array;
}

/**
 * @fn _Bool Array::containsObject(const Array *self, const ident obj)
 *
 * @memberof Array
 */
static _Bool containsObject(const Array *self, const ident obj) {

	return $(self, indexOfObject, obj) != -1;
}

/**
 * @fn void Array::enumerateObjects(const Array *self, ArrayEnumerator enumerator, ident data)
 *
 * @memberof Array
 */
static void enumerateObjects(const Array *self, ArrayEnumerator enumerator, ident data) {

	assert(enumerator);

	for (size_t i = 0; i < self->count; i++) {
		if (enumerator(self, self->elements[i], data)) {
			return;
		}
	}
}

/**
 * @fn void Array::filterObjects(const Array *self, ArrayEnumerator enumerator, ident data)
 *
 * @memberof Array
 */
static Array *filterObjects(const Array *self, ArrayEnumerator enumerator, ident data) {

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
 * @fn int Array::indexOfObject(const Array *self, const ident obj)
 *
 * @memberof Array
 */
static int indexOfObject(const Array *self, const ident obj) {

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
 * @fn Array *Array::initWithArray(Array *self, const Array *array)
 *
 * @memberof Array
 */
static Array *initWithArray(Array *self, const Array *array) {

	self = (Array *) super(Object, self, init);
	if (self) {

		self->count = array->count;
		if (self->count) {

			self->elements = calloc(self->count, sizeof(ident));
			assert(self->elements);

			for (size_t i = 0; i < self->count; i++) {
				self->elements[i] = retain(array->elements[i]);
			}
		}
	}

	return self;
}

/**
 * @fn Array *Array::initWithObjects(Array *self, ...)
 *
 * @memberof Array
 */
static Array *initWithObjects(Array *self, ...) {

	self = (Array *) super(Object, self, init);
	if (self) {

		va_list args;
		va_start(args, self);

		while (va_arg(args, ident)) {
			self->count++;
		}

		va_end(args);

		if (self->count) {

			self->elements = calloc(self->count, sizeof(ident));
			assert(self->elements);

			va_start(args, self);

			for (size_t i = 0; i < self->count; i++) {
				self->elements[i] = retain(va_arg(args, ident));
			}

			va_end(args);
		}
	}

	return self;
}

/**
 * @fn MutableArray *Array::mutableCopy(const Array *self)
 *
 * @memberof Array
 */
static MutableArray *mutableCopy(const Array *self) {

	MutableArray *copy = $(alloc(MutableArray), initWithCapacity, self->count);
	if (copy) {
		$(copy, addObjectsFromArray, self);
	}

	return copy;
}

/**
 * @fn ident Array::objectAtIndex(const Array *self, int index)
 *
 * @memberof Array
 */
static ident objectAtIndex(const Array *self, int index) {

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
