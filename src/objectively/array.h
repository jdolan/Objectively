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
	 * @brief Adds the specified Object to this Array.
	 */
	void (*addObject)(Array *self, const id obj);

	/**
	 * @return YES if this Array contains the given Object, NO otherwise.
	 */
	BOOL (*containsObject)(const Array *self, const id obj);

	/**
	 * @return The index of the given Object, or -1 if not found.
	 */
	int (*indexOfObject)(const Array *self, const id obj);

	/**
	 * @brief Removes the specified Object from this Array.
	 */
	void (*removeObject)(Array *self, const id obj);

	/**
	 * @brief Removes all Objects from this Array.
	 */
	void (*removeAllObjects)(Array *self);
};

/**
 * @brief The Array Class.
 */
extern const Class *__Array;

#endif
