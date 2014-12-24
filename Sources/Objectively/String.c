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

#include "config.h"

#include <assert.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/String.h>

#define _Class _String

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	String *this = (String *) self;
	String *that = $(alloc(String), initWithCharacters, this->chars);

	return (Object *) that;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	String *this = (String *) self;

	if (this->chars) {
		free(this->chars);
	}

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::description(const Object *)
 */
static String *description(const Object *self) {

	return (String *) $(self, copy);
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	String *this = (String *) self;

	const RANGE range = { 0, this->length };

	return HashForCharacters(HASH_SEED, this->chars, range);
}

/**
 * @see ObjectInterface::isEqual(const Object *, const Object *)
 */
static BOOL isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return YES;
	}

	if (other && self->clazz == other->clazz) {

		const String *this = (String *) self;
		const String *that = (String *) other;

		if (this->locale == that->locale) {

			const RANGE range = { 0, this->length };
			return $(this, compareTo, that, range) == SAME;
		}
	}

	return NO;
}

#pragma mark - StringInterface

/**
 * @see StringInterface::compareTo(const String *, const String *, const RANGE)
 */
static ORDER compareTo(const String *self, const String *other, const RANGE range) {

	assert(range.location + range.length <= self->length);

	if (other) {
		const int i = strncmp(self->chars + range.location, other->chars, range.length);
		if (i == 0) {
			return SAME;
		}
		if (i > 0) {
			return DESCENDING;
		}
	}

	return ASCENDING;
}

/**
 * @see StringInterface::componentsSeparatedByCharacters(const String *, const char *)
 */
static Array *componentsSeparatedByCharacters(const String *self, const char *chars) {

	assert(chars);

	MutableArray *components = $(alloc(MutableArray), init);

	RANGE search = { 0, self->length };
	RANGE result = $(self, rangeOfCharacters, chars, search);

	while (result.length) {
		search.length = result.location - search.location;

		String *component = $(self, substring, search);
		$(components, addObject, component);
		release(component);

		search.location = result.location + result.length;
		search.length = self->length - search.location;

		result = $(self, rangeOfCharacters, chars, search);
	}

	String *component = $(self, substring, search);
	$(components, addObject, component);
	release(component);

	return (Array *) components;
}

/**
 * @see StringInterface::componentsSeparatedByString(const String *, const String *)
 */
static Array *componentsSeparatedByString(const String *self, const String *string) {

	assert(string);

	return $(self, componentsSeparatedByCharacters, string->chars);
}

/**
 * @see StringInterface::hasPrefix(const String *, const String *)
 */
static BOOL hasPrefix(const String *self, const String *prefix) {

	if (prefix->length > self->length) {
		return NO;
	}

	RANGE range = { 0, prefix->length };
	return $(self, compareTo, prefix, range) == SAME;
}

/**
 * @see StringInterface::hasSuffix(const String *, const String *)
 */
static BOOL hasSuffix(const String *self, const String *suffix) {

	if (suffix->length > self->length) {
		return NO;
	}

	RANGE range = { self->length - suffix->length, suffix->length };
	return $(self, compareTo, suffix, range) == SAME;
}

/**
 * @see StringInterface::initWithBytes(String *, const byte *, size_t)
 */
static String *initWithBytes(String *self, const byte *bytes, size_t length) {

	id mem = NULL;
	if (bytes) {

		mem = malloc(length + 1);
		assert(mem);

		memcpy(mem, bytes, length);
		*(char *) (mem + length) = '\0';
	}

	return $(self, initWithMemory, mem, length);
}

/**
 * @see StringInterface::initWithCharacters(String *, const char *)
 */
static String *initWithCharacters(String *self, const char *chars) {

	if (chars) {
		char *str = strdup(chars);
		assert(str);

		return $(self, initWithMemory, str, strlen(str));
	}

	return $(self, initWithMemory, NULL, 0);
}

/**
 * @see StringInterface::initWithContentsOfFile(String *, const char *)
 */
static String *initWithContentsOfFile(String *self, const char *path) {

	assert(path);

	FILE *file = fopen(path, "r");
	if (file) {
		id mem = NULL;

		int err = fseek(file, 0, SEEK_END);
		assert(err == 0);

		const size_t length = ftell(file);

		if (length) {
			mem = malloc(length + 1);
			assert(mem);

			int err = fseek(file, 0, SEEK_SET);
			assert(err == 0);

			const size_t read = fread(mem, length, 1, file);
			assert(read == 1);

			*(char *) (mem + length) = '\0';
		}

		fclose(file);

		return $(self, initWithMemory, mem, length);
	}

	return NULL;
}

/**
 * @see StringInterface::initWithData(String *, const Data *)
 */
static String *initWithData(String *self, const Data *data) {

	assert(data);

	return $(self, initWithBytes, data->bytes, data->length);
}

/**
 * @see StringInterface::initWithFormat(id, const char *, ...)
 */
static String *initWithFormat(String *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	id mem;
	const size_t length = vaStringPrintf(&mem, fmt, args);

	va_end(args);

	return $(self, initWithMemory, mem, length);
}

/**
 * @see StringInterface::initWithMemory(String *, id, size_t)
 */
