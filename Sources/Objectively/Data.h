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

#ifndef _Objectively_Data_h_
#define _Objectively_Data_h_

#include <stdio.h>

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief Immutable data buffers.
 */

typedef struct Data Data;
typedef struct DataInterface DataInterface;

/**
 * @brief Immutable data buffers.
 *
 * @extends Object
 */
struct Data {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	DataInterface *interface;

	/**
	 * @brief The bytes.
	 */
	byte *bytes;

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
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Returns a new Data by copying `length` of `bytes`.
	 *
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to copy.
	 *
	 * @return The new Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*dataWithBytes)(const byte *bytes, size_t length);

	/**
	 * @brief Returns a new Data with the contents of `path`.
	 *
	 * @param path The path of the file to read into memory.
	 *
	 * @return The new Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*dataWithContentsOfFile)(const char *path);

	/**
	 * @brief Returns a new Data, taking ownership of the specified memory.
	 *
	 * @param mem The dynamically allocated memory to back this Data.
	 * @param length The length of `mem` in bytes.
	 *
	 * @return The new Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*dataWithMemory)(const ident mem, size_t length);

	/**
	 * @brief Initializes this Data by copying `length` of `bytes`.
	 *
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to copy.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*initWithBytes)(Data *self, const byte *bytes, size_t length);

	/**
	 * @brief Initializes this Data with the contents of `path`.
	 *
	 * @param path The path of the file to read into memory.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*initWithContentsOfFile)(Data *self, const char *path);

	/**
	 * @brief Initializes this Data, taking ownership of the specified memory.
	 *
	 * @param mem The dynamically allocated memory to back this Data.
	 * @param length The length of `mem` in bytes.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*initWithMemory)(Data *self, const ident mem, size_t length);

	/**
	 * @return A MutableData with the contents of this Data.
	 *
	 * @relates Data
	 */
	MutableData *(*mutableCopy)(const Data *self);

	/**
	 * @brief Writes this Data to `path`.
	 *
	 * @param path The path of the file to write.
	 *
	 * @return `true` on success, `false` on error.
	 *
	 * @relates Data
	 */
	_Bool (*writeToFile)(const Data *self, const char *path);
};

/**
 * @brief The Data Class.
 */
extern Class _Data;

#endif
