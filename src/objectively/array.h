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

#ifndef _objectively_array_h_
#define _objectively_array_h_

#include <objectively/object.h>

/**
 * @file
 *
 * @brief A mutable array implementation.
 */

typedef struct Array Array;
typedef struct ArrayInterface ArrayInterface;

/**
 * @brief A function pointer for Array enumeration (iteration).
 *
 * @param array The Array.
 * @param obj The Object for the current iteration.
 * @param data User data.
 *
 * @return See the documentation for the enumeration methods.
 */
typedef BOOL (*ArrayEnumerator)(const Array *array, id obj, id data);

/**
 * @brief The Array type.
 */
struct Array {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The Array capacity.
	 */
	size_t capacity;

	/**
	 * @brief The count of elements.
	 */
	size_t count;

	/**
	 * @brief The Array elements.
	 *
	 * @private
	 */
	id *elements;

	/**
	 * @brief The Array initial capacity.
	 */
	size_t initialCapacity;

	/**
	 * @brief The typed interface of Array.
	 */
	const ArrayInterface *interface;
};

/**
 * @brief The Array interface.
 */
struct ArrayInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Adds the specified Object to this Array.
	 */
	void (*addObject)(Array *self, const id obj);

	/**
	 * @return YES if this Array contains the given Object, NO otherwise.
	 */
	BOOL (*containsObject)(const Array *self, const id obj);

	/**
	 * @brief Enumerate the elements of this Array with the given function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @remark The enumerator should return `YES` to break the iteration.
	 */
	void (*enumerateObjects)(const Array *self, ArrayEnumerator enumerator, id data);

	/**
	 * @brief Creates a new Array with elements that pass the filter function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @return The new, filtered Array.
	 */
	Array *(*filterObjects)(const Array *self, ArrayEnumerator enumerator, id data);

	/**
	 * @return The index of the given Object, or -1 if not found.
	 */
	int (*indexOfObject)(const Array *self, const id obj);

	/**
	 * @param index The index of the desired Object.
	 *
	 * @return The Object at the specified index.
	 */
	id (*objectAtIndex)(const Array *self, const int index);

	/**
	 * @brief Removes all Objects from this Array.
	 */
	void (*removeAllObjects)(Array *self);

	/**
	 * @brief Removes the specified Object from this Array.
	 */
	void (*removeObject)(Array *self, const id obj);

	/**
	 * @brief Removes the Object at the specified index.
	 *
	 * @param index The index of the Object to remove.
	 */
	void (*removeObjectAtIndex)(Array *self, const int index);

	/**
	 * @brief Resizes this Array to an appropriate capacity based on count.
	 *
	 * @remark This operation is entirely optional, but can reclaim memory
	 * after large removal operations have executed.
	 */
	void (*resize)(Array *self);

	/**
	 * @brief Replaces the Object at the specified index.
	 *
	 * @param obj The Object with which to replace.
	 * @param index The index of the Object to replace.
	 *
	 * @remark The index must not exceed the size of the Array.
	 */
	void (*setObjectAtIndex)(Array *self, const id obj, const int index);
};

/**
 * @brief The Array Class.
 */
extern Class __Array;

#endif
