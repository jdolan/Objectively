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

#include <Objectively/MutableString.h>

#define _Class _MutableString

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	String *this = (String *) self;

	return (Object *) $(alloc(MutableString), initWithString, this);
}

#pragma mark - MutableString

/**
 * @fn void MutableString::appendCharacters(MutableString *self, const char *chars)
 *
 * @memberof MutableString
 */
static void appendCharacters(MutableString *self, const char *chars) {

	if (chars) {

		const size_t len = strlen(chars);
		if (len) {

			const size_t newSize = self->string.length + strlen(chars) + 1;
			const size_t newCapacity = (newSize / _pageSize + 1) * _pageSize;

			if (newCapacity > self->capacity) {

				if (self->string.length) {
					self->string.chars = realloc(self->string.chars, newCapacity);
				} else {
					self->string.chars = malloc(newCapacity);
				}

				assert(self->string.chars);
				self->capacity = newCapacity;
			}

			ident ptr = self->string.chars + self->string.length;
			memmove(ptr, chars, len);

			self->string.chars[newSize - 1] = '\0';
			self->string.length += len;
		}
	}
}

/**
 * @fn void MutableString::appendFormat(MutableString *self, const char *fmt, ...)
 *
 * @memberof MutableString
 */
static void appendFormat(MutableString *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char *chars;

	const int len = vasprintf(&chars, fmt, args);
	if (len > 0) {
		$(self, appendCharacters, chars);
	}

	free(chars);

	va_end(args);
}

/**
 * @fn void MutableString::appendString(MutableString *self, const String *string)
 *
 * @memberof MutableString
 */
static void appendString(MutableString *self, const String *string) {

	if (string) {
		$(self, appendCharacters, string->chars);
	}
}

/**
 * @fn void MutableString::deleteCharactersInRange(MutableString *self, const Range range)
 *
 * @memberof MutableString
 */
static void deleteCharactersInRange(MutableString *self, const Range range) {

	assert(range.location >= 0);
	assert(range.length <= self->string.length);

	ident ptr = self->string.chars + range.location;
	const size_t length = self->string.length - range.location - range.length;

	memmove(ptr, ptr + range.length, length);
}

/**
 * @fn MutableString *MutableString::init(MutableString *self)
 *
 * @memberof MutableString
 */
static MutableString *init(MutableString *self) {
	
	return $(self, initWithCapacity, 0);
}

/**
 * @fn MutableString *MutableString::initWithCapacity(MutableString *self, size_t capacity)
 *
 * @memberof MutableString
 */
static MutableString *initWithCapacity(MutableString *self, size_t capacity) {
	
	self = (MutableString *) super(String, self, initWithMemory, NULL, 0);
	if (self) {
		if (capacity) {
			self->string.chars = malloc(capacity);
			assert(self->string.chars);
			
			self->capacity = capacity;
		}
	}
	
	return self;
}

/**
 * @fn MutableString *MutableString::initWithString(MutableString *self, const String *string)
 *
 * @memberof MutableString
 */
static MutableString *initWithString(MutableString *self, const String *string) {

	self = $(self, init);
	if (self) {
		$(self, appendString, string);
	}

	return self;
}

/**
 * @fn void MutableString::replaceCharactersInRange(MutableString *self, const Range range, const String *string)
 *
 * @memberof MutableString
 */
static void replaceCharactersInRange(MutableString *self, const Range range, const String *string) {

	assert(range.location >= 0);
	assert(range.length <= self->string.length);

	String *remainder = str(self->string.chars + range.location + range.length);

	self->string.length = range.location;
	self->string.chars[self->string.length] = '\0';

	$(self, appendString, string);
	$(self, appendString, remainder);

	release(remainder);
}

/**
 * @fn MutableString *MutableString::string(void)
 *
 * @memberof MutableString
 */
static MutableString *string(void) {

	return $(alloc(MutableString), init);
}

/**
 * @fn MutableString *MutableString::stringWithCapacity(size_t capacity)
 *
 * @memberof MutableString
 */
static MutableString *stringWithCapacity(size_t capacity) {

	return $(alloc(MutableString), initWithCapacity, capacity);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;

	MutableStringInterface *mutableString = (MutableStringInterface *) clazz->interface;

	mutableString->appendCharacters = appendCharacters;
	mutableString->appendFormat = appendFormat;
	mutableString->appendString = appendString;
	mutableString->deleteCharactersInRange = deleteCharactersInRange;
	mutableString->init = init;
	mutableString->initWithCapacity = initWithCapacity;
	mutableString->initWithString = initWithString;
	mutableString->replaceCharactersInRange = replaceCharactersInRange;
	mutableString->string = string;
	mutableString->stringWithCapacity = stringWithCapacity;
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
