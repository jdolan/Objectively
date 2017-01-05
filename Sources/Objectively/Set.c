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

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableSet.h>
#include <Objectively/Set.h>
#include <Objectively/String.h>

#define _Class _Set

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const Set *this = (Set *) self;

	Set *that = $(alloc(Set), initWithSet, this);

	return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
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
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const Array *array = $((Set *) self, allObjects);

	return $((Object *) array, description);
}

/**
 * @see Object::hash(const Object *)
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
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && (self->clazz == other->clazz)) {

		const Set *this = (Set *) self;
		const Set *that = (Set *) other;

		if (this->count == that->count) {

			Array *objects = $(this, allObjects);

			for (size_t i = 0; i < objects->count; i++) {
				const ident obj = $(objects, objectAtIndex, i);

				if ($(that, containsObject, obj) == false) {
					release(objects);
					return false;
				}
			}

			release(objects);
			return true;
		}
	}

	return false;
}

#pragma mark - Set

/**
 * @brief SetEnumerator for allObjects.
 */
static void allObjects_enumerator(const Set *set, ident obj, ident data) {
	$((MutableArray *) data, addObject, obj);
}

/**
 * @fn Array *Set::allObjects(const Set *self)
 * @memberof Set
 */
static Array *allObjects(const Set *self) {

	MutableArray *objects = $(alloc(MutableArray), initWithCapacity, self->count);

	$(self, enumerateObjects, allObjects_enumerator, objects);

	return (Array *) objects;
}

/**
 * @fn _Bool Set::containsObject(const Set *self, const ident obj)
 * @memberof Set
 */
static _Bool containsObject(const Set *self, const ident obj) {

	const size_t bin = HashForObject(HASH_SEED, obj) % self->capacity;

	const Array *array = self->elements[bin];
	if (array) {
		return $(array, containsObject, obj);
	}

	return false;
}

/**
 * @fn void Set::enumerateObjects(const Set *self, SetEnumerator enumerator, ident data)
 * @memberof Set
 */
static void enumerateObjects(const Set *self, SetEnumerator enumerator, ident data) {

	assert(enumerator);

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array) {

			for (size_t j = 0; j < array->count; j++) {
				enumerator(self, $(array, objectAtIndex, j), data);
			}
		}
	}
}

/**
 * @fn void Set::filteredSet(const Set *self, Predicate predicate, ident data)
 * @memberof Set
 */
static Set *filteredSet(const Set *self, Predicate predicate, ident data) {

	assert(predicate);

	MutableSet *set = $(alloc(MutableSet), init);

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array) {

			for (size_t j = 0; j < array->count; j++) {
				ident obj = $(array, objectAtIndex, j);

				if (predicate(obj, data)) {
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
static void initWithArray_enumerator(const Array *array, ident obj, ident data) {
	$$(MutableSet, addObject, (MutableSet *) data, obj);
}

/**
 * @fn Set *Set::initWithArray(Set *self, const Array *array)
 * @memberof Set
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
 * @fn Set *Set::initWithObjects(Set *self, ...)
 * @memberof Set
 */
static Set *initWithObjects(Set *self, ...) {

	self = (Set *) super(Object, self, init);
	if (self) {

		va_list args;
		va_start(args, self);

		while (true) {

			ident obj = va_arg(args, ident);
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

/**
 * @brief SetEnumerator for initWithSet.
 */
static void initWithSet_enumerator(const Set *set, ident obj, ident data) {
	$$(MutableSet, addObject, (MutableSet *) data, obj);
}

/**
 * @fn Set *Set::initWithSet(Set *self, const Set *set)
 * @memberof Set
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
 * @fn Set *Set::setWithArray(const Array *array)
 * @memberof Set
 */
static Set *setWithArray(const Array *array) {

	return $(alloc(Set), initWithArray, array);
}

/**
 * @fn Set *Set::setWithObjects(ident obj, ...)
 * @memberof Set
 */
static Set *setWithObjects(ident obj, ...) {

	Set *set = (Set *) $((Object *) alloc(Set), init);
	if (set) {

		va_list args;
		va_start(args, obj);

		while (obj) {
			$$(MutableSet, addObject, (MutableSet * ) set, obj);
			obj = va_arg(args, ident);
		}

		va_end(args);
	}

	return set;
}

/**
 * @fn Set *Set::setWithSet(const Set *set)
 * @memberof Set
 */
static Set *setWithSet(const Set *set) {

	return $(alloc(Set), initWithSet, set);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->isEqual = isEqual;

	SetInterface *set = (SetInterface *) clazz->def->interface;

	set->allObjects = allObjects;
	set->containsObject = containsObject;
	set->enumerateObjects = enumerateObjects;
	set->filteredSet = filteredSet;
	set->initWithArray = initWithArray;
	set->initWithSet = initWithSet;
	set->initWithObjects = initWithObjects;
	set->setWithArray = setWithArray;
	set->setWithObjects = setWithObjects;
	set->setWithSet = setWithSet;
}

Class *_Set(void) {
	static Class _class;
	
	if (!_class.name) {
		_class.name = "Set";
		_class.superclass = _Object();
		_class.instanceSize = sizeof(Set);
		_class.interfaceOffset = offsetof(Set, interface);
		_class.interfaceSize = sizeof(SetInterface);
		_class.initialize = initialize;
	}

	return &_class;
}

#undef _Class

