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
#include <stdlib.h>
#include <string.h>

#include "Boole.h"
#include "JSONSerialization.h"
#include "MutableData.h"
#include "MutableDictionary.h"
#include "MutableArray.h"
#include "Null.h"
#include "Number.h"
#include "String.h"

#define _Class _JSONSerialization

#pragma mark - JSONSerialization

/**
 * @brief A writer for generating JSON Data.
 */
typedef struct {
	MutableData *data;
	int options;
	size_t depth;
} JSONWriter;

static void writeElement(JSONWriter *writer, const ident obj);

/**
 * @brief Writes `null` to `writer`.
 * @param writer The JSONWriter.
 * @param null The Null to write.
 */
static void writeNull(JSONWriter *writer, const Null *null) {

	$(writer->data, appendBytes, (uint8_t *) "null", 4);
}

/**
 * @brief Writes `boolean` to `writer`.
 * @param writer The JSONWriter.
 * @param boolean The Boole to write.
 */
static void writeBoole(JSONWriter *writer, const Boole *boolean) {

	if (boolean->value) {
		$(writer->data, appendBytes, (uint8_t *) "true", 4);
	} else {
		$(writer->data, appendBytes, (uint8_t *) "false", 5);
	}
}

/**
 * @brief Writes `string` to `writer`.
 * @param writer The JSONWriter.
 * @param string The String to write.
 */
static void writeString(JSONWriter *writer, const String *string) {

	$(writer->data, appendBytes, (uint8_t *) "\"", 1);
	$(writer->data, appendBytes, (uint8_t *) string->chars, string->length);
	$(writer->data, appendBytes, (uint8_t *) "\"", 1);
}

/**
 * @brief Writes `number` to `writer`.
 * @param writer The JSONWriter.
 * @param number The Number to write.
 */
static void writeNumber(JSONWriter *writer, const Number *number) {

	String *string = $(alloc(String), initWithFormat, "%g", number->value);

	$(writer->data, appendBytes, (uint8_t *) string->chars, string->length);

	release(string);
}

/**
 * @brief Writes the label (field name) `label` to `writer`.
 * @param writer The JSONWriter.
 * @param label The label to write.
 */
static void writeLabel(JSONWriter *writer, const String *label) {

	writeString(writer, label);
	$(writer->data, appendBytes, (uint8_t *) ": ", 2);
}

/**
 * @brief Writes pretty formatting, if applicable, to `writer`.
 * @param writer The JSONWriter.
 */
static void writePretty(JSONWriter *writer) {

	if (writer->options & JSON_WRITE_PRETTY) {
		$(writer->data, appendBytes, (uint8_t *) "\n", 1);
		for (size_t i = 0; i < writer->depth; i++) {
			$(writer->data, appendBytes, (uint8_t *) "  ", 2);
		}
	}
}

/**
 * @brief Writes `object` to `writer`.
 * @param writer The JSONWriter.
 * @param object The object (Dictionary) to write.
 */
static void writeObject(JSONWriter *writer, const Dictionary *object) {

	$(writer->data, appendBytes, (uint8_t * ) "{", 1);
	writer->depth++;

	Array *keys = $(object, allKeys);
	if (writer->options & JSON_WRITE_SORTED) {
		Array *sorted = $(keys, sortedArray, StringCompare);
		release(keys);
		keys = sorted;
	}

	for (size_t i = 0; i < keys->count; i++) {

		writePretty(writer);

		const ident key = $(keys, objectAtIndex, i);
		writeLabel(writer, (String *) key);

		const ident obj = $(object, objectForKey, key);
		writeElement(writer, obj);

		if (i < keys->count - 1) {
			$(writer->data, appendBytes, (uint8_t *) ",", 1);
		}
	}

	release(keys);

	writer->depth--;
	writePretty(writer);

	$(writer->data, appendBytes, (uint8_t * ) "}", 1);
}

/**
 * @brief Writes `array` to `writer`.
 * @param writer The JSONWriter.
 * @param array The Array to write.
 */
static void writeArray(JSONWriter *writer, const Array *array) {

	$(writer->data, appendBytes, (uint8_t * ) "[", 1);
	writer->depth++;

	for (size_t i = 0; i < array->count; i++) {

		writePretty(writer);
		
		writeElement(writer, $(array, objectAtIndex, i));

		if (i < array->count - 1) {
			$(writer->data, appendBytes, (uint8_t *) ",", 1);
		}
	}

	writer->depth--;
	writePretty(writer);

	$(writer->data, appendBytes, (uint8_t * ) "]", 1);
}

/**
 * @brief Writes the specified JSON element to `writer`.
 * @param writer The JSONWriter.
 * @param obj The JSON element to write.
 */
static void writeElement(JSONWriter *writer, const ident obj) {

	const Object *object = cast(Object, obj);
	if (object) {
		if ($(object, isKindOfClass, _Dictionary())) {
			writeObject(writer, (const Dictionary *) object);
		} else if ($(object, isKindOfClass, _Array())) {
			writeArray(writer, (const Array *) object);
		} else if ($(object, isKindOfClass, _String())) {
			writeString(writer, (const String *) object);
		} else if ($(object, isKindOfClass, _Number())) {
			writeNumber(writer, (const Number *) object);
		} else if ($(object, isKindOfClass, _Boole())) {
			writeBoole(writer, (const Boole *) object);
		} else if ($(object, isKindOfClass, _Null())) {
			writeNull(writer, (const Null *) object);
		}
	}
}

