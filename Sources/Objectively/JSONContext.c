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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Boole.h"
#include "JSONContext.h"
#include "Array.h"
#include "Data.h"
#include "Dictionary.h"
#include "Null.h"
#include "Number.h"
#include "String.h"

#define _Class _JSONContext

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  JSONContext *this = (JSONContext *) self;

  release(this->errors);

  super(Object, self, dealloc);
}

#pragma mark - Writer

/**
 * @brief Records an error on the context.
 * @param self The JSONContext; may be NULL (no-op).
 * @param code A JSON_ERROR_* code.
 * @param key The JSON key associated with the error, or NULL.
 * @param description A human-readable description.
 */
static void addError(JSONContext *self, int code, const char *key, const char *description) {

  if (!self) {
    return;
  }

  if (!self->errors) {
    self->errors = $(alloc(Array), init);
  }

  String *domain = $$(String, stringWithCharacters, "JSONContext");
  String *message;
  if (key) {
    message = $(alloc(String), initWithFormat, "%s: %s", key, description);
  } else {
    message = $$(String, stringWithCharacters, description);
  }

  Error *error = $(alloc(Error), initWithDomain, domain, code, message);
  $(self->errors, addObject, error);

  release(domain);
  release(message);
  release(error);
}

/**
 * @brief Internal state for JSON text generation.
 */
typedef struct {
  Data *data;
  int options;
  size_t depth;
} JSONWriter;

static void writeElement(JSONWriter *writer, const ident obj);

/**
 * @brief Writes `null` to `writer`.
 */
static void writeNull(JSONWriter *writer, const Null *null) {

  $(writer->data, appendBytes, (uint8_t *) "null", 4);
}

/**
 * @brief Writes a JSON boolean to `writer`.
 */
static void writeBoole(JSONWriter *writer, const Boole *boolean) {

  if (boolean->value) {
    $(writer->data, appendBytes, (uint8_t *) "true", 4);
  } else {
    $(writer->data, appendBytes, (uint8_t *) "false", 5);
  }
}

/**
 * @brief Writes a JSON-escaped string to `writer`.
 */
static void writeString(JSONWriter *writer, const String *string) {

  $(writer->data, appendBytes, (uint8_t *) "\"", 1);

  const char *s = string->chars;
  const char *end = s + string->length;
  while (s < end) {
    const unsigned char c = (unsigned char) *s++;
    switch (c) {
      case '"':  $(writer->data, appendBytes, (uint8_t *) "\\\"", 2); break;
      case '\\': $(writer->data, appendBytes, (uint8_t *) "\\\\", 2); break;
      case '\b': $(writer->data, appendBytes, (uint8_t *) "\\b",  2); break;
      case '\f': $(writer->data, appendBytes, (uint8_t *) "\\f",  2); break;
      case '\n': $(writer->data, appendBytes, (uint8_t *) "\\n",  2); break;
      case '\r': $(writer->data, appendBytes, (uint8_t *) "\\r",  2); break;
      case '\t': $(writer->data, appendBytes, (uint8_t *) "\\t",  2); break;
      default:
        if (c < 0x20) {
          char seq[7];
          snprintf(seq, sizeof(seq), "\\u%04x", c);
          $(writer->data, appendBytes, (uint8_t *) seq, 6);
        } else {
          $(writer->data, appendBytes, (uint8_t *) &c, 1);
        }
        break;
    }
  }

  $(writer->data, appendBytes, (uint8_t *) "\"", 1);
}

/**
 * @brief Writes a JSON number to `writer`.
 */
static void writeNumber(JSONWriter *writer, const Number *number) {

  // Use enough significant digits to preserve 32-bit epoch seconds and other
  // large integers exactly when round-tripping through JSON.
  String *string = $(alloc(String), initWithFormat, "%.17g", number->value);
  $(writer->data, appendBytes, (uint8_t *) string->chars, string->length);
  release(string);
}

/**
 * @brief Writes a JSON object key label to `writer`.
 */
static void writeLabel(JSONWriter *writer, const String *label) {

  writeString(writer, label);
  $(writer->data, appendBytes, (uint8_t *) ": ", 2);
}

/**
 * @brief Writes pretty-print indentation to `writer`, if enabled.
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
 * @brief Writes a JSON object (Dictionary) to `writer`.
 */
static void writeObject(JSONWriter *writer, const Dictionary *object) {

  $(writer->data, appendBytes, (uint8_t *) "{", 1);
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

  $(writer->data, appendBytes, (uint8_t *) "}", 1);
}

