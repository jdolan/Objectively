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

#define _Class _MutableSet

#define MUTABLESET_DEFAULT_CAPACITY 64
#define MUTABLESET_GROW_FACTOR 2.0
#define MUTABLESET_MAX_LOAD 0.75

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Set *this = (Set *) self;

	MutableSet *copy = $(alloc(MutableSet), initWithCapacity, this->capacity);

	$(copy, addObjectsFromSet, this);

	return (Object *) copy;
}

#pragma mark - MutableSet

/**
 * @brief A helper for resizing Sets as Objects are added to them.
 * @remarks Static method invocations are used for all operations.
 */
static void addObject_resize(Set *set) {

	if (set->capacity) {

		const float load = set->count / (float) set->capacity;
		if (load >= MUTABLESET_MAX_LOAD) {

			size_t capacity = set->capacity;
			ident *elements = set->elements;

			set->capacity = set->capacity * MUTABLESET_GROW_FACTOR;
			set->count = 0;

			set->elements = calloc(set->capacity, sizeof(ident));
			assert(set->elements);

			for (size_t i = 0; i < capacity; i++) {

				Array *array = elements[i];
				if (array) {
					$$(MutableSet, addObjectsFromArray, (MutableSet *) set, array);
					release(array);
				}
			}

			free(elements);
		}
	} else {
		$$(MutableSet, initWithCapacity, (MutableSet *) set, MUTABLESET_DEFAULT_CAPACITY);
	}
}

/**
 * @fn void MutableSet::addObject(MutableSet *self, const ident obj)
 * @memberof MutableSet
 */
static void addObject(MutableSet *self, const ident obj) {

	Set *set = (Set *) self;

	addObject_resize(set);

	const size_t bin = HashForObject(HASH_SEED, obj) % set->capacity;

	MutableArray *array = set->elements[bin];
	if (array == NULL) {
		array = set->elements[bin] = $(alloc(MutableArray), init);
	}

	if ($((Array *) array, containsObject, obj) == false) {
		$(array, addObject, obj);
		set->count++;
	}
}

/**
 * @brief ArrayEnumerator for addObjectsFromArray.
 */
static void addObjectsFromArray_enumerator(const Array *array, ident obj, ident data) {
	$((MutableSet *) data, addObject, obj);
}

/**
 * @fn void MutableSet::addObjectsFromArray(MutableSet *self, const Array *array)
 * @memberof MutableSet
 */
static void addObjectsFromArray(MutableSet *self, const Array *array) {

	if (array) {
		$(array, enumerateObjects, addObjectsFromArray_enumerator, self);
	}
}

/**
 * @brief SetEnumerator for addObjectsFromSet.
 */
static void addObjectsFromSet_enumerator(const Set *set, ident obj, ident data) {
	$((MutableSet *) data, addObject, obj);
}

/**
 * @fn void MutableSet::addObjectsFromSet(MutableSet *self, const Set *set)
 * @memberof MutableSet
 */
static void addObjectsFromSet(MutableSet *self, const Set *set) {

	if (set) {
		$(set, enumerateObjects, addObjectsFromSet_enumerator, self);
	}
}

/**
 * @fn void MutableSet::filter(MutableSet *self, Predicate predicate, ident data)
 * @memberof MutableSet
 */
static void filter(MutableSet *self, Predicate predicate, ident data) {

	assert(predicate);

	self->set.count = 0;

	for (size_t i = 0; i < self->set.capacity; i++) {

		MutableArray *array = self->set.elements[i];
		if (array) {

			$(array, filter, predicate, data);

			if (array->array.count == 0) {
				release(array);
				self->set.elements[i] = NULL;
			} else {
				self->set.count += array->array.count;
			}
		}
	}
}

/**
 * @fn MutableSet *MutableSet::init(MutableSet *self)
 * @memberof MutableSet
 */
static MutableSet *init(MutableSet *self) {

	return $(self, initWithCapacity, MUTABLESET_DEFAULT_CAPACITY);
}

/**
 * @fn MutableSet *MutableSet::initWithCapacity(MutableSet *self, size_t capacity)
 * @memberof MutableSet
 */
static MutableSet *initWithCapacity(MutableSet *self, size_t capacity) {

	self = (MutableSet *) super(Object, self, init);
	if (self) {

		self->set.capacity = capacity;
		if (self->set.capacity) {

			self->set.elements = calloc(self->set.capacity, sizeof(ident));
			assert(self->set.elements);
		}
	}

	return self;
}

/**
 * @fn void MutableSet::removeAllObjects(MutableSet *self)
 * @memberof MutableSet
 */
static void removeAllObjects(MutableSet *self) {

	for (size_t i = 0; i < self->set.capacity; i++) {

		Array *array = self->set.elements[i];
		if (array) {
			release(array);
			self->set.elements[i] = NULL;
		}
	}

	self->set.count = 0;
}

/**
 * @fn void MutableSet::removeObject(MutableSet *self, const ident obj)
 * @memberof MutableSet
 */
static void removeObject(MutableSet *self, const ident obj) {

	const size_t bin = HashForObject(HASH_SEED, obj) % self->set.capacity;

	MutableArray *array = self->set.elements[bin];
	if (array) {

		const ssize_t index = $((Array *) array, indexOfObject, obj);
		if (index > -1) {

			$(array, removeObjectAtIndex, index);

			if (((Array *) array)->count == 0) {
				release(array);
				self->set.elements[bin] = NULL;
			}

			self->set.count--;
		}
	}
}

/**
 * @fn MutableSet *MutableSet::set(void)
 * @memberof MutableSet
 */
static MutableSet *set(void) {

	return $(alloc(MutableSet), init);
}

/**
 * @fn MutableSet *MutableSet::setWithCapacity(size_t capacity)
 * @memberof MutableSet
 */
static MutableSet *setWithCapacity(size_t capacity) {

	return $(alloc(MutableSet), initWithCapacity, capacity);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;

	((MutableSetInterface *) clazz->interface)->addObject = addObject;
	((MutableSetInterface *) clazz->interface)->addObjectsFromArray = addObjectsFromArray;
	((MutableSetInterface *) clazz->interface)->addObjectsFromSet = addObjectsFromSet;
	((MutableSetInterface *) clazz->interface)->filter = filter;
	((MutableSetInterface *) clazz->interface)->init = init;
	((MutableSetInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
	((MutableSetInterface *) clazz->interface)->removeAllObjects = removeAllObjects;
	((MutableSetInterface *) clazz->interface)->removeObject = removeObject;
	((MutableSetInterface *) clazz->interface)->set = set;
	((MutableSetInterface *) clazz->interface)->setWithCapacity = setWithCapacity;
}

/**
 * @fn Class *MutableSet::_MutableSet(void)
 * @memberof MutableSet
 */
Class *_MutableSet(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "MutableSet",
			.superclass = _Set(),
			.instanceSize = sizeof(MutableSet),
			.interfaceOffset = offsetof(MutableSet, interface),
			.interfaceSize = sizeof(MutableSetInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
