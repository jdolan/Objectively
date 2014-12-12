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
#include <Objectively/MutableDictionary.h>

#define __Class __MutableDictionary

#define DICTIONARY_DEFAULT_CAPACITY 64

#pragma mark - MutableDictionaryInterface

/**
 * @see MutableDictionaryInterface::init(MutableDictionary *)
 */
static MutableDictionary *init(MutableDictionary *self) {

	return $(self, initWithCapacity, DICTIONARY_DEFAULT_CAPACITY);
}

/**
 * @see MutableDictionaryInterface::initWithCapacity(MutableDictionary *, size_t)
 */
static MutableDictionary *initWithCapacity(MutableDictionary *self, size_t capacity) {

	self = (MutableDictionary *) super(Object, self, init);
	if (self) {
		self->dictionary.capacity = capacity;
		self->dictionary.elements = calloc(capacity, sizeof(Array *));
		assert(self->dictionary.elements);
	}

	return self;
}

/**
 * @see MutableDictionaryInterface::removeAllObjects(MutableDictionary *)
 */
static void removeAllObjects(MutableDictionary *self) {

	for (size_t i = 0; i < self->dictionary.capacity; i++) {

		Array *array = self->dictionary.elements[i];
		if (array != NULL) {
			release(array);
			self->dictionary.elements[i] = NULL;
		}
	}

	self->dictionary.count = 0;
}

/**
 * @see MutableDictionaryInterface::removeObjectForKey(MutableDictionary *, const id)
 */
static void removeObjectForKey(MutableDictionary *self, const id key) {

	assert(cast(Object, key));

	const size_t bin = HashForObject(HASH_SEED, key) % self->dictionary.capacity;

	MutableArray *array = self->dictionary.elements[bin];
	if (array != NULL) {

		int index = $((Array *) array, indexOfObject, key);
		if (index > -1) {

			$(array, removeObjectAtIndex, index);
			$(array, removeObjectAtIndex, index);

			if (((Array *) array)->count == 0) {
				release(array);
				self->dictionary.elements[bin] = NULL;
			}

			self->dictionary.count--;
		}
	}
}

/**
 * @see MutableDictionaryInterface::setObjectForKey(MutableDictionary *, const id, const id)
 */
static void setObjectForKey(MutableDictionary *self, const id obj, const id key) {

	assert(cast(Object, obj));
	assert(cast(Object, key));

	Dictionary *dict = (Dictionary *) self;

	const size_t bin = HashForObject(HASH_SEED, key) % dict->capacity;

	MutableArray *array = dict->elements[bin];
	if (array == NULL) {
		array = dict->elements[bin] = $(alloc(MutableArray), init);
	}

	const int index = $((Array *) array, indexOfObject, key);
	if (index > -1) {
		$(array, setObjectAtIndex, obj, index + 1);
	} else {
		$(array, addObject, key);
		$(array, addObject, obj);

		dict->count++;
	}
}

/**
 * @see MutableDictionaryInterface::setObjectsForKeys(MutableDictionary *, ...)
 */
static void setObjectsForKeys(MutableDictionary *self, ...) {

	va_list args;
	va_start(args, self);

	while (YES) {

		id obj = va_arg(args, id);
		if (obj) {

			id key = va_arg(args, id);
			$(self, setObjectForKey, obj, key);
		} else {
			break;
		}
	}

	va_end(args);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	MutableDictionaryInterface *mutableDictionary = (MutableDictionaryInterface *) clazz->interface;

	mutableDictionary->init = init;
	mutableDictionary->initWithCapacity = initWithCapacity;
	mutableDictionary->removeAllObjects = removeAllObjects;
	mutableDictionary->removeObjectForKey = removeObjectForKey;
	mutableDictionary->setObjectForKey = setObjectForKey;
	mutableDictionary->setObjectsForKeys = setObjectsForKeys;
}

Class __MutableDictionary = {
	.name = "MutableDictionary",
	.superclass = &__Dictionary,
	.instanceSize = sizeof(MutableDictionary),
	.interfaceOffset = offsetof(MutableDictionary, interface),
	.interfaceSize = sizeof(MutableDictionaryInterface),
	.initialize = initialize,
};

#undef __Class
