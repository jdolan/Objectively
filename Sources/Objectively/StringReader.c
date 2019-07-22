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
#include <wchar.h>

#include "StringReader.h"

#define _Class _StringReader

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	StringReader *this = (StringReader *) self;

	return (Object *) $(alloc(StringReader), initWithString, this->string);
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	StringReader *this = (StringReader *) self;

	release(this->string);

	super(Object, self, dealloc);
}

#pragma mark - StringReader

/**
 * @fn StringReader *StringReader::initWithCharacters(StringReader *self, const char *chars)
 * @memberof StringReader
 */
static StringReader *initWithCharacters(StringReader *self, const char *chars) {

	String *string = str(chars);

	self = $(self, initWithString, string);

	release(string);

	return self;
}

/**
 * @fn StringReader *StringReader::initWithString(StringReader *self, String *string)
 * @memberof StringReader
 */
static StringReader *initWithString(StringReader *self, String *string) {

	self = (StringReader *) super(Object, self, init);
	if (self) {
		self->string = retain(string);
		$(self, reset);
	}
	return self;
}

/**
 * @fn Unicode StringReader::next(StringReader *self)
 * @memberof StringReader
 */
static Unicode next(StringReader *self, StringReaderMode mode) {
	Unicode c;

	const int bytes = mbtowc(&c, self->head, MB_CUR_MAX);
	if (bytes > 0) {
		if (mode == StringReaderRead) {
			self->head += bytes;
		}
		return c;
	}

	return READER_EOF;
}

/**
 * @fn Unicode StringReader::peek(StringReader *self)
 * @memberof StringReader
 */
static Unicode peek(StringReader *self) {
	return $(self, next, StringReaderPeek);
}

/**
 * @fn int StringReader::read(StringReader *self)
 * @memberof StringReader
 */
static Unicode read(StringReader *self) {
	return $(self, next, StringReaderRead);
}

/**
 * @fn int StringReader::readToken(StringReader *self, const Unicode *charset, Unicode *stop)
 * @memberof StringReader
 */
static String *readToken(StringReader *self, const Unicode *charset, Unicode *stop) {

	while (true) {
		const Unicode c = $(self, peek);
		if (c == READER_EOF || wcschr(charset, c) == NULL) {
			break;
		}
		$(self, read);
	}

	const char *start = self->head;

	while (true) {
		const Unicode c = $(self, peek);
		if (c == READER_EOF || wcschr(charset, c)) {
			if (stop) {
				*stop = c;
			}
			break;
		}
		$(self, read);
	}

	if (self->head - start > 0) {
		const Range range = {
			.location = start - self->string->chars,
			.length = self->head - start
		};
		return $(self->string, substring, range);
	}

	return NULL;
}

/**
 * @fn void StringReader::reset(StringReader *self)
 * @memberof StringReader
 */
static void reset(StringReader *self) {
	self->head = self->string->chars;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((StringReaderInterface *) clazz->interface)->initWithCharacters = initWithCharacters;
	((StringReaderInterface *) clazz->interface)->initWithString = initWithString;
	((StringReaderInterface *) clazz->interface)->next = next;
	((StringReaderInterface *) clazz->interface)->peek = peek;
	((StringReaderInterface *) clazz->interface)->read = read;
	((StringReaderInterface *) clazz->interface)->readToken = readToken;
	((StringReaderInterface *) clazz->interface)->reset = reset;
}

/**
 * @fn Class *StringReader::_StringReader(void)
 * @memberof StringReader
 */
Class *_StringReader(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "StringReader",
			.superclass = _Object(),
			.instanceSize = sizeof(StringReader),
			.interfaceOffset = offsetof(StringReader, interface),
			.interfaceSize = sizeof(StringReaderInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
