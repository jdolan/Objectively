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
#include <Objectively/MutableDictionary.h>

#define _Class _MutableDictionary

#define MUTABLEDICTIONARY_DEFAULT_CAPACITY 64
#define MUTABLEDICTIONARY_GROW_FACTOR 2.0
#define MUTABLEDICTIONARY_MAX_LOAD 0.75

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Dictionary *this = (Dictionary *) self;

	MutableDictionary *copy = $(alloc(MutableDictionary), initWithCapacity, this->capacity);

	$(copy, addEntriesFromDictionary, this);

	return (Object *) copy;
}

#pragma mark - MutableDictionaryInterface

/**
 * @brief DictionaryEnumerator for addEntriesFromDictionary.
 */
static BOOL addEntriesFromDictionary_enumerator(const Dictionary *dict, id obj, id key, id data) {

	$((MutableDictionary *) data, setObjectForKey, obj, key); return NO;
}

/**
 * @see MutableDictionaryInterface::addEntriesFromDictionary(MutableDictionary *, const Dictionary *)
 */
static void addEntriesFromDictionary(MutableDictionary *self, const Dictionary *dictionary) {

	$(dictionary, enumerateObjectsAndKeys, addEntriesFromDictionary_enumerator, self);
}

/**
 * @see MutableDictionaryInterface::dictionary(void)
 */
static MutableDictionary *dictionary(void) {

	return $(alloc(MutableDictionary), init);
}

/**
 * @see MutableDictionaryInterface::dictionaryWithCapacity(size_t)
 */
static MutableDictionary *dictionaryWithCapacity(size_t capacity) {

	return $(alloc(MutableDictionary), initWithCapacity, capacity);
}

/**
 * @see MutableDictionaryInterface::init(MutableDictionary *)
 */
static MutableDictionary *init(MutableDictionary *self) {

	return $(self, initWithCapacity, MUTABLEDICTIONARY_DEFAULT_CAPACITY);
}

/**
 * @see MutableDictionaryInterface::initWithCapacity(MutableDictionary *, size_t)
 */
static MutableDictionary *initWithCapacity(MutableDictionary *self, size_t capacity) {

	self = (MutableDictionary *) super(Object, self, init);
	if (self) {

		self->dictionary.capacity = capacity;
		if (self->dictionary.capacity) {

			self->dictionary.elements = calloc(self->dictionary.capacity, sizeof(id));
			assert(self->dictionary.elements);
		}
	}

	return self;
}

/**
 * @see MutableDictionaryInterface::removeAllObjects(MutableDictionary *)
 */
static void removeAllObjects(MutableDictionary *self) {

	for (size_t i = 0; i < self->dictionary.capacity; i++) {

		Array *array = self->dictionary.elements[i];
		if (array) {
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

	const size_t bin = HashForObject(HASH_SEED, key) % self->dictionary.capacity;

	MutableArray *array = self->dictionary.elements[bin];
	if (array) {

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
 * @brief A helper for resizing Dictionaries as pairs are added to them.
 *
 * @remark Static method invocations are used for all operations.
 */
static void setObjectForKey_resize(Dictionary *dict) {

	if (dict->capacity) {

		const float load = dict->count / dict->capacity;
		if (load >= MUTABLEDICTIONARY_MAX_LOAD) {

			size_t capacity = dict->capacity;
			id *elements = dict->elements;

			dict->capacity = dict->capacity * MUTABLEDICTIONARY_GROW_FACTOR;
			dict->count = 0;

			dict->elements = calloc(dict->capacity, sizeof(id));
			assert(dict->elements);

			for (size_t i = 0; i < capacity; i++) {

				Array *array = elements[i];
				if (array) {

					for (size_t j = 0; j < array->count; j += 2) {

						id key = $(array, objectAtIndex, j);
						id obj = $(array, objectAtIndex, j + 1);

						$$(MutableDictionary, setObjectForKey, (MutableDictionary *) dict, obj, key);
					}

					release(array);
				}
			}

			free(elements);
		}
	} else {
		$$(MutableDictionary, initWithCapacity, (MutableDictionary *) dict, MUTABLEDICTIONARY_DEFAULT_CAPACITY);
	}
}

/**
 * @see MutableDictionaryInterface::setObjectForKey(MutableDictionary *, const id, const id)
 */
static void setObjectForKey(MutableDictionary *self, const id obj, const id key) {

	Dictionary *dict = (Dictionary *) self;

	setObjectForKey_resize(dict);

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

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;

	MutableDictionaryInterface *mutableDictionary = (MutableDictionaryInterface *) clazz->interface;

	mutableDictionary->addEntriesFromDictionary = addEntriesFromDictionary;
	mutableDictionary->dictionary = dictionary;
	mutableDictionary->dictionaryWithCapacity = dictionaryWithCapacity;
	mutableDictionary->init = init;
	mutableDictionary->initWithCapacity = initWithCapacity;
	mutableDictionary->removeAllObjects = removeAllObjects;
	mutableDictionary->removeObjectForKey = removeObjectForKey;
	mutableDictionary->setObjectForKey = setObjectForKey;
	mutableDictionary->setObjectsForKeys = setObjectsForKeys;
}

Class _MutableDictionary = {
	.name = "MutableDictionary",
	.superclass = &_Dictionary,
	.instanceSize = sizeof(MutableDictionary),
	.interfaceOffset = offsetof(MutableDictionary, interface),
	.interfaceSize = sizeof(MutableDictionaryInterface),
	.initialize = initialize,
};

#undef _Class
