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

#pragma once

#include <stddef.h>
#include <stdint.h>

#include <Objectively/Array.h>
#include <Objectively/Date.h>
#include <Objectively/Dictionary.h>
#include <Objectively/MutableArray.h>
#include <Objectively/MutableDictionary.h>
#include <Objectively/MutableSet.h>
#include <Objectively/Object.h>
#include <Objectively/URL.h>

/**
 * @file
 * @brief Standard JSONSerializer and JSONDeserializer functions for C struct fields.
 */

/**
 * @defgroup JSON JSON
 * @brief JSON serialization and deserialization.
 */

typedef struct JSONProperties JSONProperties;
typedef struct JSONProperty JSONProperty;
typedef struct JSONContext JSONContext;

/**
 * @brief Serializes a C struct field to an Objectively Object.
 * @param properties The JSONProperties of the containing struct.
 * @param property The JSONProperty for this field.
 * @param value Pointer to the field within the struct instance.
 * @param data Opaque user data from `property->data`.
 * @param context The active JSONContext, for nested serialization and error reporting.
 * @return An Objectively Object (String, Number, Boole, etc.), or `NULL` to omit the key.
 */
typedef ident (*JSONSerializer)(const JSONProperties *properties,
                                const JSONProperty *property,
                                ident value,
                                ident data,
                                JSONContext *context);

/**
 * @brief Deserializes a JSON value into a C struct field.
 * @param properties The JSONProperties of the containing struct.
 * @param property The JSONProperty for this field.
 * @param value The parsed JSON Object; guaranteed non-NULL at the call site.
 * @param field Pointer to the destination field within the struct instance.
 * @param context The active JSONContext, for nested deserialization and error reporting.
 * @return `true` on success; `false` on type mismatch.
 */
typedef bool (*JSONDeserializer)(const JSONProperties *properties,
                                 const JSONProperty *property,
                                 const Object *value,
                                 ident field,
                                 JSONContext *context);

/**
 * @brief Describes the JSON binding strategy for a single field of a C struct.
 */
struct JSONProperty {

  /**
   * @brief The JSON key name.  A `NULL` key terminates the property list.
   */
  const char *key;

  /**
   * @brief The byte offset of the field within the struct.
   */
  ptrdiff_t offset;

  /**
   * @brief The JSONSerializer, or `NULL` to omit this field from serialization.
   */
  JSONSerializer serializer;

  /**
   * @brief The JSONDeserializer, or `NULL` to omit this field from deserialization.
   */
  JSONDeserializer deserializer;

  /**
   * @brief Opaque user data passed to the serializer and deserializer.
   * @details Carries type-specific metadata, e.g. field size for character arrays.
   */
  ident data;
};

/**
 * @brief Describes the JSON binding for all fields of a C struct.
 */
struct JSONProperties {

  /**
   * @brief The struct type name.
   */
  const char *name;

  /**
   * @brief The struct size, i.e. `sizeof(Struct)`.
   */
  size_t size;

  /**
   * @brief The NULL-terminated JSONProperty array.
   */
  const JSONProperty *properties;
};

/**
 * @brief Creates a JSONProperty for a named field of a struct, deriving offset automatically.
 * @details The JSON key is the stringified field name.
 */
#define MakeJSONProperty(Struct, field, serializer_, deserializer_, data_) \
  (JSONProperty) { \
    .key = #field, \
    .offset = (ptrdiff_t) offsetof(Struct, field), \
    .serializer = (serializer_), \
    .deserializer = (deserializer_), \
    .data = (data_) \
  }

/**
 * @brief Creates a JSONProperties descriptor for a C struct type.
 */
#define MakeJSONProperties(Struct, ...) \
  (JSONProperties) { \
    .name = #Struct, \
    .size = sizeof(Struct), \
    .properties = (const JSONProperty[]) { \
      __VA_ARGS__, \
      (JSONProperty) { .key = NULL } \
    } \
  }

/**
 * @brief Yields the byte size of a struct field, as an `ident`.
 * @details Use as the `data_` argument to MakeJSONProperty for character array fields.
 */
#define JSONFieldSize(Struct, field) ((ident) (size_t) sizeof(((Struct *) NULL)->field))

#pragma mark - Standard JSONSerializers

/**
 * @brief Serializes a fixed-size `char[]` field to a JSON string.
 * @details Omits the key (returns `NULL`) when the field is empty.
 */
OBJECTIVELY_EXPORT ident JSONSerializeCharacters(const JSONProperties *properties,
                                                 const JSONProperty *property,
                                                 ident value,
                                                 ident data,
                                                 JSONContext *context);

