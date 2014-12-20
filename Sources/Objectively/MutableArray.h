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

#ifndef _Objectively_MutableArray_h_
#define _Objectively_MutableArray_h_

#include <Objectively/Array.h>

/**
 * @file
 *
 * @brief Mutable arrays.
 */

typedef struct MutableArray MutableArray;
typedef struct MutableArrayInterface MutableArrayInterface;

/**
 * @brief Mutable arrays.
 *
 * @extends Array
 *
 * @ingroup Collections
 */
struct MutableArray {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Array array;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	MutableArrayInterface *interface;

	/**
	 * @brief The Array capacity.
	 *
	 * @private
	 */
	size_t capacity;
};

/**
 * @brief The MutableArray interface.
 */
struct MutableArrayInterface {

	/**
	 * @brief The parent interface.
	 */
	ArrayInterface arrayInterface;

	/**
	 * @brief Returns a new MutableArray.
	 *
	 * @return The new MutableArray, or `NULL` on error.
	 *
	 * @relates MutableArray
	 */
	MutableArray *(*array)(void);

	/**
	 * @brief Returns a new MutableArray with the given `capacity`.
	 *
	 * @param capacity The desired initial capacity.
	 *
	 * @return The new MutableArray, or `NULL` on error.
	 *
	 * @relates MutableArray
	 */
	MutableArray *(*arrayWithCapacity)(size_t capacity);

	/**
	 * @brief Adds the specified Object to this MutableArray.
	 *
	 * @relates MutableArray
	 */
	void (*addObject)(MutableArray *self, const id obj);

	/**
	 * @brief Adds the Objects contained in `array` to this MutableArray.
	 *
	 * @param array An Array.
	 */
	void (*addObjectsFromArray)(MutableArray *self, const Array *array);

	/**
	 * @brief Initializes this MutableArray.
	 *
	 * @return The initialized MutableArray, or `NULL` on error.
	 *
	 * @relates MutableArray
	 */
	MutableArray *(*init)(MutableArray *self);

	/**
	 * @brief Initializes this MutableArray with the specified capacity.
	 *
	 * @param capacity The desired initial capacity.
	 *
	 * @return The initialized Array, or `NULL` on error.
	 *
	 * @relates MutableArray
	 */
	MutableArray *(*initWithCapacity)(MutableArray *self, size_t capacity);

	/**
	 * @brief Removes all Objects from this MutableArray.
	 *
	 * @relates MutableArray
	 */
	void (*removeAllObjects)(MutableArray *self);

	/**
	 * @brief Removes the specified Object from this MutableArray.
	 *
	 * @relates MutableArray
	 */
	void (*removeObject)(MutableArray *self, const id obj);

	/**
	 * @brief Removes the Object at the specified index.
	 *
	 * @param index The index of the Object to remove.
	 *
	 * @relates MutableArray
	 */
	void (*removeObjectAtIndex)(MutableArray *self, const int index);

	/**
	 * @brief Replaces the Object at the specified index.
	 *
	 * @param obj The Object with which to replace.
	 * @param index The index of the Object to replace.
	 *
	 * @remark The index must not exceed the size of the MutableArray.
	 *
	 * @relates MutableArray
	 */
	void (*setObjectAtIndex)(MutableArray *self, const id obj, const int index);
};

/**
 * @brief The MutableArray Class.
 */
extern Class _MutableArray;

#endif
