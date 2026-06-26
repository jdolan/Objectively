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

#include <Objectively/Data.h>
#include <Objectively/Dictionary.h>
#include <Objectively/Error.h>
#include <Objectively/JSONSerializers.h>
#include <Objectively/Array.h>
#include <Objectively/Object.h>

/**
 * @file
 * @brief JSONContext class for JSON serialization and deserialization.
 */

typedef struct JSONContextInterface JSONContextInterface;

// ---------------------------------------------------------------------------
// JSONWriteOptions
// ---------------------------------------------------------------------------

/**
 * @brief Options for JSON serialization.
 */
typedef enum {

  /**
   * @brief Enables pretty (indented) formatting of JSON output.
   */
  JSON_WRITE_PRETTY = 0x1,

  /**
   * @brief Enables lexicographic sorting of JSON object keys.
   */
  JSON_WRITE_SORTED = 0x2,

} JSONWriteOptions;

// ---------------------------------------------------------------------------
// JSONContext
// ---------------------------------------------------------------------------

/**
 * @brief A context for JSON serialization and deserialization.
 * @details Create one instance per operation.  Errors encountered during
 *   parsing or type-binding accumulate in `errors`.
 * @extends Object
 * @ingroup JSON
 */
struct JSONContext {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  JSONContextInterface *interface;

  /**
   * @brief Errors accumulated during the operation.
   * @details Lazily allocated; `NULL` until the first error is recorded.
   */
  Array *errors;
};

/**
 * @brief The JSONContext interface.
 */
struct JSONContextInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn Data *JSONContext::dataFromObject(JSONContext *self, const ident obj, int options)
   * @brief Serializes an Objectively object graph to JSON Data.
   * @param self The JSONContext.
   * @param obj The root Object to serialize.
   * @param options A bitwise-or of JSONWriteOptions.
   * @return The resulting JSON Data.
   * @memberof JSONContext
   */
  Data *(*dataFromObject)(JSONContext *self, const ident obj, int options);

  /**
   * @fn Data *JSONContext::dataFromStruct(JSONContext *self, const JSONProperties *properties, const ident instance)
   * @brief Serializes a C struct instance to a JSON object.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the struct type.
   * @param instance Pointer to the struct instance.
   * @return The resulting JSON Data.
   * @memberof JSONContext
   */
  Data *(*dataFromStruct)(JSONContext *self,
                             const JSONProperties *properties,
                             const ident instance);

  /**
   * @fn Data *JSONContext::dataFromStructs(JSONContext *self, const JSONProperties *properties, const ident instances, size_t count)
   * @brief Serializes an array of C struct instances to a JSON array.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the struct type.
   * @param instances Pointer to the first instance.
   * @param count The number of instances to serialize.
   * @return The resulting JSON Data (a top-level JSON array), or `NULL` if `count` is zero.
   * @memberof JSONContext
   */
  Data *(*dataFromStructs)(JSONContext *self,
                              const JSONProperties *properties,
                              const ident instances,
                              size_t count);

  /**
   * @fn Dictionary *JSONContext::dictionaryFromStruct(JSONContext *self, const JSONProperties *properties, const ident instance)
   * @brief Serializes a C struct instance to a Dictionary.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the struct type.
   * @param instance Pointer to the struct instance.
   * @return A new Dictionary, or `NULL` on error.
   * @memberof JSONContext
   */
  Dictionary *(*dictionaryFromStruct)(JSONContext *self,
                                         const JSONProperties *properties,
                                         const ident instance);

  /**
   * @fn JSONContext *JSONContext::init(JSONContext *self)
   * @brief Initializes a JSONContext.
   * @param self The JSONContext.
   * @return The initialized JSONContext.
   * @memberof JSONContext
   */
  JSONContext *(*init)(JSONContext *self);

  /**
   * @fn ident JSONContext::objectFromData(JSONContext *self, const Data *data, int options)
   * @brief Parses a JSON Data buffer into an Objectively object graph.
   * @param self The JSONContext.
   * @param data The JSON Data to parse.
   * @param options Reserved; pass 0.
   * @return The root Object (Dictionary or Array), or `NULL` on parse error.
   * @memberof JSONContext
   */
  ident (*objectFromData)(JSONContext *self, const Data *data, int options);

  /**
   * @fn bool JSONContext::structFromData(JSONContext *self, const JSONProperties *properties, const Data *data, ident instance)
   * @brief Deserializes a top-level JSON object into a C struct.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the struct type.
   * @param data The JSON Data containing a top-level object.
   * @param instance Pointer to the caller-allocated struct to populate.
   * @return `true` if parsing succeeded and all recognized fields were bound without type errors.
   * @memberof JSONContext
   */
  bool (*structFromData)(JSONContext *self,
                            const JSONProperties *properties,
                            const Data *data,
                            ident instance);

  /**
   * @fn bool JSONContext::structFromDictionary(JSONContext *self, const JSONProperties *properties, const Dictionary *dictionary, ident instance)
   * @brief Deserializes a Dictionary into a C struct.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the struct type.
   * @param dictionary The source Dictionary.
   * @param instance Pointer to the caller-allocated struct to populate.
   * @return `true` if all recognized fields were bound without type errors.
   * @memberof JSONContext
   */
  bool (*structFromDictionary)(JSONContext *self,
                                  const JSONProperties *properties,
                                  const Dictionary *dictionary,
                                  ident instance);

  /**
   * @fn size_t JSONContext::structsFromArray(JSONContext *self, const JSONProperties *properties, const Array *array, ident instances, size_t count)
   * @brief Deserializes an Array of Dictionaries into an array of C structs.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the element type.
   * @param array The source Array of Dictionary objects.
   * @param instances Pointer to a caller-allocated buffer of at least `count` structs.
   * @param count The capacity of the `instances` buffer.
   * @return The number of structs actually written.
   * @memberof JSONContext
   */
  size_t (*structsFromArray)(JSONContext *self,
                                const JSONProperties *properties,
                                const Array *array,
                                ident instances,
                                size_t count);

  /**
   * @fn size_t JSONContext::structsFromData(JSONContext *self, const JSONProperties *properties, const Data *data, ident instances, size_t count)
   * @brief Deserializes a top-level JSON array into an array of C structs.
   * @param self The JSONContext.
   * @param properties The JSONProperties for the element type.
   * @param data The JSON Data containing a top-level array.
   * @param instances Pointer to a caller-allocated buffer of at least `count` structs.
   * @param count The capacity of the `instances` buffer.
   * @return The number of structs actually written.
   * @memberof JSONContext
   */
  size_t (*structsFromData)(JSONContext *self,
                               const JSONProperties *properties,
                               const Data *data,
                               ident instances,
                               size_t count);
};

/**
 * @fn Class *JSONContext::_JSONContext(void)
 * @brief The JSONContext archetype.
 * @return The JSONContext Class.
 * @memberof JSONContext
 */
OBJECTIVELY_EXPORT Class *_JSONContext(void);
