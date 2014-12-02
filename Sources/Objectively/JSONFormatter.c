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
#include <string.h>

#include <Objectively/Array.h>
#include <Objectively/Boolean.h>
#include <Objectively/Dictionary.h>
#include <Objectively/JSONFormatter.h>
#include <Objectively/Null.h>
#include <Objectively/Number.h>
#include <Objectively/String.h>

#define __Class __JSONFormatter

#pragma mark - JSONFormatterInterface

/**
 * @brief A context for parsing JSON Data.
 */
typedef struct {
	const Data *data;
	byte *b;
} JSONContext;

/**
 * @see JSONFormatterInterface::dataFromObject(JSONFormatter *, const id)
 */
static Data *dataFromObject(JSONFormatter *self, const id obj) {

	Data *data = $(alloc(Data), init);

	// TODO

	return data;
}

/**
 * @see JSONFormatterInterface::initWithOptions(JSONFormatter *)
 */
static JSONFormatter *initWithOptions(JSONFormatter *self, int options) {

	self = (JSONFormatter *) super(Object, self, init);
	if (self) {
		self->options = options;
	}

	return self;
}

/**
 * @return The next byte in `context`, or `-1` if `context` is exhausted.
 */
static int nextByte(JSONContext *context) {

	if (context->b - context->data->bytes < context->data->length) {
		return (int) *(++(context->b));
	}

	return -1;
}

/**
 * @brief Consume bytes from `context` until a byte from `stop` is found.
 *
 * @param stop A sequence of stop characters.
 *
 * @return The `stop` byte found, or `-1` if `context` is exhausted.
 */
static int nextByteUntil(JSONContext *context, const char *stop) {

	int b;

	while (YES) {
		b = nextByte(context);
		if (b == -1 || strchr(stop, b)) {
			break;
		}
	}

	return b;
}

/**
 * @brief Parses a label from `context`.
 *
 * @return The parsed label, or `NULL` on error.
 */
static String *parseLabel(JSONContext *context) {

	nextByte(context);
	byte *bytes = context->b;

	const int b = nextByteUntil(context, "\"");
	assert(b == '"');

	const size_t length = context->b - bytes;
	String *label = $(alloc(String), initWithBytes, bytes, length);

	const int c = nextByteUntil(context, ":");
	assert(c == ':');

	return label;
}

/**
 * @brief Consumes and verifies the bytes from `context.
 *
 * @param bytes The bytes to consume from `context`.
 */
static void assertBytes(JSONContext *context, const char *bytes) {

	for (size_t i = 1; i < strlen(bytes); i++) {
		int b = nextByte(context);
		assert(b == bytes[i]);
	}
}

static Dictionary *parseObject(JSONContext *context);
static Array *parseArray(JSONContext *context);

/**
 * @brief Parses an element from `context`. An element is any valid JSON type.
 *
 * @return The parsed element, or `NULL` on error.
 */
static id parseElement(JSONContext *context) {

	byte *bytes = context->b;

	int b = nextByteUntil(context, "\"{[0123456789-.tfn");
	if (b == '"') {
		int b = nextByteUntil(context, "\"");
		if (b == '"') {
			const size_t length = context->b - bytes;
			return $(alloc(String), initWithBytes, bytes, length);
		}
	} else if (b == '{') {
		return parseObject(context);
	} else if (b == '[') {
		return parseArray(context);
	} else if (b == 't') {
		assertBytes(context, "true");
		return $$(Boolean, yes);
	} else if (b == 'f') {
		assertBytes(context, "false");
		return $$(Boolean, no);
	} else if (b == 'n') {
		assertBytes(context, "null");
		return $$(Null, null);
	} else if (b == '-' || b == '.' || isdigit(b)) {
		double d = strtod((char *) bytes, (char **) &(context->b));
		return $(alloc(Number), initWithValue, d);
	}

	return NULL;
}

/**
 * @brief Parses an object from `context`. An object is a valid JSON structure.
 *
 * @return The parsed object.
 */
static Dictionary *parseObject(JSONContext *context) {

	Dictionary *object = $(alloc(Dictionary), init);

	while (YES) {
		const int b = nextByteUntil(context, "\"}");
		assert(b != -1);

		if (b == '}') {
			break;
		}

		id key = parseLabel(context);
		id obj = parseElement(context);

		$(object, setObjectForKey, obj, key);

		release(key);
		release(obj);
	}

	return object;
}

/**
 * @brief Parses an array from `context.
 *
 * @return The parsed array.
 */
static Array *parseArray(JSONContext *context) {

	Array *array = $(alloc(Array), init);

	while (YES) {
		const int b = nextByteUntil(context, "\"{[0123456789-.tfn]");
		assert(b != -1);

		if (b == ']') {
			break;
		}

		id obj = parseElement(context);

		$(array, addObject, obj);

		release(obj);
	}

	return array;
}

/**
 * @see JSONFormatterInterface::objectFromData(JSONFormatter *, const Data *)
 */
static id objectFromData(JSONFormatter *self, const Data *data) {

	JSONContext context = {
		.data = data,
		.b = data->bytes,
	};

	const int b = nextByteUntil(&context, "{[");
	assert(b != -1);

	if (b == '{') {
		return parseObject(&context);
	}

	return parseArray(&context);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	JSONFormatterInterface *json = (JSONFormatterInterface *) clazz->interface;

	json->dataFromObject = dataFromObject;
	json->initWithOptions = initWithOptions;
	json->objectFromData = objectFromData;
}

Class __JSONFormatter = {
	.name = "JSONFormatter",
	.superclass = &__Object,
	.instanceSize = sizeof(JSONFormatter),
	.interfaceOffset = offsetof(JSONFormatter, interface),
	.interfaceSize = sizeof(JSONFormatterInterface),
	.initialize = initialize,
};

#undef __Class
