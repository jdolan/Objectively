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
#include <iconv.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include <Objectively/Hash.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableString.h>
#include <Objectively/String.h>

#if defined(__MINGW32__)
#define towlower_l _towlower_l
#define towupper_l _towupper_l
#endif

#define _Class _String

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	String *this = (String *) self;
	String *that = $$(String, stringWithCharacters, this->chars);

	return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	String *this = (String *) self;

	free(this->chars);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	return (String *) $(self, copy);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

	String *this = (String *) self;

	return HashForCString(HASH_SEED, this->chars);
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static _Bool isEqual(const Object *self, const Object *other) {

	if (super(Object, self, isEqual, other)) {
		return true;
	}

	if (other && $(other, isKindOfClass, _String())) {

		const String *this = (String *) self;
		const String *that = (String *) other;

		if (this->length == that->length) {

			const Range range = { 0, this->length };
			return $(this, compareTo, that, range) == OrderSame;
		}
	}

	return false;
}

#pragma mark - String

/**
 * @brief Character transcoding context for `iconv`.
 * @see iconv(3)
 */
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
 * @param trans A Transcode struct.
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
 * @fn Order String::compareTo(const String *self, const String *other, const Range range)
 * @memberof String
 */
static Order compareTo(const String *self, const String *other, const Range range) {

	assert(range.location + range.length <= self->length);

	if (other) {
		const int i = strncmp(self->chars + range.location, other->chars, range.length);
		if (i == 0) {
			return OrderSame;
		}
		if (i > 0) {
			return OrderDescending;
		}
	}

	return OrderAscending;
}

/**
 * @fn Array *String::componentsSeparatedByCharacters(const String *self, const char *chars)
 * @memberof String
 */
static Array *componentsSeparatedByCharacters(const String *self, const char *chars) {

	assert(chars);

	MutableArray *components = $(alloc(MutableArray), init);

	Range search = { 0, self->length };
	Range result = $(self, rangeOfCharacters, chars, search);

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
 * @fn Array *String::componentsSeparatedByString(const String *self, const String *string)
 * @memberof String
 */
static Array *componentsSeparatedByString(const String *self, const String *string) {

	assert(string);

	return $(self, componentsSeparatedByCharacters, string->chars);
}

/**
 * @fn Data *String::getData(const String *self, StringEncoding encoding)
 * @memberof String
 */
static Data *getData(const String *self, StringEncoding encoding) {

	Transcode trans = {
		.to = encoding,
		.from = STRING_ENCODING_UTF8,
		.in = self->chars,
		.length = self->length,
		.out = calloc(self->length, sizeof(Unicode) / sizeof(char)),
		.size = self->length * sizeof(Unicode)
	};

	assert(trans.out);

	const size_t size = transcode(&trans);
	assert(size <= trans.size);

	return $$(Data, dataWithMemory, trans.out, size);
}

/**
 * @fn _Bool String::hasPrefix(const String *self, const String *prefix)
 * @memberof String
 */
static _Bool hasPrefix(const String *self, const String *prefix) {

	if (prefix->length > self->length) {
		return false;
	}

	Range range = { 0, prefix->length };
	return $(self, compareTo, prefix, range) == OrderSame;
}

/**
 * @fn _Bool String::hasSuffix(const String *self, const String *suffix)
 * @memberof String
 */
static _Bool hasSuffix(const String *self, const String *suffix) {

	if (suffix->length > self->length) {
		return false;
	}

	Range range = { self->length - suffix->length, suffix->length };
	return $(self, compareTo, suffix, range) == OrderSame;
}

/**
 * @fn String *String::initWithBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding)
 * @memberof String
 */
static String *initWithBytes(String *self, const uint8_t *bytes, size_t length, StringEncoding encoding) {

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

		ident mem = realloc(trans.out, size + 1);
		assert(mem);

		return $(self, initWithMemory, mem, size);
	}

	return $(self, initWithMemory, NULL, 0);
}

/**
 * @fn String *String::initWithCharacters(String *self, const char *chars)
 * @memberof String
 */
static String *initWithCharacters(String *self, const char *chars) {

	if (chars) {

		ident mem = strdup(chars);
		assert(mem);

		size_t length = strlen(chars);

		return $(self, initWithMemory, mem, length);
	}

	return $(self, initWithMemory, NULL, 0);
}

/**
 * @fn String *String::initWithContentsOfFile(String *self, const char *path, StringEncoding encoding)
 * @memberof String
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
 * @fn String *String::initWithData(String *self, const Data *data, StringEncoding encoding)
 * @memberof String
 */
static String *initWithData(String *self, const Data *data, StringEncoding encoding) {

	assert(data);

	return $(self, initWithBytes, data->bytes, data->length, encoding);
}

/**
 * @fn String *String::initWithFormat(String *self, const char *fmt, ...)
 * @memberof String
 */
static String *initWithFormat(String *self, const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	self = $(self, initWithVaList, fmt, args);

	va_end(args);

	return self;
}

/**
 * @fn String *String::initWithMemory(String *self, const ident mem, size_t length)
 * @memberof String
 */
