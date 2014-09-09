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

#include <objectively/dictionary.h>

#define DICTIONARY_CHUNK_SIZE 64

#define HASH(key) ( $((Object *) key, hash) % self->capacity )

#pragma mark - Object instance methods

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

static Array *allKeys(const Dictionary *self) {

	Array *keys = new(Array);

	BOOL enumerator(const Dictionary *dict, id obj, id key, id data) {
		$(keys, addObject, key); return NO;
	}

	$(self, enumerateObjectsAndKeys, enumerator, NULL);

	return keys;
}

static Array *allObjects(const Dictionary *self) {

	Array *objects = new(Array);

	BOOL enumerator(const Dictionary *dict, id obj, id key, id data) {
		$(objects, addObject, obj); return NO;
	}

	$(self, enumerateObjectsAndKeys, enumerator, NULL);

	return objects;
}

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

static void removeAllObjects(Dictionary *self) {

	for (size_t i = 0; i < self->capacity; i++) {

		Array *array = self->elements[i];
		if (array != NULL) {

			$(array, removeAllObjects);

			release(array);
			self->elements[i] = NULL;
		}
	}
}

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

#pragma mark - Dictionary class methods

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
}

Class __Dictionary = {
	.name = "Dictionary",
	.superclass = &__Object,
	.instanceSize = sizeof(Dictionary),
	.interfaceSize = sizeof(DictionaryInterface),
	.initialize = initialize, };