/**
 * @brief Serializes a `char *` heap string field to a JSON string.
 * @details Omits the key (returns `NULL`) when the pointer is `NULL`.
 */
OBJECTIVELY_EXPORT ident JSONSerializeCString(const JSONProperties *properties,
                                              const JSONProperty *property,
                                              ident value,
                                              ident data,
                                              JSONContext *context);

/**
 * @brief Serializes an `int32_t` field to a JSON number.
 */
OBJECTIVELY_EXPORT ident JSONSerializeInt32(const JSONProperties *properties,
                                            const JSONProperty *property,
                                            ident value,
                                            ident data,
                                            JSONContext *context);

/**
 * @brief Serializes a `uint32_t` field to a JSON number.
 */
OBJECTIVELY_EXPORT ident JSONSerializeUint32(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             ident value,
                                             ident data,
                                             JSONContext *context);

/**
 * @brief Serializes a `double` field to a JSON number.
 */
OBJECTIVELY_EXPORT ident JSONSerializeDouble(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             ident value,
                                             ident data,
                                             JSONContext *context);

/**
 * @brief Serializes a `bool` field to a JSON boolean.
 */
OBJECTIVELY_EXPORT ident JSONSerializeBoole(const JSONProperties *properties,
                                            const JSONProperty *property,
                                            ident value,
                                            ident data,
                                            JSONContext *context);

/**
 * @brief Serializes a nested struct field to a JSON object.
 * @details `property->data` must point to a JSONProperties describing the nested type.
 */
OBJECTIVELY_EXPORT ident JSONSerializeStruct(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             ident value,
                                             ident data,
                                             JSONContext *context);

/**
 * @brief Serializes an inline array field to a JSON array.
 * @details `property->data` must point to a JSONArrayProperties.
 */
OBJECTIVELY_EXPORT ident JSONSerializeArray(const JSONProperties *properties,
                                            const JSONProperty *property,
                                            ident value,
                                            ident data,
                                            JSONContext *context);

#pragma mark - Standard JSONDeserializers

/**
 * @brief Deserializes a JSON string into a fixed-size `char[]` field.
 * @details `property->data` must carry the field size via JSONFieldSize.
 * @return `false` if the JSON value is not a String.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeCharacters(const JSONProperties *properties,
                                                  const JSONProperty *property,
                                                  const Object *value,
                                                  ident field,
                                                  JSONContext *context);

/**
 * @brief Deserializes a JSON string into a heap-allocated `char *` field.
 * @details The existing pointer is freed before replacement.
 * @return `false` if the JSON value is not a String.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeCString(const JSONProperties *properties,
                                               const JSONProperty *property,
                                               const Object *value,
                                               ident field,
                                               JSONContext *context);

/**
 * @brief Deserializes a JSON number into an `int32_t` field.
 * @return `false` if the JSON value is not a Number.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeInt32(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             const Object *value,
                                             ident field,
                                             JSONContext *context);

/**
 * @brief Deserializes a JSON number into a `uint32_t` field.
 * @return `false` if the JSON value is not a Number.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeUint32(const JSONProperties *properties,
                                              const JSONProperty *property,
                                              const Object *value,
                                              ident field,
                                              JSONContext *context);

/**
 * @brief Deserializes a JSON number into a `double` field.
 * @return `false` if the JSON value is not a Number.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeDouble(const JSONProperties *properties,
                                              const JSONProperty *property,
                                              const Object *value,
                                              ident field,
                                              JSONContext *context);

/**
 * @brief Deserializes a JSON boolean into a `bool` field.
 * @return `false` if the JSON value is not a Boole.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeBoole(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             const Object *value,
                                             ident field,
                                             JSONContext *context);

/**
 * @brief Deserializes a JSON object into a nested struct field.
 * @details `property->data` must point to a JSONProperties describing the nested type.
 * @return `false` if the JSON value is not a Dictionary.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeStruct(const JSONProperties *properties,
                                              const JSONProperty *property,
                                              const Object *value,
                                              ident field,
                                              JSONContext *context);

/**
 * @brief Deserializes a JSON array into an inline array field.
 * @details `property->data` must point to a JSONArrayProperties.
 * @return `false` if the JSON value is not an Array.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeArray(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             const Object *value,
                                             ident field,
                                             JSONContext *context);

#pragma mark - JSONArrayProperties

/**
 * @brief Sentinel for JSONArrayProperties::count_offset indicating no sibling count field.
 */
#define JSONArrayProperties_NoCount ((ptrdiff_t) -1)

/**
 * @brief Describes the JSON binding for an inline array field of a C struct.
 */