static String *initWithMemory(String *self, const ident mem, size_t length) {

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
 * @fn String *String::initWithVaList(String *self, const char *fmt, va_list args)
 * @memberof String
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
 * @fn String *String::lowercaseString(const String *self)
 * @memberof String
 */
static String *lowercaseString(const String *self) {
	return $(self, lowercaseStringWithLocale, NULL);
}

/**
 * @fn String *String::lowercaseStringWithLocale(const String *self, const Locale *locale)
 * @memberof String
 */
static String *lowercaseStringWithLocale(const String *self, const Locale *locale) {

	Data *data = $(self, getData, STRING_ENCODING_WCHAR);
	assert(data);

	const size_t codepoints = data->length / sizeof(Unicode);
	Unicode *unicode = (Unicode *) data->bytes;

	for (size_t i = 0; i < codepoints; i++, unicode++) {
		if (locale) {
			*unicode = towlower_l(*unicode, locale->locale);
		} else {
			*unicode = towlower(*unicode);
		}
	}

	String *lowercase = $$(String, stringWithData, data, STRING_ENCODING_WCHAR);

	release(data);
	return lowercase;
}

/**
 * @fn MutableString *String::mutableCopy(const String *self)
 * @memberof String
 */
static MutableString *mutableCopy(const String *self) {

	return $(alloc(MutableString), initWithString, self);
}

/**
 * @fn Range String::rangeOfCharacters(const String *self, const char *chars, const Range range)
 * @memberof String
 */
static Range rangeOfCharacters(const String *self, const char *chars, const Range range) {

	assert(chars);
	assert(range.location > -1);
	assert(range.length > 0);
	assert(range.location + range.length <= self->length);

	Range match = { -1, 0 };
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
 * @fn Range String::rangeOfString(const String *self, const String *string, const Range range)
 * @memberof String
 */
static Range rangeOfString(const String *self, const String *string, const Range range) {

	assert(string);

	return $(self, rangeOfCharacters, string->chars, range);
}

/**
 * @fn String *String::stringWithBytes(const uint8_t *bytes, size_t length, StringEncoding encoding)
 * @memberof String
 */
static String *stringWithBytes(const uint8_t *bytes, size_t length, StringEncoding encoding) {

	return $(alloc(String), initWithBytes, bytes, length, encoding);
}

/**
 * @fn String *String::stringWithCharacters(const char *chars)
 * @memberof String
 */
static String *stringWithCharacters(const char *chars) {

	return $(alloc(String), initWithCharacters, chars);
}

/**
 * @fn String *String::stringWithContentsOfFile(const char *path, StringEncoding encoding)
 * @memberof String
 */
static String *stringWithContentsOfFile(const char *path, StringEncoding encoding) {

	return $(alloc(String), initWithContentsOfFile, path, encoding);
}

/**
 * @fn String *String::stringWithData(const Data *data, StringEncoding encoding)
 * @memberof String
 */
static String *stringWithData(const Data *data, StringEncoding encoding) {

	return $(alloc(String), initWithData, data, encoding);
}

/**
 * @fn String *String::stringWithFormat(const char *fmt)
 * @memberof String
 */
static String *stringWithFormat(const char *fmt, ...) {

	va_list args;
	va_start(args, fmt);

	String *string = $(alloc(String), initWithVaList, fmt, args);

	va_end(args);

	return string;
}

/**
 * @fn String *String::stringWithMemory(const ident mem, size_t length)
 * @memberof String
 */
static String *stringWithMemory(const ident mem, size_t length) {

	return $(alloc(String), initWithMemory, mem, length);
}

/**
 * @fn String *String::substring(const String *string, const Range range)
 * @memberof String
 */
static String *substring(const String *self, const Range range) {

	assert(range.location + range.length <= self->length);

	ident mem = calloc(range.length + 1, sizeof(char));
	assert(mem);

	strncpy(mem, self->chars + range.location, range.length);

	return $(alloc(String), initWithMemory, mem, range.length);
}

/**
 * @fn String *String::uppercaseString(const String *self)
 * @memberof String
 */
static String *uppercaseString(const String *self) {
	return $(self, uppercaseStringWithLocale, NULL);
}

/**
 * @fn String *String::uppercaseStringWithLocale(const String *self, const Locale *locale)
 * @memberof String
 */
static String *uppercaseStringWithLocale(const String *self, const Locale *locale) {

	Data *data = $(self, getData, STRING_ENCODING_WCHAR);
	assert(data);

	const size_t codepoints = data->length / sizeof(Unicode);
	Unicode *unicode = (Unicode *) data->bytes;

	for (size_t i = 0; i < codepoints; i++, unicode++) {
		if (locale) {
			*unicode = towupper_l(*unicode, locale->locale);
		} else {
			*unicode = towupper(*unicode);
		}
	}

	String *uppercase = $$(String, stringWithData, data, STRING_ENCODING_WCHAR);

	release(data);
	return uppercase;
}

/**
 * @fn _Bool String::writeToFile(const String *self, const char *path, StringEncoding encoding)
 * @memberof String
 */
static _Bool writeToFile(const String *self, const char *path, StringEncoding encoding) {

	Data *data = $(self, getData, encoding);
	assert(data);

	const _Bool success = $(data, writeToFile, path);

	release(data);
	return success;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->copy = copy;
	object->dealloc = dealloc;
	object->description = description;
	object->hash = hash;
	object->isEqual = isEqual;

	StringInterface *string = (StringInterface *) clazz->def->interface;

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

/**
 * @fn Class *String::_String(void)
 * @memberof String
 */
Class *_String(void) {
	static Class clazz;
	static Once once;
	
	do_once(&once, {
		clazz.name = "String";
		clazz.superclass = _Object();
		clazz.instanceSize = sizeof(String);
		clazz.interfaceOffset = offsetof(String, interface);
		clazz.interfaceSize = sizeof(StringInterface);
		clazz.initialize = initialize;
	});
	
	return &clazz;
}

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

	return "ASCII";
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
