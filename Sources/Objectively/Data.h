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

#ifndef _Objectively_Data_h_
#define _Objectively_Data_h_

#include <stdio.h>

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief Mutable data buffers.
 */

typedef struct Data Data;
typedef struct DataInterface DataInterface;

/**
 * @brief Mutable data buffers.
 *
 * @extends Object
 */
struct Data {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	DataInterface *interface;

	/**
	 * @brief The bytes.
	 */
	byte *bytes;

	/**
	 * @brief The capacity of `bytes`, which is always `>= length`.
	 */
	size_t capacity;

	/**
	 * @brief The length of `bytes`.
	 */
	size_t length;
};

/**
 * @brief The Data interface.
 */
struct DataInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Appends the given bytes to this Data.
	 *
	 * @param bytes The bytes to append.
	 * @param length The length of bytes to append.
	 *
	 * @remark Data are grown in blocks as bytes are appended. This provides
	 * a significant performance gain when frequently appending small chunks
	 * of bytes. Consider using Data when constructing very long Strings, etc.
	 *
	 * @relates Data
	 */
	void (*appendBytes)(Data *self, const byte *bytes, size_t length);

	/**
	 * @brief Initializes this Data with length `0`.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*init)(Data *self);

	/**
	 * @brief Initializes this Data by copying `bytes`.
	 *
	 * @param bytes The bytes.
	 * @param length The length of bytes.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*initWithBytes)(Data *self, const byte *bytes, size_t length);

	/**
	 * @brief Initializes this Data with the given capacity.
	 *
	 * @param capacity The capacity in bytes.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*initWithCapacity)(Data *self, size_t capacity);

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
	 * @param capacity The size of the allocated memory block.
	 * @param length The number of bytes of `mem` that are used.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates Data
	 */
	Data *(*initWithMemory)(Data *self, id mem, size_t capacity, size_t length);

	/**
	 * @brief Writes this Data to `path`.
	 *
	 * @param path The path of the file to write.
	 *
	 * @return `YES` on success, `NO` on error.
	 *
	 * @relates Data
	 */
	BOOL (*writeToFile)(const Data *self, const char *path);
};

/**
 * @brief The Data Class.
 */
extern Class __Data;

#endif
