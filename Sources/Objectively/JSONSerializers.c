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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "Boole.h"
#include "Date.h"
#include "DateFormatter.h"
#include "JSONContext.h"
#include "MutableArray.h"
#include "MutableDictionary.h"
#include "MutableSet.h"
#include "Null.h"
#include "Number.h"
#include "String.h"
#include "URL.h"

#pragma mark - Standard serializers

ident JSONSerializeCharacters(const JSONProperties *properties,
                               const JSONProperty *property,
                               ident value,
                               ident data,
                               JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  const char *s = value;
  if (s && s[0]) {
    return (ident) $$(String, stringWithCharacters, s);
  }

  return NULL;
}

ident JSONSerializeCString(const JSONProperties *properties,
                            const JSONProperty *property,
                            ident value,
                            ident data,
                            JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  const char *const *s = value;
  if (s && *s) {
    return (ident) $$(String, stringWithCharacters, *s);
  }

  return NULL;
}

ident JSONSerializeInt32(const JSONProperties *properties,
                          const JSONProperty *property,
                          ident value,
                          ident data,
                          JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  if (!value) {
    return NULL;
  }

  return (ident) $$(Number, numberWithValue, (double) *(const int32_t *) value);
}

ident JSONSerializeUint32(const JSONProperties *properties,
                           const JSONProperty *property,
                           ident value,
                           ident data,
                           JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  if (!value) {
    return NULL;
  }

  return (ident) $$(Number, numberWithValue, (double) *(const uint32_t *) value);
}

ident JSONSerializeDouble(const JSONProperties *properties,
                           const JSONProperty *property,
                           ident value,
                           ident data,
                           JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  if (!value) {
    return NULL;
  }

  return (ident) $$(Number, numberWithValue, *(const double *) value);
}

ident JSONSerializeBoole(const JSONProperties *properties,
                          const JSONProperty *property,
                          ident value,
                          ident data,
                          JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  if (!value) {
    return NULL;
  }

  return retain(*(const bool *) value ? $$(Boole, True) : $$(Boole, False));
}

ident JSONSerializeStruct(const JSONProperties *properties,
                           const JSONProperty *property,
                           ident value,
                           ident data,
                           JSONContext *context) {

  (void) properties;
  (void) property;

  const JSONProperties *child = data;
  if (!child || !child->properties || !value) {
    return NULL;
  }

  return (ident) $(context, dictionaryFromStruct, child, value);
}

ident JSONSerializeArray(const JSONProperties *properties,
                          const JSONProperty *property,
                          ident value,
                          ident data,
                          JSONContext *context) {

  (void) properties;
  (void) property;

  const JSONArrayProperties *array = data;
  if (!array || !array->properties || !value) {
    return NULL;
  }

  MutableArray *out = $(alloc(MutableArray), init);

  for (size_t i = 0; i < array->count; i++) {
    const ident instance = (uint8_t *) value + i * array->properties->size;
    Dictionary *dict = $(context, dictionaryFromStruct, array->properties, instance);
    $(out, addObject, dict);
    release(dict);
  }

  return (ident) out;
}

#pragma mark - Standard deserializers

bool JSONDeserializeCharacters(const JSONProperties *properties,
                                 const JSONProperty *property,
                                 const Object *value,
                                 ident field,
                                 JSONContext *context) {

  (void) properties;
  (void) context;

  char *dest = field;
  const size_t size = property->size;

  if (!dest || !size) {
    return false;
  }

  memset(dest, 0, size);

  if (!$(value, isKindOfClass, _String())) {
    return false;
  }

  strncpy(dest, cast(String, value)->chars, size - 1);
  dest[size - 1] = '\0';
  return true;
}

bool JSONDeserializeCString(const JSONProperties *properties,
                              const JSONProperty *property,
                              const Object *value,
                              ident field,
                              JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  char **dest = field;
  if (!dest) {
    return false;
  }

  free(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _String())) {
    return false;
  }

  *dest = strdup(cast(String, value)->chars);
  return true;
}

bool JSONDeserializeInt32(const JSONProperties *properties,
                            const JSONProperty *property,
                            const Object *value,
                            ident field,
                            JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  int32_t *dest = field;
  if (!dest) {
    return false;
  }

  *dest = 0;

  if (!$(value, isKindOfClass, _Number())) {
    return false;
  }

  *dest = (int32_t) cast(Number, value)->value;
  return true;
}

bool JSONDeserializeUint32(const JSONProperties *properties,
                             const JSONProperty *property,
                             const Object *value,
                             ident field,
                             JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  uint32_t *dest = field;
  if (!dest) {
    return false;
  }

  *dest = 0;

  if (!$(value, isKindOfClass, _Number())) {
    return false;
  }

  *dest = (uint32_t) cast(Number, value)->value;
  return true;
}

bool JSONDeserializeDouble(const JSONProperties *properties,
                             const JSONProperty *property,
                             const Object *value,
                             ident field,
                             JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  double *dest = field;
  if (!dest) {
    return false;
  }

  *dest = 0.0;

  if (!$(value, isKindOfClass, _Number())) {
    return false;
  }

  *dest = cast(Number, value)->value;
  return true;
}

bool JSONDeserializeBoole(const JSONProperties *properties,
                            const JSONProperty *property,
                            const Object *value,
                            ident field,
                            JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  bool *dest = field;
  if (!dest) {
    return false;
  }

  *dest = false;

  if (!$(value, isKindOfClass, _Boole())) {
    return false;
  }

  *dest = cast(Boole, value)->value;
  return true;
}

