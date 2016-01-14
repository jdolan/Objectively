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

#include <Objectively/MutableData.h>

#define _Class _MutableData

#pragma mark - ObjectInterface

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Data *this = (Data *) self;

	MutableData *that = $(alloc(MutableData), init);
	$(that, appendBytes, this->bytes, this->length);

	return (Object *) that;
}

#pragma mark - MutableDataInterface

/**
 * @see MutableData::appendBytes(MutableData *, const byte *, size_t)
 */
static void appendBytes(MutableData *self, const byte *bytes, size_t length) {

	const size_t oldLength = self->data.length;

	$(self, setLength, self->data.length + length);

	memcpy(self->data.bytes + oldLength, bytes, length);
}

/**
 * @see MutableData::appendData(MutableData *, const Data *)
 */
static void appendData(MutableData *self, const Data *data) {

	$(self, appendBytes, data->bytes, data->length);
}

/**
 * @see MutableData::data(void)
 */
static MutableData *data(void) {

	return $(alloc(MutableData), init);
}

/**
 * @see MutableData::dataWithCapacity(size_t)
 */
static MutableData *dataWithCapacity(size_t capacity) {

	return $(alloc(MutableData), initWithCapacity, capacity);
}

/**
 * @see MutableData::init(MutableData *)
 */
static MutableData *init(MutableData *self) {

	return $(self, initWithCapacity, 0);
}

/**
 * @see MutableData::initWithCapacity(MutableData *, size_t)
 */
static MutableData *initWithCapacity(MutableData *self, size_t capacity) {

	self = (MutableData *) super(Object, self, init);
	if (self) {

		self->capacity = capacity;
		if (self->capacity) {

			self->data.bytes = calloc(capacity, sizeof(byte));
			assert(self->data.bytes);
		}
	}

	return self;
}

/**
 * @see MutableData::initWithData(MutableData *, const Data *)
 */
static MutableData *initWithData(MutableData *self, const Data *data) {

	self = $(self, initWithCapacity, data->length);
	if (self) {
		$(self, appendData, data);
	}

	return self;
}

/**
 * @see MutableData::setLength(MutableData *, size_t)
 */
static void setLength(MutableData *self, size_t length) {

	const size_t newCapacity = (length / _pageSize + 1) * _pageSize;
	if (newCapacity > self->capacity) {

		if (self->data.bytes == NULL) {
			self->data.bytes = calloc(newCapacity, sizeof(byte));
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

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;

	MutableDataInterface *mutableData = (MutableDataInterface *) clazz->interface;

	mutableData->appendBytes = appendBytes;
	mutableData->appendData = appendData;
	mutableData->data = data;
	mutableData->dataWithCapacity = dataWithCapacity;
	mutableData->init = init;
	mutableData->initWithCapacity = initWithCapacity;
	mutableData->initWithData = initWithData;
	mutableData->setLength = setLength;
}

Class _MutableData = {
	.name = "MutableData",
	.superclass = &_Data,
	.instanceSize = sizeof(MutableData),
	.interfaceOffset = offsetof(MutableData, interface),
	.interfaceSize = sizeof(MutableDataInterface),
	.initialize = initialize,
};

#undef _Class
