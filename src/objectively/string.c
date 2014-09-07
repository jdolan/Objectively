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

#include <objectively/string.h>

#pragma mark - Object instance methods

static id init(id obj, va_list *args) {

	String *self = super(Object, obj, init, args);
	if (self) {
		const char *fmt = arg(args, const char *, NULL);
		if (fmt) {
			vasprintf(&self->str, fmt, *args);
			self->len = strlen(self->str);
		}
	}

	return self;
}

static Object *copy(const Object *self) {

	return new(String, ((String * ) self)->str);
}

static void dealloc(Object *self) {

	free(((String *) self)->str);

	super(Object, self, dealloc);
}

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

		delete(string);
	}

	return self;
}

static String *appendString(String *self, const String *other) {

	if (other->len) {
		if (self->len) {
			const size_t size = self->len + other->len + 1;
			self->str = realloc(self->str, size);
			assert(self->str);

			strlcat(self->str, other->str, size);
		} else {
			self->str = strdup(other->str);
		}

		self->len = strlen(self->str);
	}

	return self;
}

static int compareTo(const String *self, const String *other, RANGE range) {

	assert(range.offset + range.length <= self->len);

	if (other) {
		return strncmp(self->str + range.offset, other->str, range.length);
	}

	return -1;
}

static BOOL hasPrefix(const String *self, const String *prefix) {

	if (prefix->len > self->len) {
		return NO;
	}

	RANGE range = { 0, prefix->len };
	return $(self, compareTo, prefix, range) == 0;
}

static BOOL hasSuffix(const String *self, const String *suffix) {

	if (suffix->len > self->len) {
		return NO;
	}

	RANGE range = { self->len - suffix->len, suffix->len };
	return $(self, compareTo, suffix, range) == 0;
}

static String *substring(const String *self, RANGE range) {

	assert(range.offset + range.length <= self->len);

	char *str = malloc(range.length + 1);
	strlcpy(str, self->str + range.offset, range.length + 1);

	String *substring = new(String, NULL);

	substring->str = str;
	substring->len = strlen(str);

	return substring;
}

#pragma mark - Interface

static void initialize(Class *class) {

	StringInterface *this = (StringInterface *) class->interface;

	this->init = init;
	this->copy = copy;
	this->dealloc = dealloc;
	this->isEqual = isEqual;

	this->appendFormat = appendFormat;
	this->appendString = appendString;
	this->compareTo = compareTo;
	this->hasPrefix = hasPrefix;
	this->hasSuffix = hasSuffix;
	this->substring = substring;
}

const Class __String = {
	.superclass = &__Object,
	.name = "String",
	.size = sizeof(String),
	.initialize = initialize,
	.init = init,
};