static String *initWithMemory(String *self, id mem, size_t length) {

	self = (String *) super(Object, self, init);
	if (self) {
		self->locale = LC_GLOBAL_LOCALE;

		if (mem) {
			self->chars = (char *) mem;
			self->length = length;
		}
	}

	return self;
}

/**
 * @see StringInterface::lowercaseString(const String *)
 */
static String *lowercaseString(const String *self) {

	String *string = (String *) $((Object *) self, copy);

	for (size_t i = 0; i < string->length; i++) {
		string->chars[i] = tolower_l(string->chars[i], self->locale);
	}

	return string;
}

/**
 * @see StringInterface::rangeOfCharacters(const String *, const char *, const RANGE)
 */
static RANGE rangeOfCharacters(const String *self, const char *chars, const RANGE range) {

	assert(chars);
	assert(range.location > -1);
	assert(range.length > -1);
	assert(range.location + range.length <= self->length);

	RANGE match = { -1, 0 };
	const size_t len = strlen(chars);

	const char *str = self->chars + range.location;
	for (size_t i = 0; i < range.length; i++, str++) {
		if (strncmp(str, chars, len) == 0) {
			match.location = range.location + i;
			match.length = len;
			break;
		}
	}

	return match;
}

/**
 * @see StringInterface::rangeOfString(const String *, const String *, const RANGE)
 */
static RANGE rangeOfString(const String *self, const String *string, const RANGE range) {

	assert(string);

	return $(self, rangeOfCharacters, string->chars, range);
}

/**
 * @see StringInterface::stringWithBytes(const byte *, size_t)
 */
static String *stringWithBytes(const byte *bytes, size_t length) {

	return $(alloc(String), initWithBytes, bytes, length);
}

/**
 * @see StringInterface::stringWithCharacters(const char *)
 */
static String *stringWithCharacters(const char *chars) {

	return $(alloc(String), initWithCharacters, chars);
}

/**
 * @see StringInterface::stringWithContentsOfFile(const char *)
 */
static String *stringWithContentsOfFile(const char *path) {

	return $(alloc(String), initWithContentsOfFile, path);
}

/**
 * @see StringInterface::stringWithData(const Data *data)
 */
static String *stringWithData(const Data *data) {

	return $(alloc(String), initWithData, data);
}

/**
 * @see StringInterface::stringWithFormat(const char *fmt, ...)
 */
static String *stringWithFormat(const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	id mem;
	const size_t length = vaStringPrintf(&mem, fmt, args);

	va_end(args);

	return $(alloc(String), initWithMemory, mem, length);
}

/**
 * @see StringInterface::substring(const String *, RANGE)
 */
static String *substring(const String *self, const RANGE range) {

	assert(range.location + range.length <= self->length);

	id mem = calloc(range.length + 1, sizeof(char));
	memcpy(mem, self->chars + range.location, range.length);

	return $(alloc(String), initWithMemory, mem, range.length);
}

/**
 * @see StringInterface::uppercaseString(const String *)
 */
static String *uppercaseString(const String *self) {

	String *string = (String *) $((Object *) self, copy);

	for (size_t i = 0; i < string->length; i++) {
		string->chars[i] = toupper_l(string->chars[i], self->locale);
	}

	return string;
}

/**
 * @see StringInterface::writeToFile(const String *, const char *)
 */
static BOOL writeToFile(const String *self, const char *path) {

	BOOL ret = NO;

	FILE *file = fopen(path, "w");
	if (file) {

		const size_t write = fwrite(self->chars, self->length, 1, file);
		if (write == 1) {
			ret = YES;
		}

		fclose(file);
	}

	return ret;
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

	StringInterface *string = (StringInterface *) clazz->interface;

	string->compareTo = compareTo;
	string->componentsSeparatedByCharacters = componentsSeparatedByCharacters;
	string->componentsSeparatedByString = componentsSeparatedByString;
	string->hasPrefix = hasPrefix;
	string->hasSuffix = hasSuffix;
	string->initWithBytes = initWithBytes;
	string->initWithCharacters = initWithCharacters;
	string->initWithContentsOfFile = initWithContentsOfFile;
	string->initWithData = initWithData;
	string->initWithFormat = initWithFormat;
	string->initWithMemory = initWithMemory;
	string->lowercaseString = lowercaseString;
	string->rangeOfCharacters = rangeOfCharacters;
	string->rangeOfString = rangeOfString;
	string->stringWithBytes = stringWithBytes;
	string->stringWithCharacters = stringWithCharacters;
	string->stringWithContentsOfFile = stringWithContentsOfFile;
	string->stringWithData = stringWithData;
	string->stringWithFormat = stringWithFormat;
	string->substring = substring;
	string->uppercaseString = uppercaseString;
	string->writeToFile = writeToFile;
}

Class _String = {
	.name = "String",
	.superclass = &_Object,
	.instanceSize = sizeof(String),
	.interfaceOffset = offsetof(String, interface),
	.interfaceSize = sizeof(StringInterface),
	.initialize = initialize,
};

#undef _Class

/**
 * @brief A helper for initializing Strings from formatted C strings.
 */
size_t vaStringPrintf(id *mem, const char *fmt, va_list args) {

	*mem = NULL;
	size_t length = 0;

	if (fmt) {
		int err = vasprintf((char **) mem, fmt, args);
		if (err >= 0) {
			length = err;
		}
	}

	return length;
}
