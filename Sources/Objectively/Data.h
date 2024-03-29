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

#include <Objectively/Object.h>

/**
 * @file
 * @brief Immutable data buffers.
 */

/**
 * @defgroup ByteStreams Byte Streams
 * @brief Mutable and immutable byte streams.
 */
typedef struct Data Data;
typedef struct DataInterface DataInterface;

/**
 * @brief Data may optionally reference destructor to be called on `dealloc`.
 */
typedef void (*DataDestructor)(ident mem);

/**
 * @brief Immutable data buffers.
 * @extends Object
 * @ingroup ByteStreams
 */
struct Data {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	DataInterface *interface;

	/**
	 * @brief The bytes.
	 */
	uint8_t *bytes;

	/**
	 * @brief An optional destructor that, if set, is called on `dealloc`.
	 */
	DataDestructor destroy;

	/**
	 * @brief The length of `bytes`.
	 */
	size_t length;
};

typedef struct MutableData MutableData;

/**
 * @brief The Data interface.
 */
struct DataInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @static
	 * @fn Data *Data::dataWithBytes(const uint8_t *bytes, size_t length)
	 * @brief Returns a new Data by copying `length` of `bytes`.
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to copy.
	 * @return The new Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*dataWithBytes)(const uint8_t *bytes, size_t length);

	/**
	 * @static
	 * @fn Data *Data::dataWithConstMemory(const ident mem, size_t length)
	 * @brief Returns a new Data, backed by the given const memory.
	 * @param mem The constant memory to back this Data.
	 * @param length The length of `mem` in bytes.
	 * @return The new Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*dataWithConstMemory)(const ident mem, size_t length);

	/**
	 * @static
	 * @fn Data *Data::dataWithContentsOfFile(const char *path)
	 * @brief Returns a new Data with the contents of the file at `path`.
	 * @param path The path of the file to read into memory.
	 * @return The new Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*dataWithContentsOfFile)(const char *path);

	/**
	 * @static
	 * @fn Data *Data::dataWithMemory(ident mem, size_t length)
	 * @brief Returns a new Data, taking ownership of the specified memory.
	 * @param mem The dynamically allocated memory to back this Data.
	 * @param length The length of `mem` in bytes.
	 * @return The new Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*dataWithMemory)(ident mem, size_t length);

	/**
	 * @fn Data *Data::initWithBytes(Data *self, const uint8_t *bytes, size_t length)
	 * @brief Initializes this Data by copying `length` of `bytes`.
	 * @param self The Data.
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to copy.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*initWithBytes)(Data *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn Data *Data::initWithConstMemory(Data *self, const ident mem, size_t length)
	 * @brief Initializes this Data with the given const memory.
	 * @param self The Data.
	 * @param mem The const memory to back this Data.
	 * @param length The length of `mem` in bytes.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*initWithConstMemory)(Data *self, const ident mem, size_t length);

	/**
	 * @fn Data *Data::initWithContentsOfFile(Data *self, const char *path)
	 * @brief Initializes this Data with the contents of the file at `path`.
	 * @param self The Data.
	 * @param path The path of the file to read into memory.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*initWithContentsOfFile)(Data *self, const char *path);

	/**
	 * @fn Data *Data::initWithMemory(Data *self, ident mem, size_t length)
	 * @brief Initializes this Data, taking ownership of the specified memory.
	 * @param self The Data.
	 * @param mem The dynamically allocated memory to back this Data.
	 * @param length The length of `mem` in bytes.
	 * @return The initialized Data, or `NULL` on error.
	 * @memberof Data
	 */
	Data *(*initWithMemory)(Data *self, ident mem, size_t length);

	/**
	 * @fn MutableData *Data::mutableCopy(const Data *self)
	 * @param self The Data.
	 * @return A MutableData with the contents of this Data.
	 * @memberof Data
	 */
	MutableData *(*mutableCopy)(const Data *self);

	/**
	 * @fn bool Data::writeToFile(const Data *self, const char *path)
	 * @brief Writes this Data to `path`.
	 * @param self The Data.
	 * @param path The path of the file to write.
	 * @return `true` on success, `false` on error.
	 * @memberof Data
	 */
	bool (*writeToFile)(const Data *self, const char *path);
};

/**
 * @fn Class *Data::_Data(void)
 * @brief The Data archetype.
 * @return The Data Class.
 * @memberof Data
 */
OBJECTIVELY_EXPORT Class *_Data(void);
