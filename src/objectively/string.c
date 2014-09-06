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

#include <objectively/macros.h>
#include <objectively/string.h>

#pragma mark - Object

static id copy(const id self) {

	struct String *this = cast(String, self);

	return new(String, this->str);
}

static void dealloc(id self) {

	struct String *this = cast(String, self);

	free(this->str);

	super(Object, self, dealloc);
}

static BOOL isEqual(const id self, const id other) {

	if (super(Object, self, isEqual, other)) {
		return YES;
	}

	struct String *this = cast(String, self);

	RANGE range = { 0, this->len };
	return $(this, compareTo, other, range) == 0;
}

#pragma mark - String

static id appendFormat(id self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	char *str;
	vasprintf(&str, fmt, args);

	va_end(args);

	if (str) {
		struct String *this = cast(String, self);
		struct String *that = new(String, NULL);

		that->str = str;
		that->len = strlen(that->str);

		$(this, appendString, that);

		delete(that);
	}

	return self;
}

static id appendString(id self, const id other) {

	struct String *this = cast(String, self);
	struct String *that = cast(String, other);

	if (that->len) {
		if (this->len) {
			const size_t size = this->len + that->len + 1;
			this->str = realloc(this->str, size);
			assert(this->str);

			strlcat(this->str, that->str, size);
		} else {
			this->str = strdup(that->str);
		}

		this->len = strlen(this->str);
	}

	return self;
}

static int compareTo(const id self, const id other, RANGE range) {

	struct String *this = cast(String, self);
	struct String *that = cast(String, other);

	assert(range.offset + range.length <= this->len);

	if (that) {
		return strncmp(this->str + range.offset, that->str, range.length);
	}

	return -1;
}

static BOOL hasPrefix(const id self, const id prefix) {

	struct String *this = cast(String, self);
	struct String *that = cast(String, prefix);

	if (that->len > this->len) {
		return NO;
	}

	RANGE range = { 0, strlen(prefix) };
	return $(this, compareTo, that, range) == 0;
}

static BOOL hasSuffix(const id self, const id suffix) {

	struct String *this = cast(String, self);
	struct String *that = cast(String, suffix);

	if (that->len > this->len) {
		return NO;
	}

	RANGE range = { this->len - that->len, that->len };
	return $(this, compareTo, that, range) == 0;
}

static id substring(const id self, RANGE range) {

	struct String *this = cast(String, self);

	assert(range.offset + range.length <= this->len);

	char *str = malloc(range.length + 1);
	strlcpy(str, this->str + range.offset, range.length + 1);

	struct String *substring = new(String, NULL);

	substring->str = str;
	substring->len = strlen(str);

	return substring;
}

#pragma mark - Initializer

static id init(id self, va_list *args) {

	self = super(Object, self, init, args);
	if (self) {

		override(Object, self, copy, copy);
		override(Object, self, dealloc, dealloc);
		override(Object, self, isEqual, isEqual);
		override(Object, self, init, init);

		struct String *this = cast(String, self);

		this->appendFormat = appendFormat;
		this->appendString = appendString;
		this->compareTo = compareTo;
		this->hasPrefix = hasPrefix;
		this->hasSuffix = hasSuffix;
		this->substring = substring;

		const char *fmt = arg(args, const char *, NULL);
		if (fmt) {
			vasprintf(&this->str, fmt, *args);
			this->len = strlen(this->str);
		}
	}

	return self;
}

static struct String string;

static struct Class class = {
	.name = "String",
	.size = sizeof(struct String),
	.superclass = &Object,
	.archetype = &string,
	.init = init, };

const Class *String = &class;
