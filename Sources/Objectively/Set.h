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

#ifndef _Objectively_Set_h_
#define _Objectively_Set_h_

#include <Objectively/Array.h>
#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief Immutable sets.
 */

/**
 * @defgroup Collections Collections
 *
 * @brief Abstract data types for aggregating Objects.
 */

typedef struct Set Set;
typedef struct SetInterface SetInterface;

/**
 * @brief A function pointer for Set enumeration (iteration).
 *
 * @param set The Set.
 * @param obj The Object for the current iteration.
 * @param data User data.
 *
 * @return See the documentation for the enumeration methods.
 */
typedef BOOL (*SetEnumerator)(const Set *set, id obj, id data);

/**
 * @brief Immutable sets.
 *
 * @extends Object
 *
 * @ingroup Collections
 */
struct Set {

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
	SetInterface *interface;

	/**
	 * @brief The internal size (number of bins).
	 *
	 * @private
	 */
	size_t capacity;

	/**
	 * @brief The count of elements.
	 */
	size_t count;

	/**
	 * @brief The elements.
	 *
	 * @private
	 */
	id *elements;
};

/**
 * @brief The Set interface.
 */
struct SetInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return An Array containing all Objects in this Set.
	 *
	 * @relates Set
	 */
	Array *(*allObjects)(const Set *self);

	/**
	 * @return `YES` if this Set contains the given Object, `NO` otherwise.
	 *
	 * @relates Set
	 */
	BOOL (*containsObject)(const Set *self, const id obj);

	/**
	 * @brief Enumerate the elements of this Set with the given function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @remark The enumerator should return `YES` to break the iteration.
	 *
	 * @relates Set
	 */
	void (*enumerateObjects)(const Set *self, SetEnumerator enumerator, id data);

	/**
	 * @brief Creates a new Set with elements that pass the filter function.
	 *
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 *
	 * @return The new, filtered Set.
	 *
	 * @relates Set
	 */
	Set *(*filterObjects)(const Set *self, SetEnumerator enumerator, id data);

	/**
	 * @brief Initializes this Set to contain the Objects in `array`.
	 *
	 * @param array An Array.
	 *
	 * @return The initialized Set, or `NULL` on error.
	 *
	 * @relates Set
	 */
	Set *(*initWithArray)(Set *self, const Array *array);

	/**
	 * @brief Initializes this Set to contain the Objects in `set`.
	 *
	 * @param set A Set.
	 *
	 * @return The initialized Set, or `NULL` on error.
	 *
	 * @relates Set
	 */
	Set *(*initWithSet)(Set *self, const Set *set);

	/**
	 * @brief Initializes this Set with the specified objects.
	 *
	 * @return The initialized Set, or `NULL` on error.
	 *
	 * @relates Set
	 */
	Set *(*initWithObjects)(Set *self, ...);
};

/**
 * @brief The Set Class.
 */
extern Class _Set;

#endif
