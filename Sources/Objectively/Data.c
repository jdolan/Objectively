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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Objectively/Data.h>
#include <Objectively/Hash.h>
#include <Objectively/MutableData.h>

#define _Class _Data

#define DATA_BLOCK_SIZE 4096

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Data *this = (Data *) self;

	return (Object *) $alloc(Data, initWithBytes, this->bytes, this->length);
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Data *this = (Data *) self;

	if (this->bytes) {
		free(this->bytes);
	}

	super(Object, self, dealloc);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	Data *this = (Data *) self;

	int hash = HASH_SEED;
	hash = HashForInteger(hash, this->length);

	const Range range = { 0, this->length };
	hash = HashForBytes(hash, this->bytes, range);

	return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, &_Data)) {

		const Data *this = (Data *) self;
		const Data *that = (Data *) other;

		if (this->length == that->length) {
			return memcmp(this->bytes, that->bytes, this->length) == 0;
		}
	}

	return false;
}

#pragma mark - Data

/**
 * @fn Data *Data::dataWithBytes(const uint8_t *bytes, size_t length)
 *
 * @memberof Data
 */
static Data *dataWithBytes(const uint8_t *bytes, size_t length) {

	return $alloc(Data, initWithBytes, bytes, length);
}

/**
 * @fn Data *Data::dataWithContentsOfFile(const char *path)
 *
 * @memberof Data
 */
static Data *dataWithContentsOfFile(const char *path) {

	return $alloc(Data, initWithContentsOfFile, path);
}

/**
 * @fn Data *Data::dataWithMemory(const ident mem, size_t length)
 *
 * @memberof Data
 */
static Data *dataWithMemory(const ident mem, size_t length) {

	return $alloc(Data, initWithMemory, mem, length);
}

/**
 * @fn Data *Data::initWithBytes(Data *self, const uint8_t *bytes, size_t length)
 *
 * @memberof Data
 */
static Data *initWithBytes(Data *self, const uint8_t *bytes, size_t length) {

	ident mem = malloc(length);
	assert(mem);

	memcpy(mem, bytes, length);

	return $(self, initWithMemory, mem, length);
}

/**
 * @fn Data *Data::initWithContentsOfFile(Data *self, const char *path)
 *
 * @memberof Data
 */
static Data *initWithContentsOfFile(Data *self, const char *path) {

	assert(path);

	FILE *file = fopen(path, "rb");
	if (file) {
		ident mem = NULL;

		int err = fseek(file, 0, SEEK_END);
		assert(err == 0);

		const size_t length = ftell(file);
		if (length) {
			
			mem = malloc(length);
			assert(mem);

			int err = fseek(file, 0, SEEK_SET);
			assert(err == 0);

			const size_t read = fread(mem, length, 1, file);
			assert(read == 1);
		}

		fclose(file);
		return $(self, initWithMemory, mem, length);
	}

	release(self);
	return NULL;
}

/**
 * @fn Data *Data::initWithMemory(Data *self, const ident mem, size_t length)
 *
 * @memberof Data
 */
static Data *initWithMemory(Data *self, const ident mem, size_t length) {

	self = (Data *) super(Object, self, init);
	if (self) {
		self->bytes = mem;
		self->length = length;
	}

	return self;
}

/**
 * @fn MutableData *Data::mutableCopy(const Data *self)
 *
 * @memberof Data
 */
static MutableData *mutableCopy(const Data *self) {

	return $alloc(MutableData, initWithData, self);
}

/**
 * @fn _Bool Data::writeToFile(const Data *self, const char *path)
 *
 * @memberof Data
 */
static _Bool writeToFile(const Data *self, const char *path) {

	assert(path);

	FILE *file = fopen(path, "w");
	if (file) {

		size_t count = 1;

		if (self->length) {
			count = fwrite(self->bytes, self->length, 1, file);
		}

		fclose(file);

		if (count == 1) {
			return true;
		}
	}

	return false;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->hash = hash;
	object->isEqual = isEqual;

	DataInterface *data = (DataInterface *) clazz->interface;

	data->dataWithBytes = dataWithBytes;
	data->dataWithContentsOfFile = dataWithContentsOfFile;
	data->dataWithMemory = dataWithMemory;
	data->initWithBytes = initWithBytes;
	data->initWithContentsOfFile = initWithContentsOfFile;
	data->initWithMemory = initWithMemory;
	data->mutableCopy = mutableCopy;
	data->writeToFile = writeToFile;
}

Class _Data = {
	.name = "Data",
	.superclass = &_Object,
	.instanceSize = sizeof(Data),
	.interfaceOffset = offsetof(Data, interface),
	.interfaceSize = sizeof(DataInterface),
	.initialize = initialize,
};

#undef _Class
