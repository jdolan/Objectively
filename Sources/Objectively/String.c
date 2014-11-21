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
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <Objectively/String.h>

#define __Class __String

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	String *this = (String *) self;
	String *that = $(alloc(String), initWithCharacters, this->str);

	return (Object *) that;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	String *this = (String *) self;

	free(this->str);

	super(Object, self, dealloc);
}

/**
 * @see ObjectInterface::hash(const Object *)
 */
static int hash(const Object *self) {

	String *this = (String *) self;

	int hash = 13;
	for (size_t i = 0; i < this->length; i++) {

		int shift;
		if (i & 1) {
			shift = 16 + (i % 16);
		} else {
			shift = (i % 16);
		}

		hash += ((int) this->str[i]) << shift;
	}

	return hash;
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

			RANGE range = { 0, this->length };
			return $(this, compareTo, that, range) == SAME;
		}
	}

	return NO;
}

#pragma mark - StringInterface

/**
 * @see StringInterface::appendFormat(String *, const char *, ...)
 */
static String *appendFormat(String *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char *str;
	vasprintf(&str, fmt, args);

	va_end(args);

	if (str) {
		String *string = alloc(String);

		string->str = str;
		string->length = strlen(string->str);

		$(self, appendString, string);

		release(string);
	}

	return self;
}

/**
 * @see StringInterface::appendString(String *, const String *)
 */
static String *appendString(String *self, const String *other) {

	if (other->length) {
		if (self->length) {
			const size_t size = self->length + other->length + 1;

			self->str = realloc(self->str, size);
			assert(self->str);

			memcpy(self->str + self->length, other->str, other->length);
			self->str[size - 1] = '\0';
		} else {
			self->str = strdup(other->str);
		}

		self->length = strlen(self->str);
	}

	return self;
}

/**
 * @see StringInterface::compareTo(const String *, const String *, RANGE)
 */
static ORDER compareTo(const String *self, const String *other, RANGE range) {

	assert(range.location + range.length <= self->length);

	if (other) {
		const int i = strncmp(self->str + range.location, other->str, range.length);
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

	Array *components = $(alloc(Array), init);

	RANGE search = { .location = 0, .length = self->length };
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

	return components;
}

/**
 * @see StringInterface::componentsSeparatedByString(const String *, const String *)
 */
static Array *componentsSeparatedByString(const String *self, const String *string) {

	assert(string);

	return $(self, componentsSeparatedByCharacters, string->str);
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
 * @see StringInterface::init(String *)
 */
static String *init(String *self) {

	return $(self, initWithCharacters, NULL);
}

/**
 * @see StringInterface::initWithCharacters(String *, const char *)
 */
static String *initWithCharacters(String *self, const char *chars) {

	return $(self, initWithMemory, chars ? strdup(chars) : NULL);
}

/**
 * @see StringInterface::initWithFormat(id, const char *, ...)
 */
static String *initWithFormat(String *self, const char *fmt, ...) {

	char *str = NULL;

	if (fmt) {
		va_list args;

		va_start(args, fmt);
		vasprintf(&str, fmt, args);
		va_end(args);
	}

	return $(self, initWithMemory, str);
}

/**
 * @see StringInterface::initWithMemory(String *self, id mem)
 */
static String *initWithMemory(String *self, id mem) {

	self = (String *) super(Object, self, init);
	if (self) {
		self->locale = LC_GLOBAL_LOCALE;

		if (mem) {
			self->str = (char *) mem;
			self->length = strlen(self->str);
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
		string->str[i] = tolower_l(string->str[i], self->locale);
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

	const char *str = self->str + range.location;
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

	return $(self, rangeOfCharacters, string->str, range);
}

/**
 * @see StringInterface::substring(const String *, RANGE)
 */
static String *substring(const String *self, const RANGE range) {

	assert(range.location + range.length <= self->length);

	char *str = calloc(1, range.length + 1);
	memcpy(str, self->str + range.location, range.length);

	String *substring = alloc(String);

	substring->str = str;
	substring->length = strlen(str);

	return substring;
}

/**
 * @see StringInterface::uppercaseString(const String *)
 */
static String *uppercaseString(const String *self) {

	String *string = (String *) $((Object *) self, copy);

	for (size_t i = 0; i < string->length; i++) {
		string->str[i] = toupper_l(string->str[i], self->locale);
	}

	return string;
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

	StringInterface *string = (StringInterface *) clazz->interface;

	string->appendFormat = appendFormat;
	string->appendString = appendString;
	string->compareTo = compareTo;
	string->componentsSeparatedByCharacters = componentsSeparatedByCharacters;
	string->componentsSeparatedByString = componentsSeparatedByString;
	string->hasPrefix = hasPrefix;
	string->hasSuffix = hasSuffix;
	string->init = init;
	string->initWithCharacters = initWithCharacters;
	string->initWithFormat = initWithFormat;
	string->initWithMemory = initWithMemory;
	string->lowercaseString = lowercaseString;
	string->rangeOfCharacters = rangeOfCharacters;
	string->rangeOfString = rangeOfString;
	string->substring = substring;
	string->uppercaseString = uppercaseString;
}

Class __String = {
	.name = "String",
	.superclass = &__Object,
	.instanceSize = sizeof(String),
	.interfaceOffset = offsetof(String, interface),
	.interfaceSize = sizeof(StringInterface),
	.initialize = initialize,
};

#undef __Class
