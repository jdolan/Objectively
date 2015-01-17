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
#include <iconv.h>
#include <locale.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableString.h>
#include <Objectively/String.h>

#define _Class _String

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	String *this = (String *) self;
	String *that = $$(String, stringWithCharacters, this->chars);

	return (Object *) that;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	String *this = (String *) self;

	free(this->chars);

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

	if (other && $(other, isKindOfClass, &_String)) {

		const String *this = (String *) self;
		const String *that = (String *) other;

		if (this->length == that->length) {

			const RANGE range = { 0, this->length };
			return $(this, compareTo, that, range) == SAME;
		}
	}

	return NO;
}

#pragma mark - StringInterface

typedef struct {
	StringEncoding to;
	StringEncoding from;
	char *in;
	size_t length;
	char *out;
	size_t size;
} Transcode;

/**
 * @brief Transcodes input from one character encoding to another via `iconv`.
 *
 * @param trans A Transcode struct.
 *
 * @return The number of bytes written to `trans->out`.
 */
static size_t transcode(Transcode *trans) {

	assert(trans);
	assert(trans->to);
	assert(trans->from);
	assert(trans->out);
	assert(trans->size);

	iconv_t cd = iconv_open(NameForStringEncoding(trans->to), NameForStringEncoding(trans->from));
	assert(cd != (iconv_t ) -1);

	char *in = trans->in;
	char *out = trans->out;

	size_t inBytesRemaining = trans->length;
	size_t outBytesRemaining = trans->size;

	const size_t ret = iconv(cd, &in, &inBytesRemaining, &out, &outBytesRemaining);
	assert(ret != (size_t ) -1);

	int err = iconv_close(cd);
	assert(err == 0);

	return trans->size - outBytesRemaining;
}

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
 * @see StringInterface::getData(const String *, StringEncoding)
 */
