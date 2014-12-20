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

#include "config.h"

#include <assert.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <Objectively/MutableString.h>

#define _Class _MutableString

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	MutableString *this = (MutableString *) self;

	return (Object *) $(alloc(MutableString), initWithString, (String *) self);
}

#pragma mark - MutableStringInterface

/**
 * @see MutableStringInterface::string(void)
 */
static MutableString *string(void) {

	return $(alloc(MutableString), init);
}

/**
 * @see MutableStringInterface::stringWithCapacity(size_t)
 */
static MutableString *stringWithCapacity(size_t capacity) {

	return $(alloc(MutableString), initWithCapacity, capacity);
}

/**
 * @see MutableStringInterface::appendFormat(MutableString *, const char *, ...)
 */
static void appendFormat(MutableString *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char *str;
	vasprintf(&str, fmt, args);

	va_end(args);

	if (str) {
		String *string = $(alloc(String), initWithMemory, str);

		string->chars = str;
		string->length = strlen(string->chars);

		$(self, appendString, string);

		release(string);
	}
}

static size_t pageSize;

/**
 * @see MutableStringInterface::appendString(MutableString *, const String *)
 */
static void appendString(MutableString *self, const String *string) {

	if (string->length) {

		const size_t newSize = self->string.length + string->length + 1;
		const size_t newCapacity = (newSize / pageSize + 1) * pageSize;

		if (newCapacity > self->capacity) {

			if (self->string.length) {
				self->string.chars = realloc(self->string.chars, newCapacity);
			} else {
				self->string.chars = malloc(newCapacity);
			}

			assert(self->string.chars);
			self->capacity = newCapacity;
		}

		memcpy(self->string.chars + self->string.length, string->chars, string->length);
		self->string.chars[newSize - 1] = '\0';

		self->string.length += string->length;
	}
}

/**
 * @see MutableStringInterface::deleteCharactersInRange(MutableString *, const RANGE)
 */
static void deleteCharactersInRange(MutableString *self, const RANGE range) {

	assert(range.location >= 0);
	assert(range.length <= self->string.length);

	id ptr = self->string.chars + range.location;
	const size_t len = self->string.length - range.location - range.length;

	memmove(ptr, ptr + range.length, len);
}

/**
 * @see MutableStringInterface::init(MutableString *)
 */
static MutableString *init(MutableString *self) {

	return $(self, initWithString, NULL);
}

/**
 * @see MutableStringInterface::initWithString(MutableString *, const String *)
 */
static MutableString *initWithString(MutableString *self, const String *string) {

	self = (MutableString *) super(String, self, initWithMemory, NULL);
	if (self) {

		if (string) {
			self->string.locale = string->locale;
			$(self, appendString, string);
		}
	}

	return self;
}

/**
 * @see MutableStringInterface::replaceCharactersInRange(MutableString *, const RANGE, const String *)
 */
static void replaceCharactersInRange(MutableString *self, const RANGE range, const String *string) {

	assert(range.location >= 0);
	assert(range.length <= self->string.length);

	assert(string);

	const char *c = self->string.chars + range.location + range.length;
	String *remainder = $(alloc(String), initWithCharacters, c);

	self->string.length = range.location;
	self->string.chars[self->string.length] = '\0';

	$(self, appendString, string);
	$(self, appendString, remainder);

	release(remainder);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;

	MutableStringInterface *mutableString = (MutableStringInterface *) clazz->interface;

	mutableString->string = string;
	mutableString->stringWithCapacity = stringWithCapacity;
	mutableString->appendFormat = appendFormat;
	mutableString->appendString = appendString;
	mutableString->deleteCharactersInRange = deleteCharactersInRange;
	mutableString->init = init;
	mutableString->initWithString = initWithString;
	mutableString->replaceCharactersInRange = replaceCharactersInRange;

	pageSize = sysconf(_SC_PAGESIZE);
}

Class _MutableString = {
	.name = "MutableString",
	.superclass = &_String,
	.instanceSize = sizeof(MutableString),
	.interfaceOffset = offsetof(MutableString, interface),
	.interfaceSize = sizeof(MutableStringInterface),
	.initialize = initialize,
};

#undef _Class
