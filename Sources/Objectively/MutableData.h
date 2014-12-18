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

#ifndef _Objectively_MutableData_h_
#define _Objectively_MutableData_h_

#include <Objectively/Data.h>

/**
 * @file
 *
 * @brief Mutable data buffers.
 */

typedef struct MutableData MutableData;
typedef struct MutableDataInterface MutableDataInterface;

/**
 * @brief Mutable data buffers.
 *
 * @extends Object
 */
struct MutableData {

	/**
	 * @brief The parent.
	 */
	Data data;

	/**
	 * @brief The typed interface.
	 */
	MutableDataInterface *interface;

	/**
	 * @brief The capacity of `bytes`, which is always `>= length`.
	 *
	 * @private
	 */
	size_t capacity;
};

/**
 * @brief The MutableData interface.
 */
struct MutableDataInterface {

	/**
	 * @brief The parent.
	 */
	DataInterface dataInterface;

	/**
	 * @brief Appends the given `bytes` to this Data.
	 *
	 * @param bytes The bytes to append.
	 * @param length The length of bytes to append.
	 *
	 * @remark MutableData are grown in blocks as bytes are appended. This
	 * provides a significant performance gain when frequently appending small
	 * chunks of bytes.
	 *
	 * @relates MutableData
	 */
	void (*appendBytes)(MutableData *self, const byte *bytes, size_t length);

	/**
	 * @brief Appends the given `data` to this Data.
	 *
	 * @param data The Data to append.
	 *
	 * @relates MutableData
	 */
	void (*appendData)(MutableData *self, const Data *data);

	/**
	 * @brief Initializes this Data with length `0`.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates MutableData
	 */
	MutableData *(*init)(MutableData *self);

	/**
	 * @brief Initializes this Data with the given capacity.
	 *
	 * @param capacity The capacity in bytes.
	 *
	 * @return The initialized Data, or `NULL` on error.
	 *
	 * @relates MutableData
	 */
	MutableData *(*initWithCapacity)(MutableData *self, size_t capacity);

	/**
	 * @brief Sets the length of this Data, truncating or expanding it.
	 *
	 * @param length The new desired length.
	 *
	 * @remark If the data is expanded, the newly allocated bytes are filled
	 * with zeros.
	 *
	 * @relates MutableData
	 */
	void (*setLength)(MutableData *self, size_t length);
};

/**
 * @brief The MutableData Class.
 */
extern Class _MutableData;

#endif
