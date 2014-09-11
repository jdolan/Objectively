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
#include <stdio.h>
#include <string.h>

#include <Objectively/String.h>

#pragma mark - Object instance methods

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return new(String, ((String * ) self)->str);
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	free(((String *) self)->str);

	super(Object, self, dealloc);
}

/**
 * @see Object::init(id, id, va_list *)
 */
static Object *init(id obj, id interface, va_list *args) {

	String *self = (String *) super(Object, obj, init, interface, args);
	if (self) {
		self->interface = (StringInterface *) interface;

		const char *fmt = arg(args, const char *, NULL);
		if (fmt) {
			vasprintf(&self->str, fmt, *args);
			self->len = strlen(self->str);
		}
	}

	return (Object *) self;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static BOOL isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return YES;
	}

	if (other && other->class == (Class *) &__String) {

		const String *this = (String *) self;
		const String *that = (String *) other;

		RANGE range = { 0, this->len };
		return $(this, compareTo, that, range) == 0;
	}

	return NO;
}

#pragma mark - String instance methods

/**
 * @see String::appendFormat(String *, const char *, ...)
 */
static String *appendFormat(String *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char *str;
	vasprintf(&str, fmt, args);

	va_end(args);

	if (str) {
		String *string = new(String, NULL);

		string->str = str;
		string->len = strlen(string->str);

		$(self, appendString, string);

		release(string);
	}

	return self;
}

/**
 * @see String::appendString(String *, const String *)
 */
static String *appendString(String *self, const String *other) {

	if (other->len) {
		if (self->len) {
			const size_t size = self->len + other->len + 1;
			self->str = realloc(self->str, size);
			assert(self->str);

			strcat(self->str, other->str);
			self->str[size - 1] = '\0';
		} else {
			self->str = strdup(other->str);
		}

		self->len = strlen(self->str);
	}

	return self;
}

/**
 * @see String::compareTo(const String *, const String *, RANGE)
 */
static int compareTo(const String *self, const String *other, RANGE range) {

	assert(range.offset + range.length <= self->len);

	if (other) {
		return strncmp(self->str + range.offset, other->str, range.length);
	}

	return -1;
}

/**
 * @see String::hasPrefix(const String *, const String *)
 */
static BOOL hasPrefix(const String *self, const String *prefix) {

	if (prefix->len > self->len) {
		return NO;
	}

	RANGE range = { 0, prefix->len };
	return $(self, compareTo, prefix, range) == 0;
}

/**
 * @see String::hasSuffix(const String *, const String *)
 */
static BOOL hasSuffix(const String *self, const String *suffix) {

	if (suffix->len > self->len) {
		return NO;
	}

	RANGE range = { self->len - suffix->len, suffix->len };
	return $(self, compareTo, suffix, range) == 0;
}

/**
 * @see String::substring(const String *, RANGE)
 */
static String *substring(const String *self, RANGE range) {

	assert(range.offset + range.length <= self->len);

	char *str = malloc(range.length + 1);
	strncpy(str, self->str + range.offset, range.length);

	String *substring = new(String, NULL);

	substring->str = str;
	substring->len = strlen(str);

	return substring;
}

#pragma mark - Interface

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *self) {

	ObjectInterface *object = (ObjectInterface *) self->interface;

	object->init = init;
	object->copy = copy;
	object->dealloc = dealloc;
	object->isEqual = isEqual;

	StringInterface *string = (StringInterface *) self->interface;

	string->appendFormat = appendFormat;
	string->appendString = appendString;
	string->compareTo = compareTo;
	string->hasPrefix = hasPrefix;
	string->hasSuffix = hasSuffix;
	string->substring = substring;
}

Class __String = {
	.name = "String",
	.superclass = &__Object,
	.instanceSize = sizeof(String),
	.interfaceSize = sizeof(StringInterface),
	.initialize = initialize, };
