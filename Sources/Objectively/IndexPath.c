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
#include "IndexPath.h"
#include "MutableString.h"

#define _Class _IndexPath

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	IndexPath *this = (IndexPath *) self;
	IndexPath *that = $(alloc(IndexPath), initWithIndexes, this->indexes, this->length);

	return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	IndexPath *this = (IndexPath *) self;

	free(this->indexes);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	const IndexPath *this = (IndexPath *) self;
	MutableString *desc = mstr("[");

	for (size_t i = 0; i < this->length; i++) {
		$(desc, appendFormat, "%d", this->indexes[i]);
		if (i < this->length - 1) {
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

	const IndexPath *this = (IndexPath *) self;

	for (size_t i = 0; i < this->length; i++) {
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

	if (other && $(other, isKindOfClass, _IndexPath())) {

		const IndexPath *this = (IndexPath *) self;
		const IndexPath *that = (IndexPath *) other;

		if (this->length == that->length) {
			return memcmp(this->indexes, that->indexes, this->length * sizeof(size_t)) == 0;
		}
	}

	return false;
}

#pragma mark - IndexPath

/**
 * @fn size_t IndexPath::indexAtPosition(const IndexPath *self, size_t position)
 * @memberof IndexPath
 */
static size_t indexAtPosition(const IndexPath *self, size_t position) {

	assert(position < self->length);

	return self->indexes[position];
}

/**
 * @fn IndexPath *IndexPath::initWithIndex(IndexPath *self, size_t index)
 * @memberof IndexPath
 */
static IndexPath *initWithIndex(IndexPath *self, size_t index) {
	return $(self, initWithIndexes, &index, 1);
}

/**
 * @fn IndexPath *IndexPath::initWithIndexes(IndexPath *self, size_t *indexes, size_t length)
 * @memberof IndexPath
 */
static IndexPath *initWithIndexes(IndexPath *self, size_t *indexes, size_t length) {

	self = (IndexPath *) super(Object, self, init);
	if (self) {

		self->length = length;
		assert(self->length);

		self->indexes = calloc(sizeof(size_t), length);
		assert(self->indexes);

		memcpy(self->indexes, indexes, sizeof(size_t) * length);
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

	((IndexPathInterface *) clazz->interface)->indexAtPosition = indexAtPosition;
	((IndexPathInterface *) clazz->interface)->initWithIndex = initWithIndex;
	((IndexPathInterface *) clazz->interface)->initWithIndexes = initWithIndexes;
}

/**
 * @fn Class *IndexPath::_IndexPath(void)
 * @memberof IndexPath
 */
Class *_IndexPath(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "IndexPath",
			.superclass = _Object(),
			.instanceSize = sizeof(IndexPath),
			.interfaceOffset = offsetof(IndexPath, interface),
			.interfaceSize = sizeof(IndexPathInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

