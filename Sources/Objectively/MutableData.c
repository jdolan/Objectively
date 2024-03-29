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

#include "MutableData.h"

#define _Class _MutableData

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Data *this = (Data *) self;

	MutableData *that = $(alloc(MutableData), init);
	$(that, appendBytes, this->bytes, this->length);

	return (Object *) that;
}

#pragma mark - MutableData

/**
 * @fn void MutableData::appendBytes(MutableData *self, const uint8_t *bytes, size_t length)
 * @memberof MutableData
 */
static void appendBytes(MutableData *self, const uint8_t *bytes, size_t length) {

	const size_t oldLength = self->data.length;

	$(self, setLength, self->data.length + length);

	memcpy(self->data.bytes + oldLength, bytes, length);
}

/**
 * @fn void MutableData::appendData(MutableData *self, const Data *data)
 * @memberof MutableData
 */
static void appendData(MutableData *self, const Data *data) {

	$(self, appendBytes, data->bytes, data->length);
}

/**
 * @fn MutableData *MutableData::data(void)
 * @memberof MutableData
 */
static MutableData *data(void) {

	return $(alloc(MutableData), init);
}

/**
 * @fn MutableData *MutableData::dataWithCapacity(size_t capacity)
 * @memberof MutableData
 */
static MutableData *dataWithCapacity(size_t capacity) {

	return $(alloc(MutableData), initWithCapacity, capacity);
}

/**
 * @fn MutableData *MutableData::init(MutableData *self)
 * @memberof MutableData
 */
static MutableData *init(MutableData *self) {

	return $(self, initWithCapacity, 0);
}

/**
 * @fn MutableData *MutableData::initWithCapacity(MutableData *self, size_t capacity)
 * @memberof MutableData
 */
static MutableData *initWithCapacity(MutableData *self, size_t capacity) {

	self = (MutableData *) super(Object, self, init);
	if (self) {

		self->capacity = capacity;
		if (self->capacity) {

			self->data.bytes = calloc(capacity, sizeof(uint8_t));
			assert(self->data.bytes);
		}

		self->data.destroy = free;
	}

	return self;
}

/**
 * @fn MutableData *MutableData::initWithData(MutableData *self, const Data *data)
 * @memberof MutableData
 */
static MutableData *initWithData(MutableData *self, const Data *data) {

	self = $(self, initWithCapacity, data->length);
	if (self) {
		$(self, appendData, data);
	}

	return self;
}

/**
 * @fn void MutableData::setLength(MutableData *self, size_t length)
 * @memberof MutableData
 */
static void setLength(MutableData *self, size_t length) {

	const size_t newCapacity = (length / _pageSize + 1) * _pageSize;
	if (newCapacity > self->capacity) {

		if (self->data.bytes == NULL) {
			self->data.bytes = calloc(newCapacity, sizeof(uint8_t));
			assert(self->data.bytes);
		} else {
			self->data.bytes = realloc(self->data.bytes, newCapacity);
			assert(self->data.bytes);

			memset(self->data.bytes + self->data.length, 0, length - self->data.length);
		}

		self->capacity = newCapacity;
	}

	self->data.length = length;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;

	((MutableDataInterface *) clazz->interface)->appendBytes = appendBytes;
	((MutableDataInterface *) clazz->interface)->appendData = appendData;
	((MutableDataInterface *) clazz->interface)->data = data;
	((MutableDataInterface *) clazz->interface)->dataWithCapacity = dataWithCapacity;
	((MutableDataInterface *) clazz->interface)->init = init;
	((MutableDataInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
	((MutableDataInterface *) clazz->interface)->initWithData = initWithData;
	((MutableDataInterface *) clazz->interface)->setLength = setLength;
}

/**
 * @fn Class *MutableData::_MutableData(void)
 * @memberof MutableData
 */
Class *_MutableData(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "MutableData",
			.superclass = _Data(),
			.instanceSize = sizeof(MutableData),
			.interfaceOffset = offsetof(MutableData, interface),
			.interfaceSize = sizeof(MutableDataInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
