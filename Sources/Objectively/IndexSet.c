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
#include <stdlib.h>
#include <string.h>

#include "Hash.h"
#include "IndexSet.h"
#include "MutableString.h"

/**
 * @brief qsort comparator for indexes.
 */
static int compare(const void *a, const void *b) {

	const size_t sa = *(size_t *) a;
	const size_t sb = *(size_t *) b;

	return sa < sb ? -1 : sa > sb ? 1 : 0;
}

/**
 * @brief Sorts and compacts the given array to contain only unique values.
 */
static size_t compact(size_t *indexes, size_t count) {

	size_t size = 0;

	if (count) {
		qsort(indexes, count, sizeof(size_t), compare);

		for (size_t i = 1; i < count; i++) {
			if (indexes[i] != indexes[size]) {
				indexes[++size] = indexes[i];
			}
		}

		size++;
	}

	return size;
}

#define _Class _IndexSet

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	IndexSet *this = (IndexSet *) self;
	IndexSet *that = $(alloc(IndexSet), initWithIndexes, this->indexes, this->count);

	return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	IndexSet *this = (IndexSet *) self;

	free(this->indexes);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const IndexSet *this = (IndexSet *) self;
	MutableString *desc = mstr("[");

	for (size_t i = 0; i < this->count; i++) {
		$(desc, appendFormat, "%d", this->indexes[i]);
		if (i < this->count - 1) {
			$(desc, appendCharacters, ", ");
		}
	}

	$(desc, appendCharacters, "]");
	return (String *) desc;
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	int hash = HASH_SEED;

	const IndexSet *this = (IndexSet *) self;

	for (size_t i = 0; i < this->count; i++) {
		hash = HashForInteger(hash, this->indexes[i]);
	}

	return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _IndexSet())) {

		const IndexSet *this = (IndexSet *) self;
		const IndexSet *that = (IndexSet *) other;

		if (this->count == that->count) {
			return memcmp(this->indexes, that->indexes, this->count * sizeof(size_t)) == 0;
		}
	}

	return false;
}

#pragma mark - IndexSet

/**
 * @fn bool IndexSet::containsIndex(const IndexSet *self, size_t index)
 * @memberof IndexSet
 */
static bool containsIndex(const IndexSet *self, size_t index) {

	for (size_t i = 0; i < self->count; i++) {
		if (self->indexes[i] == index) {
			return true;
		}
	}

	return false;
}

/**
 * @fn IndexSet *IndexSet::initWithIndex(IndexSet *self, size_t index)
 * @memberof IndexSet
 */
static IndexSet *initWithIndex(IndexSet *self, size_t index) {
	return $(self, initWithIndexes, &index, 1);
}

/**
 * @fn IndexSet *IndexSet::initWithIndexes(IndexSet *self, size_t *indexes, size_t count)
 * @memberof IndexSet
 */
static IndexSet *initWithIndexes(IndexSet *self, size_t *indexes, size_t count) {

	self = (IndexSet *) super(Object, self, init);
	if (self) {

		self->count = compact(indexes, count);
		if (self->count) {

			self->indexes = calloc(sizeof(size_t), self->count);
			assert(self->indexes);

			memcpy(self->indexes, indexes, sizeof(size_t) * self->count);
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->description = description;
	((ObjectInterface *) clazz->interface)->hash = hash;
	((ObjectInterface *) clazz->interface)->isEqual = isEqual;

	((IndexSetInterface *) clazz->interface)->containsIndex = containsIndex;
	((IndexSetInterface *) clazz->interface)->initWithIndex = initWithIndex;
	((IndexSetInterface *) clazz->interface)->initWithIndexes = initWithIndexes;
}

/**
 * @fn Class *IndexSet::_IndexSet(void)
 * @memberof IndexSet
 */
Class *_IndexSet(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "IndexSet",
			.superclass = _Object(),
			.instanceSize = sizeof(IndexSet),
			.interfaceOffset = offsetof(IndexSet, interface),
			.interfaceSize = sizeof(IndexSetInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