/**
 * @fn Data *JSONSerialization::dataFromObject(const ident obj, int options)
 * @memberof JSONSerialization
 */
static Data *dataFromObject(const ident obj, int options) {

	if (obj) {
		JSONWriter writer = {
			.data = $(alloc(MutableData), init),
			.options = options
		};

		writeObject(&writer, obj);

		return (Data *) writer.data;
	}

	return NULL;
}

/**
 * @brief A reader for parsing JSON Data.
 */
typedef struct {
	const Data *data;
	int options;
	uint8_t *b;
} JSONReader;

static ident readElement(JSONReader *reader);

/**
 * @param reader The JSONReader.
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
 * @param reader The JSONReader.
 * @param stop A sequence of stop characters.
 * @return The `stop` byte found, or `-1` if `reader` is exhausted.
 */
static int readByteUntil(JSONReader *reader, const char *stop) {

	int b;

	while (true) {
		b = readByte(reader);
		if (b == -1 || strchr(stop, b)) {
			break;
		}
	}

	return b;
}

/**
 * @brief Consumes and verifies `bytes` from `reader.
 * @param reader The JSONReader.
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
 * @param reader The JSONReader.
 * @return The String.
 */
static String *readString(JSONReader *reader) {

	uint8_t *bytes = reader->b;

	const int b = readByteUntil(reader, "\"");
	assert(b == '"');

	const size_t length = reader->b - bytes - 1;
	return $$(String, stringWithBytes, bytes + 1, length, STRING_ENCODING_UTF8);
}

/**
 * @brief Reads a Number from `reader`.
 * @param reader The JSONReader.
 * @return The Number.
 */
static Number *readNumber(JSONReader *reader) {

	uint8_t *bytes = reader->b;

	uint8_t *end;
	double d = strtod((char *) bytes, (char **) &end);

	assert(end > bytes);
	reader->b = end - 1;

	return $$(Number, numberWithValue, d);
}

/**
 * @brief Reads a Boole from `reader`.
 * @param reader The JSONReader.
 * @return The Boole.
 */
static bool *readBoole(JSONReader *reader) {

	Boole *boolean = NULL;

	switch (*reader->b) {
		case 't':
			consumeBytes(reader, "true");
			boolean = $$(Boole, True);
			break;
		case 'f':
			consumeBytes(reader, "false");
			boolean = $$(Boole, False);
			break;
		default:
			assert(false);
	}

	return retain(boolean);
}

/**
 * @brief Reads Null from `Reader`.
 * @param reader The JSONReader.
 * @return Null.
 */
static Null *readNull(JSONReader *reader) {

	consumeBytes(reader, "null");

	Null *null = $$(Null, null);

	return retain(null);
}

/**
 * @brief Reads a label from `reader`.
 * @param reader The JSONReader.
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
 * @param reader The JSONReader.
 * @return The object.
 */
static Dictionary *readObject(JSONReader *reader) {

	MutableDictionary *object = $(alloc(MutableDictionary), init);

	while (true) {

		String *key = readLabel(reader);
		if (key == NULL) {
			const int b = readByteUntil(reader, "}");
			assert(b == '}');
			break;
		}

		const int b = readByteUntil(reader, ":");
		assert(b == ':');

		ident obj = readElement(reader);
		assert(obj);

		$(object, setObjectForKey, obj, key);

		release(key);
		release(obj);
	}

	return (Dictionary *) object;
}

/**
 * @brief Reads an array from `reader.
 * @param reader The JSONReader.
 * @return The array.
 */
static Array *readArray(JSONReader *reader) {

	MutableArray *array = $(alloc(MutableArray), init);

	while (true) {

		Object *obj = readElement(reader);
		if (obj == NULL) {
			const int b = readByteUntil(reader, "]");
			assert(b == ']');
			break;
		}

		$(array, addObject, obj);

		release(obj);
	}

	return (Array *) array;
}

/**
 * @brief Reads an element from `reader`. An element is any valid JSON type.
 * @param reader The JSONReader.
 * @return The element, or `NULL` if no element is available.
 */
static ident readElement(JSONReader *reader) {

	const int b = readByteUntil(reader, "{[\"tfn0123456789.-]}");
	if (b == '{') {
		return readObject(reader);
	} else if (b == '[') {
		return readArray(reader);
	} else if (b == '\"') {
		return readString(reader);
	} else if (b == 't' || b == 'f') {
		return readBoole(reader);
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
 * @fn ident JSONSerialization::objectFromData(const Data *data, int options)
 * @memberof JSONSerialization
 */
static ident objectFromData(const Data *data, int options) {

	if (data && data->length) {
		JSONReader reader = {
			.data = data,
			.options = options
		};

		return readElement(&reader);
	}

	return NULL;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((JSONSerializationInterface *) clazz->interface)->dataFromObject = dataFromObject;
	((JSONSerializationInterface *) clazz->interface)->objectFromData = objectFromData;
}

/**
 * @fn Class *JSONSerialization::_JSONSerialization(void)
 * @memberof JSONSerialization
 */
Class *_JSONSerialization(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "JSONSerialization",
			.superclass = _Object(),
			.instanceSize = sizeof(JSONSerialization),
			.interfaceOffset = offsetof(JSONSerialization, interface),
			.interfaceSize = sizeof(JSONSerializationInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
