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

#include "Config.h"

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "MutableString.h"

#define _Class _MutableString

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	const String *this = (String *) self;

	return (Object *) $(alloc(MutableString), initWithString, this);
}

#pragma mark - MutableString

/**
 * @fn void MutableString::appendCharacters(MutableString *self, const char *chars)
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
 * @memberof MutableString
 */
static void appendFormat(MutableString *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	$(self, appendVaList, fmt, args);

	va_end(args);
}

/**
 * @fn void MutableString::appendString(MutableString *self, const String *string)
 * @memberof MutableString
 */
static void appendString(MutableString *self, const String *string) {

	if (string) {
		$(self, appendCharacters, string->chars);
	}
}

/**
 * @fn void MutableString::appendVaList(MutableString *self, const char *fmt, va_list args)
 * @memberof MutableString
 */
static void appendVaList(MutableString *self, const char *fmt, va_list args) {
	char *chars;

	const int len = vasprintf(&chars, fmt, args);
	if (len > 0) {
		$(self, appendCharacters, chars);
	}

	free(chars);
}

/**
 * @fn void MutableString::deleteCharactersInRange(MutableString *self, const Range range)
 * @memberof MutableString
 */
static void deleteCharactersInRange(MutableString *self, const Range range) {

	assert(range.location >= 0);
	assert(range.length <= self->string.length);

	ident ptr = self->string.chars + range.location;
	const size_t length = self->string.length - range.location - range.length + 1;

	memmove(ptr, ptr + range.length, length);

	self->string.length -= range.length;
}

/**
 * @fn MutableString *MutableString::init(MutableString *self)
 * @memberof MutableString
 */
static MutableString *init(MutableString *self) {
	return $(self, initWithCapacity, 0);
}

/**
 * @fn MutableString *MutableString::initWithCapacity(MutableString *self, size_t capacity)
 * @memberof MutableString
 */
static MutableString *initWithCapacity(MutableString *self, size_t capacity) {

	self = (MutableString *) super(String, self, initWithMemory, NULL, 0);
	if (self) {
		if (capacity) {
			self->string.chars = calloc(capacity, sizeof(char));
			assert(self->string.chars);

			self->capacity = capacity;
		}
	}

	return self;
}

/**
 * @fn MutableString *MutableString::initWithString(MutableString *self, const String *string)
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
 * @fn void MutableString::insertCharactersAtIndex(MutableString *self, const char *chars, size_t index)
 * @memberof MutableString
 */
static void insertCharactersAtIndex(MutableString *self, const char *chars, size_t index) {

	const Range range = { .location = index };

	$(self, replaceCharactersInRange, range, chars);
}

/**
 * @fn void MutableString::insertStringAtIndex(MutableString *self, const String *string, size_t index)
 * @memberof MutableString
 */
static void insertStringAtIndex(MutableString *self, const String *string, size_t index) {

	$(self, insertCharactersAtIndex, string->chars, index);
}

/**
 * @fn void MutableString::replaceCharactersInRange(MutableString *self, const Range range, const char *chars)
 * @memberof MutableString
 */
static void replaceCharactersInRange(MutableString *self, const Range range, const char *chars) {

	assert(range.location >= 0);
	assert(range.location + range.length <= self->string.length);

	if (self->capacity == 0) {
		$(self, appendCharacters, chars);
	} else {
		char *remainder = strdup(self->string.chars + range.location + range.length);

		self->string.length = range.location;
		self->string.chars[range.location + 1] = '\0';

		$(self, appendCharacters, chars);
		$(self, appendCharacters, remainder);

		free(remainder);
	}
}

/**
 * @fn void MutableString::replaceOccurrencesOfCharacters(MutableString *self, const char *chars, const char *replacement)
 * @memberof MutableString
 */
static void replaceOccurrencesOfCharacters(MutableString *self, const char *chars, const char *replacement) {
	$(self, replaceOccurrencesOfCharactersInRange, chars, (Range) { .length = self->string.length }, replacement);
}

/**
 * @fn void MutableString::replaceOccurrencesOfCharactersInRange(MutableString *self, const char *chars, const Range range, const char *replacement)
 * @memberof MutableString
 */
static void replaceOccurrencesOfCharactersInRange(MutableString *self, const char *chars, const Range range, const char *replacement) {

	assert(chars);
	assert(replacement);

	assert(range.location >= 0);
	assert(range.location + range.length <= self->string.length);

	Range search = range;
	while (true) {

		const Range result = $((String *) self, rangeOfCharacters, chars, search);
		if (result.location == -1) {
			break;
		}

		$(self, replaceCharactersInRange, result, replacement);

		search.length -= (result.location - search.location);
		search.length -= strlen(replacement);
		search.length += ((int) strlen(replacement) - (int) strlen(chars));

		search.location = result.location + strlen(replacement);
	}
}