typedef struct {

  /**
   * @brief The JSONProperties of the array element type.
   */
  const JSONProperties *properties;

  /**
   * @brief The capacity of the array field (number of elements).
   */
  size_t count;

  /**
   * @brief Byte offset of the sibling element-count field, or JSONArrayProperties_NoCount.
   * @details When set, the deserializer writes the parsed element count here.
   */
  ptrdiff_t count_offset;

} JSONArrayProperties;

# pragma mark - Object serializers

/**
 * @brief Serializes a `String *` field to a JSON string.
 */
OBJECTIVELY_EXPORT ident JSONSerializeString(const JSONProperties *properties,
                                             const JSONProperty *property,
                                             ident value,
                                             ident data,
                                             JSONContext *context);

/**
 * @brief Deserializes a JSON string into a `String *` field.
 * @details The existing String is released before replacement.
 * @return `false` if the JSON value is not a String.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeString(const JSONProperties *properties,
                                              const JSONProperty *property,
                                              const Object *value,
                                              ident field,
                                              JSONContext *context);

/**
 * @brief Serializes a `URL *` field to a JSON string (the URL's string form).
 */
OBJECTIVELY_EXPORT ident JSONSerializeURL(const JSONProperties *properties,
                                          const JSONProperty *property,
                                          ident value,
                                          ident data,
                                          JSONContext *context);

/**
 * @brief Deserializes a JSON string into a `URL *` field via `initWithString`.
 * @details The existing URL is released before replacement.
 * @return `false` if the JSON value is not a String.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeURL(const JSONProperties *properties,
                                           const JSONProperty *property,
                                           const Object *value,
                                           ident field,
                                           JSONContext *context);

/**
 * @brief Serializes a `Date *` field to a JSON string (ISO 8601 by default).
 * @details Pass a `const char *` format string as `property->data` to override the format.
 */
OBJECTIVELY_EXPORT ident JSONSerializeDate(const JSONProperties *properties,
                                           const JSONProperty *property,
                                           ident value,
                                           ident data,
                                           JSONContext *context);

/**
 * @brief Deserializes a JSON string into a `Date *` field (ISO 8601 by default).
 * @details Pass a `const char *` format string as `property->data` to override the format.
 * @return `false` if the JSON value is not a String or cannot be parsed.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeDate(const JSONProperties *properties,
                                            const JSONProperty *property,
                                            const Object *value,
                                            ident field,
                                            JSONContext *context);

/**
 * @brief Serializes a `MutableArray *` field to a JSON array.
 * @details Elements are written as-is; each must be a JSON-compatible Objectively type.
 */
OBJECTIVELY_EXPORT ident JSONSerializeMutableArray(const JSONProperties *properties,
                                                   const JSONProperty *property,
                                                   ident value,
                                                   ident data,
                                                   JSONContext *context);

/**
 * @brief Deserializes a JSON array into a `MutableArray *` field.
 * @details The existing MutableArray is released before replacement. Elements are
 *   retained as their parsed Objectively types (String, Number, Boole, etc.).
 * @return `false` if the JSON value is not an Array.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeMutableArray(const JSONProperties *properties,
                                                    const JSONProperty *property,
                                                    const Object *value,
                                                    ident field,
                                                    JSONContext *context);

/**
 * @brief Serializes a `MutableSet *` field to a JSON array.
 * @details Elements are written as-is; each must be a JSON-compatible Objectively type.
 */
OBJECTIVELY_EXPORT ident JSONSerializeMutableSet(const JSONProperties *properties,
                                                 const JSONProperty *property,
                                                 ident value,
                                                 ident data,
                                                 JSONContext *context);

/**
 * @brief Deserializes a JSON array into a `MutableSet *` field.
 * @details The existing MutableSet is released before replacement.
 * @return `false` if the JSON value is not an Array.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeMutableSet(const JSONProperties *properties,
                                                  const JSONProperty *property,
                                                  const Object *value,
                                                  ident field,
                                                  JSONContext *context);

/**
 * @brief Serializes a `MutableDictionary *` field to a JSON object.
 * @details Values are written as-is; each must be a JSON-compatible Objectively type.
 */
OBJECTIVELY_EXPORT ident JSONSerializeMutableDictionary(const JSONProperties *properties,
                                                        const JSONProperty *property,
                                                        ident value,
                                                        ident data,
                                                        JSONContext *context);

/**
 * @brief Deserializes a JSON object into a `MutableDictionary *` field.
 * @details The existing MutableDictionary is released before replacement.
 * @return `false` if the JSON value is not a Dictionary.
 */
OBJECTIVELY_EXPORT bool JSONDeserializeMutableDictionary(const JSONProperties *properties,
                                                         const JSONProperty *property,
                                                         const Object *value,
                                                         ident field,
                                                         JSONContext *context);