bool JSONDeserializeStruct(const JSONProperties *properties,
                             const JSONProperty *property,
                             const Object *value,
                             ident field,
                             JSONContext *context) {

  (void) properties;

  const JSONProperties *child = property->data;
  if (!child || !child->properties || !field) {
    return false;
  }

  memset(field, 0, child->size);

  if (!$(value, isKindOfClass, _Dictionary())) {
    return false;
  }

  return $(context, structFromDictionary, child, cast(Dictionary, value), field);
}

bool JSONDeserializeArray(const JSONProperties *properties,
                            const JSONProperty *property,
                            const Object *value,
                            ident field,
                            JSONContext *context) {

  (void) properties;

  const JSONArrayProperties *array = property->data;
  if (!array || !array->properties || !field) {
    return false;
  }

  uint8_t *instance = (uint8_t *) field - property->offset;
  if (array->count_offset != JSONArrayProperties_NoCount) {
    *(size_t *) (instance + array->count_offset) = 0;
  }

  memset(field, 0, array->properties->size * array->count);

  if (!$(value, isKindOfClass, _Array())) {
    return false;
  }

  const size_t n = $(context, structsFromArray, array->properties,
                                                    cast(Array, value), field, array->count);

  if (array->count_offset != JSONArrayProperties_NoCount) {
    *(size_t *) (instance + array->count_offset) = n;
  }

  return true;
}

#pragma mark - Object serializers / deserializers

ident JSONSerializeString(const JSONProperties *properties,
                           const JSONProperty *property,
                           ident value,
                           ident data,
                           JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  String **s = value;
  return *s ? retain(*s) : NULL;
}

bool JSONDeserializeString(const JSONProperties *properties,
                            const JSONProperty *property,
                            const Object *value,
                            ident field,
                            JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  String **dest = field;
  release(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _String())) {
    return false;
  }

  *dest = retain((String *) value);
  return true;
}

ident JSONSerializeURL(const JSONProperties *properties,
                        const JSONProperty *property,
                        ident value,
                        ident data,
                        JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  URL **url = value;
  return *url ? retain((*url)->urlString) : NULL;
}

bool JSONDeserializeURL(const JSONProperties *properties,
                         const JSONProperty *property,
                         const Object *value,
                         ident field,
                         JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  URL **dest = field;
  release(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _String())) {
    return false;
  }

  *dest = $(alloc(URL), initWithString, cast(String, value));
  return *dest != NULL;
}

ident JSONSerializeDate(const JSONProperties *properties,
                         const JSONProperty *property,
                         ident value,
                         ident data,
                         JSONContext *context) {

  (void) properties;
  (void) context;

  Date **date = value;
  if (!*date) {
    return NULL;
  }

  const char *fmt = data ? (const char *) data : DATEFORMAT_ISO8601;
  DateFormatter *formatter = $(alloc(DateFormatter), initWithFormat, fmt);
  String *string = $(formatter, stringFromDate, *date);
  release(formatter);
  return string;
}

bool JSONDeserializeDate(const JSONProperties *properties,
                          const JSONProperty *property,
                          const Object *value,
                          ident field,
                          JSONContext *context) {

  (void) properties;
  (void) context;

  Date **dest = field;
  release(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _String())) {
    return false;
  }

  const char *fmt = property->data ? (const char *) property->data : DATEFORMAT_ISO8601;
  DateFormatter *formatter = $(alloc(DateFormatter), initWithFormat, fmt);
  *dest = $(formatter, dateFromString, cast(String, value));
  release(formatter);
  return *dest != NULL;
}

ident JSONSerializeMutableArray(const JSONProperties *properties,
                                 const JSONProperty *property,
                                 ident value,
                                 ident data,
                                 JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  MutableArray **arr = value;
  return *arr ? retain(*arr) : NULL;
}

bool JSONDeserializeMutableArray(const JSONProperties *properties,
                                  const JSONProperty *property,
                                  const Object *value,
                                  ident field,
                                  JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  MutableArray **dest = field;
  release(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _Array())) {
    return false;
  }

  const Array *src = cast(Array, value);
  *dest = $(alloc(MutableArray), initWithCapacity, src->count);
  $(*(MutableArray **) dest, addObjectsFromArray, src);
  return true;
}

ident JSONSerializeMutableSet(const JSONProperties *properties,
                               const JSONProperty *property,
                               ident value,
                               ident data,
                               JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  MutableSet **set = value;
  if (!*set) {
    return NULL;
  }

  return (ident) $(cast(Set, *set), allObjects);
}

bool JSONDeserializeMutableSet(const JSONProperties *properties,
                                const JSONProperty *property,
                                const Object *value,
                                ident field,
                                JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  MutableSet **dest = field;
  release(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _Array())) {
    return false;
  }

  const Array *src = cast(Array, value);
  *dest = $(alloc(MutableSet), initWithCapacity, src->count);
  $(*(MutableSet **) dest, addObjectsFromArray, src);
  return true;
}

ident JSONSerializeMutableDictionary(const JSONProperties *properties,
                                      const JSONProperty *property,
                                      ident value,
                                      ident data,
                                      JSONContext *context) {

  (void) properties;
  (void) property;
  (void) data;
  (void) context;

  MutableDictionary **dict = value;
  return *dict ? retain(*dict) : NULL;
}

bool JSONDeserializeMutableDictionary(const JSONProperties *properties,
                                       const JSONProperty *property,
                                       const Object *value,
                                       ident field,
                                       JSONContext *context) {

  (void) properties;
  (void) property;
  (void) context;

  MutableDictionary **dest = field;
  release(*dest);
  *dest = NULL;

  if (!$(value, isKindOfClass, _Dictionary())) {
    return false;
  }

  *dest = $(alloc(MutableDictionary), init);
  $(*(MutableDictionary **) dest, addEntriesFromDictionary, cast(Dictionary, value));
  return true;
}