/**
 * @fn void MutableString::replaceOccurrencesOfString(MutableString *self, const String *string, const String *replacement)
 * @memberof MutableString
 */
static void replaceOccurrencesOfString(MutableString *self, const String *string, const String *replacement) {
	$(self, replaceOccurrencesOfStringInRange, string, (Range) { .length = self->string.length }, replacement);
}

/**
 * @fn void MutableString::replaceOccurrencesOfStringInRange(MutableString *self, const String *string, const Range range, const String *replacement)
 * @memberof MutableString
 */
static void replaceOccurrencesOfStringInRange(MutableString *self, const String *string, const Range range, const String *replacement) {

	assert(string);
	assert(replacement);

	$(self, replaceOccurrencesOfCharactersInRange, string->chars, range, replacement->chars);
}

/**
 * @fn void MutableString::replaceStringInRange(MutableString *self, const Range range, const String *string)
 * @memberof MutableString
 */
static void replaceStringInRange(MutableString *self, const Range range, const String *string) {

	$(self, replaceCharactersInRange, range, string->chars);
}

/**
 * @fn MutableString *MutableString::string(void)
 * @memberof MutableString
 */
static MutableString *string(void) {

	return $(alloc(MutableString), init);
}

/**
 * @fn MutableString *MutableString::stringWithCapacity(size_t capacity)
 * @memberof MutableString
 */
static MutableString *stringWithCapacity(size_t capacity) {

	return $(alloc(MutableString), initWithCapacity, capacity);
}

/**
 * @fn void MutableString::trim(MutableString *self)
 * @memberof MutableString
 */
static void trim(MutableString *self) {

	String *trimmed = $((String *) self, trimmedString);

	$(self, replaceStringInRange, (const Range) { .length = self->string.length }, trimmed);

	release(trimmed);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;

	((MutableStringInterface *) clazz->interface)->appendCharacters = appendCharacters;
	((MutableStringInterface *) clazz->interface)->appendFormat = appendFormat;
	((MutableStringInterface *) clazz->interface)->appendString = appendString;
	((MutableStringInterface *) clazz->interface)->appendVaList = appendVaList;
	((MutableStringInterface *) clazz->interface)->deleteCharactersInRange = deleteCharactersInRange;
	((MutableStringInterface *) clazz->interface)->init = init;
	((MutableStringInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
	((MutableStringInterface *) clazz->interface)->initWithString = initWithString;
	((MutableStringInterface *) clazz->interface)->insertCharactersAtIndex = insertCharactersAtIndex;
	((MutableStringInterface *) clazz->interface)->insertStringAtIndex = insertStringAtIndex;
	((MutableStringInterface *) clazz->interface)->replaceCharactersInRange = replaceCharactersInRange;
	((MutableStringInterface *) clazz->interface)->replaceOccurrencesOfCharacters = replaceOccurrencesOfCharacters;
	((MutableStringInterface *) clazz->interface)->replaceOccurrencesOfCharactersInRange = replaceOccurrencesOfCharactersInRange;
	((MutableStringInterface *) clazz->interface)->replaceOccurrencesOfString = replaceOccurrencesOfString;
	((MutableStringInterface *) clazz->interface)->replaceOccurrencesOfStringInRange = replaceOccurrencesOfStringInRange;
	((MutableStringInterface *) clazz->interface)->replaceStringInRange = replaceStringInRange;
	((MutableStringInterface *) clazz->interface)->string = string;
	((MutableStringInterface *) clazz->interface)->stringWithCapacity = stringWithCapacity;
	((MutableStringInterface *) clazz->interface)->trim = trim;
}

/**
 * @fn Class *MutableString::_MutableString(void)
 * @memberof MutableString
 */
Class *_MutableString(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "MutableString",
			.superclass = _String(),
			.instanceSize = sizeof(MutableString),
			.interfaceOffset = offsetof(MutableString, interface),
			.interfaceSize = sizeof(MutableStringInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

MutableString *mstr(const char *fmt, ...) {

	MutableString *string = $$(MutableString, string);

	va_list args;
	va_start(args, fmt);

	$(string, appendVaList, fmt, args);

	va_end(args);
	return string;
}
