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

#include <Objectively/dictionary.h>

#define DICTIONARY_CHUNK_SIZE 64

#define HASH(key) ( $((Object *) key, hash) % self->capacity )

#pragma mark - Object instance methods

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Dictionary *this = (Dictionary *) self;

	for (size_t i = 0; i < this->capacity; i++) {
		if (this->elements[i] != NULL) {
			release(this->elements[i]);
		}
	}

	free(this->elements);

	super(Object, self, dealloc);
}

/**
 * @see Object::init(id, id, va_list *)
 */
static Object *init(id obj, id interface, va_list *args) {

	Dictionary *self = (Dictionary *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (DictionaryInterface *) interface;

		self->capacity = self->initialCapacity = arg(args, size_t, DICTIONARY_CHUNK_SIZE);
		self->elements = calloc(1, self->capacity * sizeof(id));
	}

	return (Object *) self;
}

#pragma mark - Dictionary instance methods

static BOOL allKeys_enumerator(const Dictionary *dict, id obj, id key, id data) {
	$((Array * ) data, addObject, key);
	return NO;
}

/**
 * @see Dictionary::allKeys(const Dictionary *)
 */
static Array *allKeys(const Dictionary *self) {

	Array *keys = new(Array);

	$(self, enumerateObjectsAndKeys, allKeys_enumerator, keys);

	return keys;
}

static BOOL allObjects_enumerator(const Dictionary *dict, id obj, id key, id data) {
	$((Array * ) data, addObject, obj);
	return NO;
}

/**
 * @see Dictionary::allObjects(const Dictionary *)
 */
static Array *allObjects(const Dictionary *self) {

	Array *objects = new(Array);

	$(self, enumerateObjectsAndKeys, allObjects_enumerator, objects);

	return objects;
}

/**
 * @see Dictionary::enumerateObjectsAndKeys(const Dictionary *, DictionaryEnumerator, id)
 */
static void enumerateObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator,
		id data) {

	assert(enumerator);

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array != NULL) {

			for (size_t j = 0; j < array->count; j += 2) {

				id key = $(array, objectAtIndex, j);
				id obj = $(array, objectAtIndex, j + 1);

				if (enumerator(self, obj, key, data)) {
					return;
				}
			}
		}
	}
}

/**
 * @see Dictionary::filterObjectsAndKeys(const Dictionary *, DictionaryEnumerator, id)
 */
static Dictionary *filterObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator,
		id data) {

	assert(enumerator);

	Dictionary *dictionary = new(Dictionary);

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array != NULL) {

			for (size_t j = 0; j < array->count; j += 2) {

				id key = $(array, objectAtIndex, j);
				id obj = $(array, objectAtIndex, j + 1);

				if (enumerator(self, obj, key, data)) {
					$(dictionary, setObjectForKey, obj, key);
				}
			}
		}
	}

	return dictionary;
}

/**
 * @see Dictionary::objectForKey(const Dictionary *, const id)
 */
static id objectForKey(const Dictionary *self, const id key) {

	assert(cast(Object, key));

	Array *array = self->elements[HASH(key)];
	if (array != NULL) {

		int index = $(array, indexOfObject, key);
		if (index > -1) {
			return $(array, objectAtIndex, index + 1);
		}
	}

	return NULL;
}

/**
 * @see Dictionary::removeAllObjects(Dictionary *)
 */
static void removeAllObjects(Dictionary *self) {

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array != NULL) {

			$(array, removeAllObjects);

			release(array);
			self->elements[i] = NULL;
		}
	}

	self->count = 0;
}

/**
 * @see Dictionary::removeObjectForKey(Dictionary *, const id)
 */
static void removeObjectForKey(Dictionary *self, const id key) {

	assert(cast(Object, key));

	Array *array = self->elements[HASH(key)];
	if (array != NULL) {

		int index = $(array, indexOfObject, key);
		if (index > -1) {

			$(array, removeObjectAtIndex, index);
			$(array, removeObjectAtIndex, index);

			if (array->count == 0) {
				release(array);
				self->elements[HASH(key)] = NULL;
			}

			self->count--;
		}
	}
}

/**
 * @see Dictionary::setObjectForKey(Dictionary *, const id, const id)
 */
static void setObjectForKey(Dictionary *self, const id obj, const id key) {

	assert(cast(Object, obj));
	assert(cast(Object, key));

	Array *array = self->elements[HASH(key)];
	if (array == NULL) {
		array = self->elements[HASH(key)] = new(Array);
	}

	int index = $(array, indexOfObject, key);
	if (index > -1) {
		$(array, setObjectAtIndex, obj, index + 1);
	} else {
		$(array, addObject, key);
		$(array, addObject, obj);

		self->count++;
	}
}

/**
 * @see Dictionary::setObjectsForKeys(Dictionary *, ...)
 */
static void setObjectsForKeys(Dictionary *self, ...) {

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

#pragma mark - Dictionary class methods

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	ObjectInterface *object = (ObjectInterface *) self->interface;

	object->dealloc = dealloc;
	object->init = init;

	DictionaryInterface *dictionary = (DictionaryInterface *) self->interface;

	dictionary->allKeys = allKeys;
	dictionary->allObjects = allObjects;
	dictionary->enumerateObjectsAndKeys = enumerateObjectsAndKeys;
	dictionary->filterObjectsAndKeys = filterObjectsAndKeys;
	dictionary->objectForKey = objectForKey;
	dictionary->removeAllObjects = removeAllObjects;
	dictionary->removeObjectForKey = removeObjectForKey;
	dictionary->setObjectForKey = setObjectForKey;
	dictionary->setObjectsForKeys = setObjectsForKeys;
}

Class __Dictionary = {
	.name = "Dictionary",
	.superclass = &__Object,
	.instanceSize = sizeof(Dictionary),
	.interfaceSize = sizeof(DictionaryInterface),
	.initialize = initialize, };
