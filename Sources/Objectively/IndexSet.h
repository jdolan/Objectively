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
 * @brief Immutable collections of unique index values.
 */

typedef struct IndexSet IndexSet;
typedef struct IndexSetInterface IndexSetInterface;

/**
 * @brief Immutable collections of unique index values.
 * @extends Object
 * @ingroup Collections
 */
struct IndexSet {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	IndexSetInterface *interface;

	/**
	 * @brief The indexes.
	 */
	size_t *indexes;

	/**
	 * @brief The count of `indexes`.
	 */
	size_t count;
};

/**
 * @brief The IndexSet interface.
 */
struct IndexSetInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn bool IndexSet::containsIndex(const IndexSet *self, size_t index)
	 * @param self The IndexSet.
	 * @param index The index.
	 * @return True if this IndexSet contains `index`, false otherwise.
	 * @memberof IndexSet
	 */
	bool (*containsIndex)(const IndexSet *self, size_t index);

	/**
	 * @fn IndexSet *IndexSet::initWithIndex(IndexSet *self, size_t index)
	 * @brief Initializes this IndexSet with the specified index.
	 * @param self The IndexSet.
	 * @param index The index.
	 * @return The intialized IndexSet, or `NULL` on error.
	 * @memberof IndexSet
	 */
	IndexSet *(*initWithIndex)(IndexSet *self, size_t index);

	/**
	 * @fn IndexSet *IndexSet::initWithIndexes(IndexSet *self, size_t *indexes, size_t count)
	 * @brief Initializes this IndexSet with the specified indexes and count.
	 * @param self The IndexSet.
	 * @param indexes The indexes.
	 * @param count The count of `indexes`.
	 * @return The intialized IndexSet, or `NULL` on error.
	 * @memberof IndexSet
	 */
	IndexSet *(*initWithIndexes)(IndexSet *self, size_t *indexes, size_t count);
};

/**
 * @fn Class *IndexSet::_IndexSet(void)
 * @brief The IndexSet archetype.
 * @return The IndexSet Class.
 * @memberof IndexSet
 */
OBJECTIVELY_EXPORT Class *_IndexSet(void);