/**
 * @brief Writes a JSON array to `writer`.
 */
static void writeArray(JSONWriter *writer, const Array *array) {

  $(writer->data, appendBytes, (uint8_t *) "[", 1);
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

  $(writer->data, appendBytes, (uint8_t *) "]", 1);
}

/**
 * @brief Writes any JSON element to `writer`.
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

#pragma mark - Reader

/**
 * @brief Internal state for JSON text parsing.
 */
typedef struct {
  const Data *data;
  int options;
  uint8_t *b;
  bool error;
} JSONReader;

static ident readElement(JSONReader *reader);

/**
 * @brief Advances the reader by one byte.
 * @return The next byte, or -1 when the input is exhausted.
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
 * @brief Advances the reader until one of the bytes in `stop` is found.
 * @return The matched stop byte, or -1 when the input is exhausted.
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
 * @brief Consumes and validates a fixed byte sequence from `reader`.
 * @return `true` on success, `false` if the input did not match.
 */
static bool consumeBytes(JSONReader *reader, const char *bytes) {

  for (size_t i = 1; i < strlen(bytes); i++) {
    const int b = readByte(reader);
    if (b != bytes[i]) {
      reader->error = true;
      return false;
    }
  }

  return true;
}

/**
 * @brief Reads a JSON-escaped string from `reader`.
 */
static String *readString(JSONReader *reader) {

  Data *buf = $(alloc(Data), init);

  while (true) {
    int b = readByte(reader);
    if (b == -1 || b == '"') {
      break;
    }
    if (b == '\\') {
      b = readByte(reader);
      switch (b) {
        case '"':  b = '"';  break;
        case '\\': b = '\\'; break;
        case '/':  b = '/';  break;
        case 'b':  b = '\b'; break;
        case 'f':  b = '\f'; break;
        case 'n':  b = '\n'; break;
        case 'r':  b = '\r'; break;
        case 't':  b = '\t'; break;
        case 'u': {
          char hex[5] = {0};
          for (int i = 0; i < 4; i++) {
            int h = readByte(reader);
            if (h == -1) {
              reader->error = true;
              release(buf);
              return NULL;
            }
            hex[i] = (char) h;
          }
          unsigned long cp = strtoul(hex, NULL, 16);
          if (cp < 0x80) {
            uint8_t byte = (uint8_t) cp;
            $(buf, appendBytes, &byte, 1);
          } else if (cp < 0x800) {
            uint8_t bytes[2] = {
              (uint8_t) (0xC0 | (cp >> 6)),
              (uint8_t) (0x80 | (cp & 0x3F))
            };
            $(buf, appendBytes, bytes, 2);
          } else {
            uint8_t bytes[3] = {
              (uint8_t) (0xE0 | (cp >> 12)),
              (uint8_t) (0x80 | ((cp >> 6) & 0x3F)),
              (uint8_t) (0x80 | (cp & 0x3F))
            };
            $(buf, appendBytes, bytes, 3);
          }
          continue;
        }
        default:
          reader->error = true;
          release(buf);
          return NULL;
      }
    }
    uint8_t byte = (uint8_t) b;
    $(buf, appendBytes, &byte, 1);
  }

  String *string = $$(String, stringWithBytes, buf->bytes, buf->length, STRING_ENCODING_UTF8);
  release(buf);
  return string;
}

/**
 * @brief Reads a JSON number from `reader`.
 */
static Number *readNumber(JSONReader *reader) {

  uint8_t *bytes = reader->b;

  uint8_t *end;
  const double d = strtod((char *) bytes, (char **) &end);

  if (end <= bytes) {
    reader->error = true;
    return NULL;
  }

  reader->b = end - 1;
  return $$(Number, numberWithValue, d);
}

/**
 * @brief Reads a JSON boolean from `reader`.
 */
static Boole *readBoole(JSONReader *reader) {

  Boole *boolean = NULL;

  switch (*reader->b) {
    case 't':
      if (!consumeBytes(reader, "true")) {
        return NULL;
      }
      boolean = $$(Boole, True);
      break;
    case 'f':
      if (!consumeBytes(reader, "false")) {
        return NULL;
      }
      boolean = $$(Boole, False);
      break;
    default:
      reader->error = true;
      return NULL;
  }

  return retain(boolean);
}

/**
 * @brief Reads a JSON null from `reader`.
 */
