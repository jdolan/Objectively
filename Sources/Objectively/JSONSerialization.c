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
#include <Objectively/JSONSerialization.h>
#include <Objectively/Null.h>
#include <Objectively/Number.h>
#include <Objectively/String.h>

#define __Class __JSONSerialization

#pragma mark - JSONSerializationInterface

/**
 * @see JSONSerializationInterface::dataFromObject(const id, int options)
 */
static Data *dataFromObject(const id obj, int options) {

	Data *data = $(alloc(Data), init);

	// TODO

	return data;
}

/**
 * @brief A reader for parsing JSON Data.
 */
typedef struct {
	const Data *data;
	int options;
	byte *b;
} JSONReader;

static id readElement(JSONReader *reader);

/**
 * @return The next byte in `reader`, or `-1` if `reader` is exhausted.
 */
static int readByte(JSONReader *reader) {

	if (reader->b) {
		if (reader->b - reader->data->bytes < reader->data->length) {
			return (int) *(++(reader->b));
		}
	} else {
		if (reader->data->bytes) {
			return (int) *(reader->b = reader->data->bytes);
		}
	}

	return -1;
}

/**
 * @brief Consume bytes from `reader` until a byte from `stop` is found.
 *
 * @param stop A sequence of stop characters.
 *
 * @return The `stop` byte found, or `-1` if `reader` is exhausted.
 */
static int readByteUntil(JSONReader *reader, const char *stop) {

	int b;

	while (YES) {
		b = readByte(reader);
		if (b == -1 || strchr(stop, b)) {
			break;
		}
	}

	return b;
}

/**
 * @brief Consumes and verifies `bytes` from `reader.
 *
 * @param bytes The bytes to consume from `reader`.
 */
static void consumeBytes(JSONReader *reader, const char *bytes) {

	for (size_t i = 1; i < strlen(bytes); i++) {
		int b = readByte(reader);
		assert(b == bytes[i]);
	}
}

/**
 * @brief Reads a String from `reader`.
 *
 * @return The String.
 */
static String *readString(JSONReader *reader) {

	byte *bytes = reader->b;

	const int b = readByteUntil(reader, "\"");
	assert(b == '"');

	const size_t length = reader->b - bytes - 1;
	return $(alloc(String), initWithBytes, bytes + 1, length);
}

/**
 * @brief Reads a Number from `reader`.
 *
 * @return The Number.
 */
static Number *readNumber(JSONReader *reader) {

	byte *bytes = reader->b;

	byte *end;
	double d = strtod((char *) bytes, (char **) &end);

	assert(end > bytes);
	reader->b = end - 1;

	return $(alloc(Number), initWithValue, d);
}

/**
 * Reads a Boolean from `reader`.
 *
 * @return The Boolean.
 */
static Boolean *readBoolean(JSONReader *reader) {

	Boolean *bool = NULL;

	switch (*reader->b) {
		case 't':
			consumeBytes(reader, "true");
			bool = $$(Boolean, yes);
			break;
		case 'f':
			consumeBytes(reader, "false");
			bool = $$(Boolean, no);
			break;
		default:
			assert(NO);
	}

	retain(bool);
	return bool;
}

/**
 * @brief Reads Null from `Reader`.
 *
 * @return Null.
 */
static Null *readNull(JSONReader *reader) {

	consumeBytes(reader, "null");
	Null *null = $$(Null, null);

	retain(null);
	return null;
}

/**
 * @brief Reads a label from `reader`.
 *
 * @return The label, or `NULL` on error.
 */
static String *readLabel(JSONReader *reader) {

	const int b = readByteUntil(reader, "\"}");
	if (b == '"') {
		return readString(reader);
	} if (b == '}') {
		reader->b--;
	}

	return NULL;
}

/**
 * @brief Reads an object from `reader`. An object is a valid JSON structure.
 *
 * @return The object.
 */
static Dictionary *readObject(JSONReader *reader) {

	Dictionary *object = $(alloc(Dictionary), init);

	while (YES) {

		String *key = readLabel(reader);
		if (key == NULL) {
			const int b = readByteUntil(reader, "}");
			assert(b == '}');
			break;
		}

		const int b = readByteUntil(reader, ":");
		assert(b == ':');

		id obj = readElement(reader);
		assert(obj);

		$(object, setObjectForKey, obj, key);

		release(key);
		release(obj);
	}

	return object;
}

/**
 * @brief Reads an array from `reader.
 *
 * @return The array.
 */
static Array *readArray(JSONReader *reader) {

	Array *array = $(alloc(Array), init);

	while (YES) {

		Object *obj = readElement(reader);
		if (obj == NULL) {
			const int b = readByteUntil(reader, "]");
			assert(b == ']');
			break;
		}

		$(array, addObject, obj);

		release(obj);
	}

	return array;
}

/**
 * @brief Reads an element from `reader`. An element is any valid JSON type.
 *
 * @return The element, or `NULL` if no element is available..
 */
static id readElement(JSONReader *reader) {

	const int b = readByteUntil(reader, "{[\"tfn0123456789.-]}");
	if (b == '"') {
		return readString(reader);
	} else if (b == '{') {
		return readObject(reader);
	} else if (b == '[') {
		return readArray(reader);
	} else if (b == 't' || b == 'f') {
		return readBoolean(reader);
	} else if (b == 'n') {
		return readNull(reader);
	} else if (b == '.' || b == '-' || isdigit(b)) {
		return readNumber(reader);
	} else if (b == ']' || b == '}') {
		reader->b--;
	}

	return NULL;
}

/**
 * @see JSONSerializationInterface::objectFromData(const Data *, int)
 */
static id objectFromData(const Data *data, int options) {

	JSONReader reader = {
		.data = data,
		.options = options
	};

	return readElement(&reader);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	JSONSerializationInterface *json = (JSONSerializationInterface *) clazz->interface;

	json->dataFromObject = dataFromObject;
	json->objectFromData = objectFromData;
}

Class __JSONSerialization = {
	.name = "JSONSerialization",
	.superclass = &__Object,
	.instanceSize = sizeof(JSONSerialization),
	.interfaceOffset = offsetof(JSONSerialization, interface),
	.interfaceSize = sizeof(JSONSerializationInterface),
	.initialize = initialize,
};

#undef __Class
