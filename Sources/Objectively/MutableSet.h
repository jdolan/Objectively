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

#ifndef _Objectively_MutableSet_h_
#define _Objectively_MutableSet_h_

#include <Objectively/Set.h>

/**
 * @file
 *
 * @brief Mutable sets.
 */

typedef struct MutableSet MutableSet;
typedef struct MutableSetInterface MutableSetInterface;

/**
 * @brief Mutable sets.
 *
 * @extends Set
 *
 * @ingroup Collections
 */
struct MutableSet {

	/**
	 * @brief The parent.
	 */
	Set set;

	/**
	 * @brief The typed interface.
	 */
	MutableSetInterface *interface;
};

/**
 * @brief The MutableSet interface.
 *
 * @extends SetInterface
 *
 * @ingroup Collections
 */
struct MutableSetInterface {

	/**
	 * @brief The parent.
	 */
	SetInterface setInterface;

	/**
	 * @brief Adds the specified Object to this Set.
	 *
	 * @relates MutableSet
	 */
	void (*addObject)(MutableSet *self, const id obj);

	/**
	 * @brief Adds the Objects contained in `array` to this Set.
	 *
	 * @param array An Array.
	 */
	void (*addObjectsFromArray)(MutableSet *self, const Array *array);

	/**
	 * @brief Adds the Objects contained in `set` to this Set.
	 *
	 * @param set An Set.
	 */
	void (*addObjectsFromSet)(MutableSet *self, const Set *set);

	/**
	 * @brief Initializes this MutableSet.
	 *
	 * @return The initialized MutableSet, or `NULL` on error.
	 *
	 * @relates MutableSet
	 */
	MutableSet *(*init)(MutableSet *self);

	/**
	 * @brief Initializes this Set with the specified capacity.
	 *
	 * @param capacity The desired initial capacity.
	 *
	 * @return The initialized Set, or `NULL` on error.
	 *
	 * @relates MutableSet
	 */
	MutableSet *(*initWithCapacity)(MutableSet *self, size_t capacity);

	/**
	 * @brief Removes all Objects from this Set.
	 *
	 * @relates MutableSet
	 */
	void (*removeAllObjects)(MutableSet *self);

	/**
	 * @brief Removes the specified Object from this Set.
	 *
	 * @relates MutableSet
	 */
	void (*removeObject)(MutableSet *self, const id obj);

	/**
	 * @brief Resizes this Set, if necessary, based on its load factor.
	 *
	 * @private
	 *
	 * @relates MutableSet
	 */
	void (*resize)(MutableSet *self);
};

/**
 * @brief The MutableSet Class.
 */
extern Class _MutableSet;

#endif