static Null *readNull(JSONReader *reader) {

  if (!consumeBytes(reader, "null")) {
    return NULL;
  }

  return retain($$(Null, null));
}

/**
 * @brief Reads a JSON object key label from `reader`.
 */
static String *readLabel(JSONReader *reader) {

  const int b = readByteUntil(reader, "\"}");
  if (b == '"') {
    return readString(reader);
  }
  if (b == '}') {
    reader->b--;
  }

  return NULL;
}

/**
 * @brief Reads a JSON object (Dictionary) from `reader`.
 */
static Dictionary *readObject(JSONReader *reader) {

  Dictionary *object = $(alloc(Dictionary), init);

  while (true) {

    String *key = readLabel(reader);
    if (key == NULL) {
      const int b = readByteUntil(reader, "}");
      if (b != '}') {
        reader->error = true;
      }
      break;
    }

    const int b = readByteUntil(reader, ":");
    if (b != ':') {
      release(key);
      reader->error = true;
      break;
    }

    ident obj = readElement(reader);
    if (!obj) {
      release(key);
      reader->error = true;
      break;
    }

    $(object, setObjectForKey, obj, key);

    release(key);
    release(obj);
  }

  return (Dictionary *) object;
}

/**
 * @brief Reads a JSON array from `reader`.
 */
static Array *readArray(JSONReader *reader) {

  Array *array = $(alloc(Array), init);

  while (true) {

    Object *obj = readElement(reader);
    if (obj == NULL) {
      const int b = readByteUntil(reader, "]");
      if (b != ']') {
        reader->error = true;
      }
      break;
    }

    $(array, addObject, obj);
    release(obj);
  }

  return (Array *) array;
}

/**
 * @brief Reads any JSON element from `reader`.
 */
