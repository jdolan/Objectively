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

#include <Objectively/Dictionary.h>
#include <Objectively/Hash.h>
#include <Objectively/String.h>

#define __Class __Dictionary

#define DICTIONARY_CHUNK_SIZE 64

#define HASH(key) ( $((Object *) key, hash) % self->capacity )

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const Dictionary *this = (Dictionary *) self;
	Dictionary *that = $(alloc(Dictionary), initWithCapacity, this->capacity);

	for (size_t i = 0; i < this->capacity; i++) {

		Array *array = this->elements[i];
		if (array != NULL) {
			that->elements[i] = $((Object *) array, copy);
		}
	}

	return (Object *) that;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Dictionary *this = (Dictionary *) self;

	$(this, removeAllObjects);
	free(this->elements);

	super(Object, self, dealloc);
}

/**
 * @brief A DictionaryEnumerator for description.
 */
static BOOL description_enumerator(const Dictionary *dict, id obj, id key, id data) {

	String *desc = (String *) data;

	String *objDesc = $((Object *) obj, description);
	String *keyDesc = $((Object *) key, description);

	$(desc, appendFormat, "%s: %s, ", keyDesc->chars, objDesc->chars);

	release(objDesc);
	release(keyDesc);

	return NO;
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	const Dictionary *this = (Dictionary *) self;

	String *desc = $(alloc(String), initWithCharacters, "{");

	$(this, enumerateObjectsAndKeys, description_enumerator, desc);

	$(desc, appendFormat, "}");

	return desc;
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	const Dictionary *this = (Dictionary *) self;

	int hash = HASH_SEED;

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

		const Dictionary *this = (Dictionary *) self;
		const Dictionary *that = (Dictionary *) other;

		if (this->count == that->count) {

			Array *keys = $(this, allKeys);

			for (size_t i = 0; i < keys->count; i++) {
				const id key = $(keys, objectAtIndex, i);

				const Object *thisObject = $(this, objectForKey, key);
				const Object *thatObject = $(that, objectForKey, key);

				if ($(thisObject, isEqual, thatObject) == NO) {
					release(keys);
					return NO;
				}
			}

			release(keys);
			return YES;
		}
	}

	return NO;
}

#pragma mark - DictionaryInterface

/**
 * @brief DictionaryEnumerator for allKeys.
 */
static BOOL allKeys_enumerator(const Dictionary *dict, id obj, id key, id data) {
	$((Array *) data, addObject, key); return NO;
}

/**
 * @see DictionaryInterface::allKeys(const Dictionary *)
 */
static Array *allKeys(const Dictionary *self) {

	Array *keys = $(alloc(Array), initWithCapacity, self->count);

	$(self, enumerateObjectsAndKeys, allKeys_enumerator, keys);

	return keys;
}

/**
 * @brief DictionaryEnumerator for allObjects.
 */
static BOOL allObjects_enumerator(const Dictionary *dict, id obj, id key, id data) {
	$((Array *) data, addObject, obj); return NO;
}

/**
 * @see DictionaryInterface::allObjects(const Dictionary *)
 */
static Array *allObjects(const Dictionary *self) {

	Array *objects = $(alloc(Array), initWithCapacity, self->count);

	$(self, enumerateObjectsAndKeys, allObjects_enumerator, objects);

	return objects;
}

/**
 * @see DictionaryInterface::enumerateObjectsAndKeys(const Dictionary *, DictionaryEnumerator, id)
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
 * @see DictionaryInterface::filterObjectsAndKeys(const Dictionary *, DictionaryEnumerator, id)
 */
static Dictionary *filterObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator,
		id data) {

	assert(enumerator);

	Dictionary *dictionary = alloc(Dictionary);

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
 * @see DictionaryInterface::init(Dictionary *)
 */
static Dictionary *init(Dictionary *self) {
	return $(self, initWithCapacity, DICTIONARY_CHUNK_SIZE);
}

/**
 * @see DicionaryInterface::initWithCapacity(Dictionary *, size_t)
 */
static Dictionary *initWithCapacity(Dictionary *self, size_t capacity) {

	self = (Dictionary *) super(Object, self, init);
	if (self) {
		self->capacity = self->initialCapacity = capacity;
		self->elements = calloc(1, self->capacity * sizeof(id));
		assert(self->elements);
	}

	return self;
}

/**
 * @see DictionaryInterface::objectForKey(const Dictionary *, const id)
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
 * @see DictionaryInterface::removeAllObjects(Dictionary *)
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
 * @see DictionaryInterface::removeObjectForKey(Dictionary *, const id)
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
 * @see DictionaryInterface::setObjectForKey(Dictionary *, const id, const id)
 */
static void setObjectForKey(Dictionary *self, const id obj, const id key) {

	assert(cast(Object, obj));
	assert(cast(Object, key));

	Array *array = self->elements[HASH(key)];
	if (array == NULL) {
		array = self->elements[HASH(key)] = alloc(Array);
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
 * @see DictionaryInterface::setObjectsForKeys(Dictionary *, ...)
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

	DictionaryInterface *dictionary = (DictionaryInterface *) clazz->interface;

	dictionary->allKeys = allKeys;
	dictionary->allObjects = allObjects;
	dictionary->enumerateObjectsAndKeys = enumerateObjectsAndKeys;
	dictionary->filterObjectsAndKeys = filterObjectsAndKeys;
	dictionary->init = init;
	dictionary->initWithCapacity = initWithCapacity;
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
	.interfaceOffset = offsetof(Dictionary, interface),
	.interfaceSize = sizeof(DictionaryInterface),
	.initialize = initialize,
};

#undef __Class

