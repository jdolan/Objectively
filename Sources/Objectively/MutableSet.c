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

#pragma mark - ObjectInterface

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Set *this = (Set *) self;

	MutableSet *copy = $(alloc(MutableSet), initWithCapacity, this->capacity);

	$(copy, addObjectsFromSet, this);

	return (Object *) copy;
}

#pragma mark - MutableSetInterface

/**
 * @brief A helper for resizing Sets as Objects are added to them.
 *
 * @remark Static method invocations are used for all operations.
 */
static void addObject_resize(Set *set) {

	if (set->capacity) {

		const float load = set->count / set->capacity;
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
 * @see MutableSet::addObject(MutableSet *, const id)
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
 * @see MutableSet::addObjectsFromArray(MutableSet *, const Array *)
 */
static void addObjectsFromArray(MutableSet *self, const Array *array) {

	if (array) {
		for (size_t i = 0; i < array->count; i++) {
			_call(self, addObject, $(array, objectAtIndex, i));
		}
	}
}

/**
 * @brief SetEnumerator for addObjectsFromSet.
 */
static _Bool addObjectsFromSet_enumerator(const Set *set, ident obj, ident data) {

	$((MutableSet *) data, addObject, obj); return false;
}

/**
 * @see MutableSet::addEntriesFromSet(MutableSet *, const Set *)
 */
static void addObjectsFromSet(MutableSet *self, const Set *set) {

	if (set) {
		$(set, enumerateObjects, addObjectsFromSet_enumerator, self);
	}
}

/**
 * @see MutableSet::init(MutableSet *)
 */
static MutableSet *init(MutableSet *self) {

	return $(self, initWithCapacity, MUTABLESET_DEFAULT_CAPACITY);
}

/**
 * @see MutableSet::initWithCapacity(MutableSet *, size_t)
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
 * @see MutableSet::removeAllObjects(MutableSet *)
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
 * @see MutableSet::removeObjectForKey(MutableSet *, const id)
 */
static void removeObject(MutableSet *self, const ident obj) {

	const size_t bin = HashForObject(HASH_SEED, obj) % self->set.capacity;

	MutableArray *array = self->set.elements[bin];
	if (array) {

		int index = $((Array *) array, indexOfObject, obj);
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
 * @see MutableSet::set(void)
 */
static MutableSet *set(void) {

	return $(alloc(MutableSet), init);
}

/**
 * @see MutableSet::initWithCapacity(size_t)
 */
static MutableSet *setWithCapacity(size_t capacity) {

	return $(alloc(MutableSet), initWithCapacity, capacity);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;

	MutableSetInterface *mutableSet = (MutableSetInterface *) clazz->interface;

	mutableSet->addObject = addObject;
	mutableSet->addObjectsFromArray = addObjectsFromArray;
	mutableSet->addObjectsFromSet = addObjectsFromSet;
	mutableSet->init = init;
	mutableSet->initWithCapacity = initWithCapacity;
	mutableSet->removeAllObjects = removeAllObjects;
	mutableSet->removeObject = removeObject;
	mutableSet->set = set;
	mutableSet->setWithCapacity = setWithCapacity;
}

Class _MutableSet = {
	.name = "MutableSet",
	.superclass = &_Set,
	.instanceSize = sizeof(MutableSet),
	.interfaceOffset = offsetof(MutableSet, interface),
	.interfaceSize = sizeof(MutableSetInterface),
	.initialize = initialize,
};

#undef _Class