static ident readElement(JSONReader *reader) {

  const int b = readByteUntil(reader, "{[\"tfn0123456789.-]}");
  if (b == '{') {
    return readObject(reader);
  } else if (b == '[') {
    return readArray(reader);
  } else if (b == '"') {
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

#pragma mark - JSONContext

/**
 * @fn Data *JSONContext::dataFromObject(JSONContext *self, const ident obj, int options)
 * @memberof JSONContext
 */
static Data *dataFromObject(JSONContext *self, const ident obj, int options) {

  (void) self;

  if (obj) {
    JSONWriter writer = {
      .data = $(alloc(Data), init),
      .options = options
    };

    writeElement(&writer, obj);

    return (Data *) writer.data;
  }

  return NULL;
}

/**
 * @fn Data *JSONContext::dataFromStruct(JSONContext *self, const JSONProperties *properties, const ident instance)
 * @memberof JSONContext
 */
static Data *dataFromStruct(JSONContext *self, const JSONProperties *properties, const ident instance) {

  Dictionary *dict = $(self, dictionaryFromStruct, properties, instance);
  Data *data = dataFromObject(self, dict, 0);
  release(dict);
  return data;
}

/**
 * @fn Data *JSONContext::dataFromStructs(JSONContext *self, const JSONProperties *properties, const ident instances, size_t count)
 * @memberof JSONContext
 */
static Data *dataFromStructs(JSONContext *self, const JSONProperties *properties, const ident instances, size_t count) {

  if (!count) {
    return NULL;
  }

  Array *array = $(alloc(Array), init);

  for (size_t i = 0; i < count; i++) {
    const ident instance = instances + i * properties->size;
    Dictionary *dict = $(self, dictionaryFromStruct, properties, instance);
    $(array, addObject, dict);
    release(dict);
  }

  Data *data = dataFromObject(self, array, 0);
  release(array);
  return data;
}

/**
 * @brief Serializes a C struct instance to a Dictionary.
 */
static Dictionary *dictionaryFromStruct(JSONContext *self, const JSONProperties *properties, const ident instance) {

  if (!properties || !instance) {
    return NULL;
  }

  Dictionary *dict = $(alloc(Dictionary), init);

  for (const JSONProperty *p = properties->properties; p->key; p++) {
    if (!p->serializer) {
      continue;
    }

    const ident field = instance + p->offset;
    ident val = p->serializer(properties, p, field, p->data, self);

    if (val) {
      String *key = $$(String, stringWithCharacters, p->key);
      $(dict, setObjectForKey, val, key);
      release(key);
      release(val);
    }
  }

  return (Dictionary *) dict;
}

/**
 * @see Object::init(Object *)
 */
static JSONContext *init(JSONContext *self) {
  return (JSONContext *) super(Object, self, init);
}

/**
 * @fn ident JSONContext::objectFromData(JSONContext *self, const Data *data, int options)
 * @memberof JSONContext
 */
static ident objectFromData(JSONContext *self, const Data *data, int options) {

  if (data && data->length) {
    JSONReader reader = {
      .data = data,
      .options = options
    };

    ident obj = readElement(&reader);

    if (reader.error) {
      addError(self, 1, NULL, "JSON parse error");
      release(obj);
      return NULL;
    }

    return obj;
  }

  return NULL;
}

/**
 * @brief Deserializes a Dictionary into a C struct.
 */
static bool structFromDictionary(JSONContext *self, const JSONProperties *properties, const Dictionary *dictionary, ident instance) {

  if (!properties || !dictionary || !instance) {
    return false;
  }

  bool ok = true;

  for (const JSONProperty *p = properties->properties; p->key; p++) {
    if (!p->deserializer) {
      continue;
    }

    String *key = $$(String, stringWithCharacters, p->key);
    const Object *val = $(dictionary, objectForKey, key);
    release(key);

    if (!val) {
      continue;
    }

    if (!p->deserializer(properties, p, val, instance + p->offset, self)) {
      addError(self, 2, p->key, "type mismatch");
      ok = false;
    }
  }

  return ok;
}

/**
 * @brief Deserializes a JSON Array into an array of C structs.
 */
static size_t structsFromArray(JSONContext *self, const JSONProperties *properties, const Array *array, ident instances, size_t count) {

  if (!properties || !array || !instances || !count) {
    return 0;
  }

  const size_t n = min(array->count, count);

  for (size_t i = 0; i < n; i++) {
    const Dictionary *dictionary = cast(Dictionary, $(array, objectAtIndex, i));
    $(self, structFromDictionary, properties, dictionary, instances + i * properties->size);
  }

  return n;
}

/**
 * @fn bool JSONContext::structFromData(JSONContext *self, const JSONProperties *properties, const Data *data, ident instance)
 * @memberof JSONContext
 */
static bool structFromData(JSONContext *self, const JSONProperties *properties, const Data *data, ident instance) {

  ident obj = $(self, objectFromData, data, 0);
  if (!obj || !$((Object *) obj, isKindOfClass, _Dictionary())) {
    release(obj);
    return false;
  }

  const bool ok = structFromDictionary(self, properties, (Dictionary *) obj, instance);

  release(obj);
  return ok;
}

/**
 * @fn size_t JSONContext::structsFromData(JSONContext *self, const JSONProperties *properties, const Data *data, ident instances, size_t count)
 * @memberof JSONContext
 */
static size_t structsFromData(JSONContext *self, const JSONProperties *properties, const Data *data, ident instances, size_t count) {

  ident obj = objectFromData(self, data, 0);
  if (!obj || !$((Object *) obj, isKindOfClass, _Array())) {
    release(obj);
    return 0;
  }

  const size_t n = structsFromArray(self, properties, (Array *) obj, instances, count);

  release(obj);
  return n;
}

#pragma mark - Class lifecycle

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {
  (void) clazz;
}

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((JSONContextInterface *) clazz->interface)->dataFromObject = dataFromObject;
  ((JSONContextInterface *) clazz->interface)->dataFromStruct = dataFromStruct;
  ((JSONContextInterface *) clazz->interface)->dataFromStructs = dataFromStructs;
  ((JSONContextInterface *) clazz->interface)->dictionaryFromStruct = dictionaryFromStruct;
  ((JSONContextInterface *) clazz->interface)->init = init;
  ((JSONContextInterface *) clazz->interface)->objectFromData = objectFromData;
  ((JSONContextInterface *) clazz->interface)->structFromData = structFromData;
  ((JSONContextInterface *) clazz->interface)->structsFromData = structsFromData;
  ((JSONContextInterface *) clazz->interface)->structFromDictionary = structFromDictionary;
  ((JSONContextInterface *) clazz->interface)->structsFromArray = structsFromArray;
}

/**
 * @fn Class *JSONContext::_JSONContext(void)
 * @memberof JSONContext
 */
Class *_JSONContext(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "JSONContext",
      .superclass = _Object(),
      .instanceSize = sizeof(JSONContext),
      .interfaceOffset = offsetof(JSONContext, interface),
      .interfaceSize = sizeof(JSONContextInterface),
      .initialize = initialize,
      .destroy = destroy,
    });
  });

  return clazz;
}

#undef _Class
