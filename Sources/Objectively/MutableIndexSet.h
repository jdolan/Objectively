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

#include <Objectively/IndexSet.h>

/**
 * @file
 * @brief Mutable collections of unique index values.
 */

typedef struct MutableIndexSet MutableIndexSet;
typedef struct MutableIndexSetInterface MutableIndexSetInterface;

/**
 * @brief Mutable collections of unique index values.
 * @extends IndexSet
 * @ingroup Collections
 */
struct MutableIndexSet {

	/**
	 * @brief The superclass.
	 */
	IndexSet indexSet;

	/**
	 * @brief The interface.
	 * @protected
	 */
	MutableIndexSetInterface *interface;

	/**
	 * @brief The capacity.
	 */
	size_t capacity;
};

/**
 * @brief The MutableIndexSet interface.
 */
struct MutableIndexSetInterface {

	/**
	 * @brief The superclass interface.
	 */
	IndexSetInterface indexSetInterface;

	/**
	 * @fn void MutableIndexSet::addIndex(MutableIndexSet *self, size_t index)
	 * @brief Adds the specified index to this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @param index The index to add.
	 * @memberof MutableIndexSet
	 */
	void (*addIndex)(MutableIndexSet *self, size_t index);

	/**
	 * @fn void MutableIndexSet::addIndexes(MutableIndexSet *self, size_t *indexes, size_t count)
	 * @brief Adds the specified indexes to this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @param indexes The indexes to add.
	 * @param count The count of indexes.
	 * @memberof MutableIndexSet
	 */
	void (*addIndexes)(MutableIndexSet *self, size_t *indexes, size_t count);

	/**
	 * @fn void MutableIndexSet::addIndexesInRange(MutableIndexSet *self, const Range range)
	 * @brief Adds indexes in the specified Range to this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @param range The Range of indexes to add.
	 * @memberof MutableIndexSet
	 */
	void (*addIndexesInRange)(MutableIndexSet *self, const Range range);

	/**
	 * @fn MutableIndexSet *MutableIndexSet::init(MutableIndexSet *self)
	 * @brief Initializes this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @return The initialized MutableIndexSet, or `NULL` on error.
	 * @memberof MutableIndexSet
	 */
	MutableIndexSet *(*init)(MutableIndexSet *self);

	/**
	 * @fn MutableIndexSet *MutableIndexSet::initWithCapacity(MutableIndexSet *self, size_t capacity)
	 * @brief Initializes this MutableIndexSet with the specified capacity.
	 * @param self The MutableIndexSet.
	 * @param capacity The capacity.
	 * @return The initialized MutableIndexSet, or `NULL` on error.
	 * @memberof MutableIndexSet
	 */
	MutableIndexSet *(*initWithCapacity)(MutableIndexSet *self, size_t capacity);

	/**
	 * @fn void MutableIndexSet::removeAllIndexes(MutableIndexSet *self)
	 * @brief Removes all indexes from this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @memberof MutableIndexSet
	 */
	void (*removeAllIndexes)(MutableIndexSet *self);

	/**
	 * @fn void MutableIndexSet::removeIndex(MutableIndexSet *self, size_t index)
	 * @brief Removes the specified index from this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @param index The index to remove.
	 * @memberof MutableIndexSet
	 */
	void (*removeIndex)(MutableIndexSet *self, size_t index);

	/**
	 * @fn void MutableIndexSet::removeIndexes(MutableIndexSet *self, size_t *indexes, size_t count)
	 * @brief Removes the specified indexes from this MutableIndexSet.
	 * @param self The MutableIndexSet.
	 * @param indexes The indexes to remove.
	 * @param count The count of indexes.
	 * @memberof MutableIndexSet
	 */
	void (*removeIndexes)(MutableIndexSet *self, size_t *indexes, size_t count);

	/**
	* @fn void MutableIndexSet::removeIndexesInRange(MutableIndexSet *self, const Range range)
	* @brief Removes indexes in the specified Range from this MutableIndexSet.
	* @param self The MutableIndexSet.
	* @param range The Range of indexes to remove.
	* @memberof MutableIndexSet
	*/
	void (*removeIndexesInRange)(MutableIndexSet *self, const Range range);
};

/**
 * @fn Class *MutableIndexSet::_MutableIndexSet(void)
 * @brief The MutableIndexSet archetype.
 * @return The MutableIndexSet Class.
 * @memberof MutableIndexSet
 */
OBJECTIVELY_EXPORT Class *_MutableIndexSet(void);
