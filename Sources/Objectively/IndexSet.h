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
 * @brief Index Sets represent the Set to an element or node within a tree or graph structure.
 */

typedef struct IndexSet IndexSet;
typedef struct IndexSetInterface IndexSetInterface;

/**
 * @brief Index Sets represent the Set to an element or node within a tree or graph structure.
 * @extends Object
 */
struct IndexSet {
	
	/**
	 * @brief The parent.
	 */
	Object object;
	
	/**
	 * @brief The typed interface.
	 * @protected
	 */
	IndexSetInterface *interface;

	/**
	 * @brief The indexes.
	 */
	int *indexes;

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
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn _Bool IndexSet::containsIndex(const IndexSet *self, int index)
	 * @param self The IndexSet.
	 * @param index The index.
	 * @return True if this IndexSet contains `index`, false otherwise.
	 * @memberof IndexSet
	 */
	_Bool (*containsIndex)(const IndexSet *self, int index);

	/**
	 * @fn IndexSet *IndexSet::initWithIndex(IndexSet *self, int index)
	 * @brief Initializes this IndexSet with the specified index.
	 * @param self The IndexSet.
	 * @param index The index.
	 * @return The intialized IndexSet, or `NULL` on error.
	 * @memberof IndexSet
	 */
	IndexSet *(*initWithIndex)(IndexSet *self, int index);

	/**
	 * @fn IndexSet *IndexSet::initWithIndexes(IndexSet *self, int *indexes, size_t count)
	 * @brief Initializes this IndexSet with the specified indexes and count.
	 * @param self The IndexSet.
	 * @param indexes The indexes.
	 * @param count The count of `indexes`.
	 * @return The intialized IndexSet, or `NULL` on error.
	 * @memberof IndexSet
	 */
	IndexSet *(*initWithIndexes)(IndexSet *self, int *indexes, size_t count);
};

/**
 * @brief The IndexSet Class.
 */
extern Class _IndexSet;

