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
#include <string.h>

#include <Objectively/Data.h>
#include <Objectively/Hash.h>

#define __Class __Data

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	Data *this = (Data *) self;

	return (Object *) $(alloc(Data), initWithBytes, this->bytes, this->length);
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Data *this = (Data *) self;

	if (this->bytes) {
		free(this->bytes);
	}

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	Data *this = (Data *) self;

	int hash = HASH_SEED;
	hash = HashForInteger(hash, this->length);

	const RANGE range = { 0, this->length };
	hash = HashForCharacters(hash, (char *) this->bytes, range);

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

		const Data *this = (Data *) self;
		const Data *that = (Data *) other;

		if (this->length == that->length) {
			return memcmp(this->bytes, that->bytes, this->length) == 0;
		}
	}

	return NO;
}

#pragma mark - DataInterface

/**
 * @see DataInterface::appendBytes(Data *, const byte *, size_t)
 */
static void appendBytes(Data *self, const byte *bytes, size_t length) {

	if (self->bytes == NULL) {
		self->bytes = malloc(length);
	} else {
		self->bytes = realloc(self->bytes, self->length + length);
	}

	assert(self->bytes);

	id ptr = self->bytes + self->length;
	memcpy(ptr, bytes, length);

	self->length += length;
}

/**
 * @see DataInterface::initWithBytes(Data *, const byte *, const size_t)
 */
static Data *initWithBytes(Data *self, const byte *bytes, size_t length) {

	id mem = malloc(length);
	assert(mem);

	memcpy(mem, bytes, length);

	return $(self, initWithMemory, mem, length);
}

/**
 * @see DataInterface::initWithContentsOfFile(Data *, const char *)
 */
static Data *initWithContentsOfFile(Data *self, const char *path) {

	assert(path);

	FILE *file = fopen(path, "r");
	if (file) {
		id mem = NULL;

		fseek(file, 0, SEEK_END);
		const size_t length = ftell(file);

		if (length) {
			mem = malloc(length);
			assert(mem);

			fseek(file, 0, SEEK_SET);

			const size_t read = fread(mem, 1, length, file);
			assert(read == length);
		}

		fclose(file);

		return $(self, initWithMemory, mem, length);
	}

	return NULL;
}

/**
 * @see DataInterface::initWithMemory(Data *, id, const size_t)
 */
static Data *initWithMemory(Data *self, id mem, size_t length) {

	self = (Data *) super(Object, self, init);
	if (self) {
		self->bytes = mem;
		self->length = length;
	}

	return self;
}

/**
 * @see DataInterface::writeToFile(const Data *, const char *)
 */
static BOOL writeToFile(const Data *self, const char *path) {

	FILE *file = fopen(path, "w");
	if (file) {

		const size_t write = fwrite(self->bytes, 1, self->length, file);
		fclose(file);

		if (write == self->length) {
			return YES;
		}
	}

	return NO;
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

	data->appendBytes = appendBytes;
	data->initWithBytes = initWithBytes;
	data->initWithContentsOfFile = initWithContentsOfFile;
	data->initWithMemory = initWithMemory;
	data->writeToFile = writeToFile;
}

Class __Data = {
	.name = "Data",
	.superclass = &__Object,
	.instanceSize = sizeof(Data),
	.interfaceOffset = offsetof(Data, interface),
	.interfaceSize = sizeof(DataInterface),
	.initialize = initialize,
};

#undef __Class
