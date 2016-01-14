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

#include <Objectively/Boole.h>
#include <Objectively/JSONSerialization.h>
#include <Objectively/MutableData.h>
#include <Objectively/MutableDictionary.h>
#include <Objectively/MutableArray.h>
#include <Objectively/Null.h>
#include <Objectively/Number.h>
#include <Objectively/String.h>

#define _Class _JSONSerialization

#pragma mark - JSONSerializationInterface

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
 * Writes `null` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param null The Null to write.
 */
static void writeNull(JSONWriter *writer, const Null *null) {

	$(writer->data, appendBytes, (byte *) "null", 4);
}

/**
 * Writes `boolean` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param boolean The Boole to write.
 */
static void writeBoole(JSONWriter *writer, const Boole *boolean) {

	if (boolean->value) {
		$(writer->data, appendBytes, (byte *) "true", 4);
	} else {
		$(writer->data, appendBytes, (byte *) "false", 5);
	}
}

/**
 * Writes `string` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param string The String to write.
 */
static void writeString(JSONWriter *writer, const String *string) {

	$(writer->data, appendBytes, (byte *) "\"", 1);
	$(writer->data, appendBytes, (byte *) string->chars, string->length);
	$(writer->data, appendBytes, (byte *) "\"", 1);
}

/**
 * Writes `number` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param number The Number to write.
 */
static void writeNumber(JSONWriter *writer, const Number *number) {

	String *string = $(alloc(String), initWithFormat, "%.5f", number->value);

	$(writer->data, appendBytes, (byte *) string->chars, string->length);

	release(string);
}

/**
 * Writes the label (field name) `label` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param label The label to write.
 */
static void writeLabel(JSONWriter *writer, const String *label) {

	writeString(writer, label);
	$(writer->data, appendBytes, (byte *) ": ", 2);
}

/**
 * Writes `object` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param object The object (Dictionary) to write.
 */
static void writeObject(JSONWriter *writer, const Dictionary *object) {

	$(writer->data, appendBytes, (byte * ) "{", 1);

	Array *keys = $(object, allKeys);
	for (size_t i = 0; i < keys->count; i++) {

		const ident key = $(keys, objectAtIndex, i);
		writeLabel(writer, (String *) key);

		const ident obj = $(object, objectForKey, key);
		writeElement(writer, obj);

		if (i < keys->count - 1) {
			$(writer->data, appendBytes, (byte *) ", ", 2);
		}
	}

	release(keys);

	$(writer->data, appendBytes, (byte * ) "}", 1);
}

/**
 * Writes `array` to `writer`.
 *
 * @param writer The JSONWriter.
 * @param array The Array to write.
 */
static void writeArray(JSONWriter *writer, const Array *array) {

	$(writer->data, appendBytes, (byte * ) "[", 1);

	for (size_t i = 0; i < array->count; i++) {

		writeElement(writer, $(array, objectAtIndex, i));

		if (i < array->count - 1) {
			$(writer->data, appendBytes, (byte *) ", ", 2);
		}
	}

	$(writer->data, appendBytes, (byte * ) "]", 1);
}

/**
 * Writes the specified JSON element to `writer`.
 *
 * @param writer The JSONWriter.
 * @param obj The JSON element to write.
 */
static void writeElement(JSONWriter *writer, const ident obj) {

	const Object *object = cast(Object, obj);
	if (object) {
		if ($(object, isKindOfClass, &_Dictionary)) {
			writeObject(writer, (Dictionary *) object);
		} else if ($(object, isKindOfClass, &_Array)) {
			writeArray(writer, (Array *) object);
		} else if ($(object, isKindOfClass, &_String)) {
			writeString(writer, (String *) object);
		} else if ($(object, isKindOfClass, &_Number)) {
			writeNumber(writer, (Number *) object);
		} else if ($(object, isKindOfClass, &_Boole)) {
			writeBoole(writer, (Boole *) object);
		} else if ($(object, isKindOfClass, &_Null)) {
			writeNull(writer, (Null *) object);
		}
	}
}

/**
 * @see JSONSerialization::dataFromObject(const id, int options)
 */
static Data *dataFromObject(const ident obj, int options) {

	JSONWriter writer = {
		.data = $(alloc(MutableData), init),
		.options = options
	};

	writeObject(&writer, obj);

	return (Data *) writer.data;
}

/**
 * @brief A reader for parsing JSON Data.
 */
typedef struct {
	const Data *data;
	int options;
	byte *b;
} JSONReader;

static ident readElement(JSONReader *reader);

/**
 * @param reader The JSONReader.
 *
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
 * @param reader The JSONReader.
 * @param stop A sequence of stop characters.
 *
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
 *
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
 *
 * @param reader The JSONReader.
 *
 * @return The String.
 */
static String *readString(JSONReader *reader) {

	byte *bytes = reader->b;

	const int b = readByteUntil(reader, "\"");
	assert(b == '"');

	const size_t length = reader->b - bytes - 1;
	return $$(String, stringWithBytes, bytes + 1, length, STRING_ENCODING_UTF8);
}

/**
 * @brief Reads a Number from `reader`.
 *
 * @param reader The JSONReader.
 *
 * @return The Number.
 */
static Number *readNumber(JSONReader *reader) {

	byte *bytes = reader->b;

	byte *end;
	double d = strtod((char *) bytes, (char **) &end);

	assert(end > bytes);
	reader->b = end - 1;

	return $$(Number, numberWithValue, d);
}

/**
 * @brief Reads a Boole from `reader`.
 *
 * @param reader The JSONReader.
 *
 * @return The Boole.
 */
static _Bool *readBoole(JSONReader *reader) {

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
 *
 * @param reader The JSONReader.
 *
 * @return Null.
 */
static Null *readNull(JSONReader *reader) {

	consumeBytes(reader, "null");

	Null *null = $$(Null, null);

	return retain(null);
}

/**
 * @brief Reads a label from `reader`.
 *
 * @param reader The JSONReader.
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
 * @param reader The JSONReader.
 *
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
 *
 * @param reader The JSONReader.
 *
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
 *
 * @param reader The JSONReader.
 *
 * @return The element, or `NULL` if no element is available..
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
 * @see JSONSerialization::objectFromData(const Data *, int)
 */
static ident objectFromData(const Data *data, int options) {

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

Class _JSONSerialization = {
	.name = "JSONSerialization",
	.superclass = &_Object,
	.instanceSize = sizeof(JSONSerialization),
	.interfaceOffset = offsetof(JSONSerialization, interface),
	.interfaceSize = sizeof(JSONSerializationInterface),
	.initialize = initialize,
};

#undef _Class
