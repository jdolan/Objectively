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
#include <stdarg.h>
#include <stdlib.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableSet.h>
#include <Objectively/Set.h>
#include <Objectively/String.h>

#define _Class _Set

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const Set *this = (Set *) self;

	Set *that = $(alloc(Set), initWithSet, this);

	return (Object *) that;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Set *this = (Set *) self;

	for (size_t i = 0; i < this->capacity; i++) {
		release(this->elements[i]);
	}

	free(this->elements);

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	const Array *array = $((Set *) self, allObjects);

	return $((Object *) array, description);
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	const Set *this = (Set *) self;

	int hash = HashForInteger(HASH_SEED, this->count);

	for (size_t i = 0; i < this->capacity; i++) {
		if (this->elements[i]) {
			hash = HashForObject(hash, this->elements[i]);
		}
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

	if (other && (self->clazz == other->clazz)) {

		const Set *this = (Set *) self;
		const Set *that = (Set *) other;

		if (this->count == that->count) {

			Array *objects = $(this, allObjects);

			for (size_t i = 0; i < objects->count; i++) {
				const id obj = $(objects, objectAtIndex, i);

				if ($(that, containsObject, obj) == NO) {
					release(objects);
					return NO;
				}
			}

			release(objects);
			return YES;
		}
	}

	return NO;
}

#pragma mark - SetInterface

/**
 * @brief SetEnumerator for allObjects.
 */
static BOOL allObjects_enumerator(const Set *set, id obj, id data) {

	$((MutableArray *) data, addObject, obj); return NO;
}

/**
 * @see SetInterface::allObjects(const Set *)
 */
static Array *allObjects(const Set *self) {

	MutableArray *objects = $(alloc(MutableArray), initWithCapacity, self->count);

	$(self, enumerateObjects, allObjects_enumerator, objects);

	return (Array *) objects;
}

/**
 * @see SetInterface::containsObject(const Set *, const id)
 */
static BOOL containsObject(const Set *self, const id obj) {

	const size_t bin = HashForObject(HASH_SEED, obj) % self->capacity;

	const Array *array = self->elements[bin];
	if (array) {
		return $(array, containsObject, obj);
	}

	return NO;
}

/**
 * @see SetInterface::enumerateObjects(const Set *, SetEnumerator, id)
 */
static void enumerateObjects(const Set *self, SetEnumerator enumerator, id data) {

	assert(enumerator);

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array) {

			for (size_t j = 0; j < array->count; j++) {
				id obj = $(array, objectAtIndex, j);

				if (enumerator(self, obj, data)) {
					return;
				}
			}
		}
	}
}

/**
 * @see SetInterface::filterObjects(const Set *, SetEnumerator, id)
 */
static Set *filterObjects(const Set *self, SetEnumerator enumerator, id data) {

	assert(enumerator);

	MutableSet *set = $(alloc(MutableSet), init);

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array) {

			for (size_t j = 0; j < array->count; j++) {
				id obj = $(array, objectAtIndex, j);

				if (enumerator(self, obj, data)) {
					$(set, addObject, obj);
				}
			}
		}
	}

	return (Set *) set;
}

/**
 * @brief ArrayEnumerator for initWithArray.
 */
static BOOL initWithArray_enumerator(const Array *array, id obj, id data) {

	$$(MutableSet, addObject, (MutableSet *) data, obj); return NO;
}

/**
 * @see SetInterface::initWithArray(Set *, const Array *)
 */
static Set *initWithArray(Set *self, const Array *array) {

	self = (Set *) super(Object, self, init);
	if (self) {
		if (array) {
			$(array, enumerateObjects, initWithArray_enumerator, self);
		}
	}

	return self;
}

/**
 * @brief SetEnumerator for initWithSet.
 */
static BOOL initWithSet_enumerator(const Set *set, id obj, id data) {

	$$(MutableSet, addObject, (MutableSet *) data, obj); return NO;
}

/**
 * @see SetInterface::initWithSet(Set *, const Set *)
 */
static Set *initWithSet(Set *self, const Set *set) {

	self = (Set *) super(Object, self, init);
	if (self) {
		if (set) {
			$(set, enumerateObjects, initWithSet_enumerator, self);
		}
	}

	return self;
}

/**
 * @see SetInterface::initWithObjects(Set *, ...)
 */
static Set *initWithObjects(Set *self, ...) {

	self = (Set *) super(Object, self, init);
	if (self) {

		va_list args;
		va_start(args, self);

		while (YES) {

			id obj = va_arg(args, id);
			if (obj) {
				$$(MutableSet, addObject, (MutableSet *) self, obj);
			} else {
				break;
			}
		}

		va_end(args);
	}

	return self;
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

	SetInterface *set = (SetInterface *) clazz->interface;

	set->allObjects = allObjects;
	set->containsObject = containsObject;
	set->enumerateObjects = enumerateObjects;
	set->filterObjects = filterObjects;
	set->initWithArray = initWithArray;
	set->initWithSet = initWithSet;
	set->initWithObjects = initWithObjects;
}

Class _Set = {
	.name = "Set",
	.superclass = &_Object,
	.instanceSize = sizeof(Set),
	.interfaceOffset = offsetof(Set, interface),
	.interfaceSize = sizeof(SetInterface),
	.initialize = initialize,
};

#undef _Class

