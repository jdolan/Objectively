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

#include <Objectively/Data.h>
#include <Objectively/Dictionary.h>
#include <Objectively/Object.h>

/**
 * @file
 * @brief JSON serialization and introspection.
 */

/**
 * @defgroup JSON JSON
 * @brief JSON serialization and introspection.
 */

/**
 * @brief The type of a JsonProperty value.
 */
typedef enum {

  /**
   * @brief A `char *` pointer (size == 0) or a fixed `char[N]` buffer (size > 0).
   */
  JsonPropertyTypeCharacters,

  /**
   * @brief A signed integer whose width is given by `size` (1, 2, 4, or 8 bytes).
   */
  JsonPropertyTypeInteger,

  /**
   * @brief A floating-point value: `float` (size == 4) or `double` (size == 8).
   */
  JsonPropertyTypeDouble,

  /**
   * @brief A boolean value whose width is given by `size` (typically 1 for `bool`/`_Bool`).
   */
  JsonPropertyTypeBool,

} JsonPropertyType;

typedef struct JsonProperty JsonProperty;

/**
 * @brief Describes a single field of a C struct for JSON serialization.
 * @details Use `MakeJsonProperty` to construct and `MakeJsonProperties` to build a
 * NULL-terminated array.  Pass the array to `JSONSerialization::dictionaryFromInstance`
 * or `JSONSerialization::dataFromInstances`.
 *
 * @code
 * static const JsonProperty frag_properties[] = MakeJsonProperties(
 *     MakeJsonProperty("name", JsonPropertyTypeCharacters, offsetof(MyStruct, name), sizeof(((MyStruct *)0)->name)),
 *     MakeJsonProperty("count", JsonPropertyTypeInteger,    offsetof(MyStruct, count), sizeof(((MyStruct *)0)->count)),
 *     MakeJsonProperty("ratio", JsonPropertyTypeDouble,     offsetof(MyStruct, ratio), sizeof(((MyStruct *)0)->ratio)),
 *     MakeJsonProperty("flag",  JsonPropertyTypeBool,       offsetof(MyStruct, flag),  sizeof(((MyStruct *)0)->flag))
 * );
 * @endcode
 */
struct JsonProperty {

  /**
   * @brief The JSON key name.  A `NULL` name terminates the list.
   */
  const char *name;

  /**
   * @brief The property type.
   */
  JsonPropertyType type;

  /**
   * @brief Byte offset of the field within the struct (use `offsetof`).
   */
  ptrdiff_t offset;

  /**
   * @brief Type-dependent size.
   * @details
   *   - `JsonPropertyTypeCharacters`: 0 for `char *`, > 0 for `char[N]`.
   *   - `JsonPropertyTypeInteger`: `sizeof` the integer field (1, 2, 4, or 8).
   *   - `JsonPropertyTypeDouble`: `sizeof` the float/double field (4 or 8).
   *   - `JsonPropertyTypeBool`: `sizeof` the boolean field.
   */
  size_t size;
};

/**
 * @brief Creates a JsonProperty with the given parameters.
 */
#define MakeJsonProperty(name, type, offset, size) \
  (JsonProperty) { (name), (type), (ptrdiff_t)(offset), (size) }

/**
 * @brief Creates a NULL-terminated array of JsonProperty descriptors.
 */
#define MakeJsonProperties(...) \
  { \
    __VA_ARGS__, \
    MakeJsonProperty(NULL, 0, 0, 0) \
  }

/**
 * @brief JSON write options.
 */
typedef enum {

  /**
   * @brief Enables pretty (indented) formatting of JSON output.
   */
  JSON_WRITE_PRETTY = 0x1,

  /**
   * @brief Enables lexicographic sorting of JSON output.
   */
  JSON_WRITE_SORTED = 0x2
}  JSONWriteOptions;

typedef struct JSONSerialization JSONSerialization;
typedef struct JSONSerializationInterface JSONSerializationInterface;

/**
 * @brief JSON serialization and introspection.
 * @extends Object
 * @ingroup JSON
 */
struct JSONSerialization {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  JSONSerializationInterface *interface;
};

/**
 * @brief The JSONSerialization interface.
 */
struct JSONSerializationInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @static
   * @fn Data *JSONSerialization::dataFromObject(const ident obj, int options)
   * @brief Serializes the given Object to JSON Data.
   * @param obj The Object to serialize.
   * @param options A bitwise-or of `JSON_WRITE_*`.
   * @return The resulting JSON Data.
   * @memberof JSONSerialization
   */
  Data *(*dataFromObject)(const ident obj, int options);

  /**
   * @static
   * @fn Dictionary *JSONSerialization::dictionaryFromInstance(const JsonProperty *properties, const void *instance)
   * @brief Creates an Objectively Dictionary from a single C struct instance.
   * @param properties A NULL-terminated array of JsonProperty descriptors.
   * @param instance Pointer to the struct instance to read from.
   * @return A new Dictionary whose keys are the property names and whose values are the
   *   corresponding Objectively objects.  The caller is responsible for releasing it.
   * @memberof JSONSerialization
   */
  Dictionary *(*dictionaryFromInstance)(const JsonProperty *properties, const void *instance);

  /**
   * @static
   * @fn Data *JSONSerialization::dataFromInstances(const JsonProperty *properties, const void *instances, size_t count, size_t stride)
   * @brief Serializes an array of C structs to a JSON array.
   * @param properties A NULL-terminated array of JsonProperty descriptors.
   * @param instances Pointer to the first struct in the array.
   * @param count The number of structs to serialize.
   * @param stride The byte distance between consecutive structs (typically `sizeof(Struct)`).
   * @return The resulting JSON Data (a top-level JSON array), or `NULL` if `count` is zero.
   * @memberof JSONSerialization
   */
  Data *(*dataFromInstances)(const JsonProperty *properties, const void *instances, size_t count, size_t stride);

  /**
   * @static
   * @fn size_t JSONSerialization::instancesFromData(const JsonProperty *properties, const Data *data, void *instances, size_t stride, size_t count)
   * @brief Deserializes a JSON array into an array of C structs.
   * @param properties A NULL-terminated array of JsonProperty descriptors.
   * @param data The JSON Data containing a top-level array.
   * @param instances Pointer to a caller-allocated buffer of at least `count` structs.
   * @param stride The byte distance between consecutive structs (typically `sizeof(Struct)`).
   * @param count The capacity of the `instances` buffer.
   * @return The number of structs actually written (min of parsed count and `count`).
   * @memberof JSONSerialization
   */
  size_t (*instancesFromData)(const JsonProperty *properties, const Data *data, void *instances, size_t stride, size_t count);

  /**
   * @static
   * @fn ident JSONSerialization::objectFromData(const Data *data, int options)
   * @brief Parses an Object from the specified Data.
   * @param data The JSON Data.
   * @param options A bitwise-or of `JSON_READ_*`.
   * @return The Object, or `NULL` on error.
   * @memberof JSONSerialization
   */
  ident (*objectFromData)(const Data *data, int options);
};

/**
 * @fn Class *JSONSerialization::_JSONSerialization(void)
 * @brief The JSONSerialization archetype.
 * @return The JSONSerialization Class.
 * @memberof JSONSerialization
 */
OBJECTIVELY_EXPORT Class *_JSONSerialization(void);