static Data *getData(const String *self, StringEncoding encoding) {

	Transcode trans = {
		.to = encoding,
		.from = STRING_ENCODING_UTF8,
		.in = self->chars,
		.length = self->length,
		.out = calloc(self->length, sizeof(Unicode)),
		.size = self->length * sizeof(Unicode)
	};

	assert(trans.out);

	const size_t size = transcode(&trans);
	assert(size <= trans.size);

	return $$(Data, dataWithMemory, trans.out, size);
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
 * @see StringInterface::initWithBytes(String *, const byte *, size_t, StringEncoding)
 */
static String *initWithBytes(String *self, const byte *bytes, size_t length, StringEncoding encoding) {

	if (bytes) {

		Transcode trans = {
			.to = STRING_ENCODING_UTF8,
			.from = encoding,
			.in = (char *) bytes,
			.length = length,
			.out = calloc(length * sizeof(Unicode) + 1, sizeof(char)),
			.size = length * sizeof(Unicode) + 1
		};

		assert(trans.out);

		const size_t size = transcode(&trans);
		assert(size < trans.size);

		id mem = realloc(trans.out, size + 1);
		assert(mem);

		return $(self, initWithMemory, mem, size);
	}

	return $(self, initWithMemory, NULL, 0);
}

/**
 * @see StringInterface::initWithCharacters(String *, const char *)
 */
static String *initWithCharacters(String *self, const char *chars) {

	if (chars) {

		id mem = strdup(chars);
		assert(mem);

		size_t length = strlen(chars);

		return $(self, initWithMemory, mem, length);
	}

	return $(self, initWithMemory, NULL, 0);
}

/**
 * @see StringInterface::initWithContentsOfFile(String *, const char *, StringEncoding)
 */
static String *initWithContentsOfFile(String *self, const char *path, StringEncoding encoding) {

	Data *data = $$(Data, dataWithContentsOfFile, path);
	if (data) {
		self = $(self, initWithData, data, encoding);
		release(data);
	} else {
		self = $(self, initWithMemory, NULL, 0);
	}

	return self;
}

/**
 * @see StringInterface::initWithData(String *, const Data *, StringEncoding)
 */
static String *initWithData(String *self, const Data *data, StringEncoding encoding) {

	assert(data);

	return $(self, initWithBytes, data->bytes, data->length, encoding);
}

/**
 * @see StringInterface::initWithFormat(id, const char *, ...)
 */
static String *initWithFormat(String *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	self = $(self, initWithVaList, fmt, args);

	va_end(args);

	return self;
}

/**
 * @see StringInterface::initWithMemory(String *, id, size_t)
 */
static String *initWithMemory(String *self, const id mem, size_t length) {

	self = (String *) super(Object, self, init);
	if (self) {

		if (mem) {
			self->chars = (char *) mem;
			self->length = length;
		}
	}

	return self;
}

/**
 * @see StringInterface::initWithVaList(String *, const char *, va_list)
 */
static String *initWithVaList(String *self, const char *fmt, va_list args) {

	self = (String *) super(Object, self, init);
	if (self) {

		if (fmt) {
			const int len = vasprintf(&self->chars, fmt, args);
			assert(len >= 0);

			self->length = len;
		}
	}

	return self;
}

/**
 * @see StringInterface::lowercaseString(const String *)
 */
static String *lowercaseString(const String *self) {

	return $(self, lowercaseStringWithLocale, LC_GLOBAL_LOCALE);
}

/**
 * @see StringInterface::lowercaseStringWithLocale(const String *, const Locale)
 */
static String *lowercaseStringWithLocale(const String *self, const Locale locale) {

	Data *data = $(self, getData, STRING_ENCODING_WCHAR);
	assert(data);

	const size_t codepoints = data->length / sizeof(Unicode);
	Unicode *unicode = (Unicode *) data->bytes;

	for (size_t i = 0; i < codepoints; i++, unicode++) {
		*unicode = towlower_l(*unicode, locale);
	}

	String *lowercase = $$(String, stringWithData, data, STRING_ENCODING_WCHAR);

	release(data);
	return lowercase;
}

/**
 * @see StringInterface::mutableCopy(const String *)
 */
static MutableString *mutableCopy(const String *self) {

	return $(alloc(MutableString), initWithString, self);
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
 * @see StringInterface::stringWithBytes(const byte *, size_t, StringEncoding)
 */
static String *stringWithBytes(const byte *bytes, size_t length, StringEncoding encoding) {

	return $(alloc(String), initWithBytes, bytes, length, encoding);
}

/**
 * @see StringInterface::stringWithCharacters(const char *)
 */
static String *stringWithCharacters(const char *chars) {

	return $(alloc(String), initWithCharacters, chars);
}

/**
 * @see StringInterface::stringWithContentsOfFile(const char *, StringEncoding)
 */
static String *stringWithContentsOfFile(const char *path, StringEncoding encoding) {

	return $(alloc(String), initWithContentsOfFile, path, encoding);
}

/**
 * @see StringInterface::stringWithData(const Data *data, StringEncoding)
 */
static String *stringWithData(const Data *data, StringEncoding encoding) {

	return $(alloc(String), initWithData, data, encoding);
}

/**
 * @see StringInterface::stringWithFormat(const char *fmt, ...)
 */
static String *stringWithFormat(const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	String *string = $(alloc(String), initWithVaList, fmt, args);

	va_end(args);

	return string;
}

/**
 * @see StringInterface::stringWithMemory(const id, size_t)
 */
static String *stringWithMemory(const id mem, size_t length) {

	return $(alloc(String), initWithMemory, mem, length);
}

/**
 * @see StringInterface::substring(const String *, RANGE)
 */
static String *substring(const String *self, const RANGE range) {

	assert(range.location + range.length <= self->length);

	id mem = calloc(range.length + 1, sizeof(char));
	assert(mem);

	strncpy(mem, self->chars + range.location, range.length);

	return $(alloc(String), initWithMemory, mem, range.length);
}

/**
 * @see StringInterface::uppercaseString(const String *)
 */
static String *uppercaseString(const String *self) {

	return $(self, uppercaseStringWithLocale, LC_GLOBAL_LOCALE);
}

/**
 * @see StringInterface::uppercaseStringWithLocale(const String *, const Locale)
 */
static String *uppercaseStringWithLocale(const String *self, const Locale locale) {

	Data *data = $(self, getData, STRING_ENCODING_WCHAR);
	assert(data);

	const size_t codepoints = data->length / sizeof(Unicode);
	Unicode *unicode = (Unicode *) data->bytes;

	for (size_t i = 0; i < codepoints; i++, unicode++) {
		*unicode = towupper_l(*unicode, locale);
	}

	String *uppercase = $$(String, stringWithData, data, STRING_ENCODING_WCHAR);

	release(data);
	return uppercase;
}

/**
 * @see StringInterface::writeToFile(const String *, const char *, StringEncoding)
 */
static BOOL writeToFile(const String *self, const char *path, StringEncoding encoding) {

	Data *data = $(self, getData, encoding);
	assert(data);

	const BOOL success = $(data, writeToFile, path);

	release(data);
	return success;
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
	string->getData = getData;
	string->hasPrefix = hasPrefix;
	string->hasSuffix = hasSuffix;
	string->initWithBytes = initWithBytes;
	string->initWithCharacters = initWithCharacters;
	string->initWithContentsOfFile = initWithContentsOfFile;
	string->initWithData = initWithData;
	string->initWithFormat = initWithFormat;
	string->initWithMemory = initWithMemory;
	string->initWithVaList = initWithVaList;
	string->lowercaseString = lowercaseString;
	string->lowercaseStringWithLocale = lowercaseStringWithLocale;
	string->mutableCopy = mutableCopy;
	string->rangeOfCharacters = rangeOfCharacters;
	string->rangeOfString = rangeOfString;
	string->stringWithBytes = stringWithBytes;
	string->stringWithCharacters = stringWithCharacters;
	string->stringWithContentsOfFile = stringWithContentsOfFile;
	string->stringWithData = stringWithData;
	string->stringWithFormat = stringWithFormat;
	string->stringWithMemory = stringWithMemory;
	string->substring = substring;
	string->uppercaseString = uppercaseString;
	string->uppercaseStringWithLocale = uppercaseStringWithLocale;
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

const char *NameForStringEncoding(StringEncoding encoding) {

	switch (encoding) {
		case STRING_ENCODING_ASCII:
			return "ASCII";
		case STRING_ENCODING_LATIN1:
			return "ISO-8859-1";
		case STRING_ENCODING_LATIN2:
			return "ISO-8859-2";
		case STRING_ENCODING_MACROMAN:
			return "MacRoman";
		case STRING_ENCODING_UTF16:
			return "UTF-16";
		case STRING_ENCODING_UTF32:
			return "UTF-32";
		case STRING_ENCODING_UTF8:
			return "UTF-8";
		case STRING_ENCODING_WCHAR:
			return "WCHAR_T";
	}
}

StringEncoding StringEncodingForName(const char *name) {

	if (strcasecmp("ASCII", name) == 0) {
		return STRING_ENCODING_ASCII;
	} else if (strcasecmp("ISO-8859-1", name) == 0) {
		return STRING_ENCODING_LATIN1;
	} else if (strcasecmp("ISO-8859-2", name) == 0) {
		return STRING_ENCODING_LATIN2;
	} else if (strcasecmp("MacRoman", name) == 0) {
		return STRING_ENCODING_MACROMAN;
	} else if (strcasecmp("UTF-16", name) == 0) {
		return STRING_ENCODING_UTF16;
	} else if (strcasecmp("UTF-32", name) == 0) {
		return STRING_ENCODING_UTF32;
	} else if (strcasecmp("UTF-8", name) == 0) {
		return STRING_ENCODING_UTF8;
	} else if (strcasecmp("WCHAR", name) == 0) {
		return STRING_ENCODING_WCHAR;
	}

	return STRING_ENCODING_ASCII;
}

String *str(const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	String *string = $(alloc(String), initWithVaList, fmt, args);

	va_end(args);

	return string;
}
