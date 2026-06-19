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
 * @brief Collections of unique index values.
 */

typedef struct IndexSet IndexSet;
typedef struct IndexSetInterface IndexSetInterface;

/**
 * @brief Collections of unique index values.
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

  /**
   * @brief The capacity.
   * @private
   */
  size_t capacity;
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
  /**
   * @fn void IndexSet::addIndex(IndexSet *self, size_t index)
   * @brief Adds the specified index to this IndexSet.
   * @param self The IndexSet.
   * @param index The index to add.
   * @memberof IndexSet
   */
  void (*addIndex)(IndexSet *self, size_t index);

  /**
   * @fn void IndexSet::addIndexes(IndexSet *self, size_t *indexes, size_t count)
   * @brief Adds the specified indexes to this IndexSet.
   * @param self The IndexSet.
   * @param indexes The indexes to add.
   * @param count The count of indexes.
   * @memberof IndexSet
   */
  void (*addIndexes)(IndexSet *self, size_t *indexes, size_t count);

  /**
   * @fn void IndexSet::addIndexesInRange(IndexSet *self, const Range range)
   * @brief Adds indexes in the specified Range to this IndexSet.
   * @param self The IndexSet.
   * @param range The Range of indexes to add.
   * @memberof IndexSet
   */
  void (*addIndexesInRange)(IndexSet *self, const Range range);

  /**
   * @fn IndexSet *IndexSet::init(IndexSet *self)
   * @brief Initializes this IndexSet.
   * @param self The IndexSet.
   * @return The initialized IndexSet, or `NULL` on error.
   * @memberof IndexSet
   */
  IndexSet *(*init)(IndexSet *self);

  /**
   * @fn IndexSet *IndexSet::initWithCapacity(IndexSet *self, size_t capacity)
   * @brief Initializes this IndexSet with the specified capacity.
   * @param self The IndexSet.
   * @param capacity The capacity.
   * @return The initialized IndexSet, or `NULL` on error.
   * @memberof IndexSet
   */
  IndexSet *(*initWithCapacity)(IndexSet *self, size_t capacity);

  /**
   * @fn void IndexSet::removeAllIndexes(IndexSet *self)
   * @brief Removes all indexes from this IndexSet.
   * @param self The IndexSet.
   * @memberof IndexSet
   */
  void (*removeAllIndexes)(IndexSet *self);

  /**
   * @fn void IndexSet::removeIndex(IndexSet *self, size_t index)
   * @brief Removes the specified index from this IndexSet.
   * @param self The IndexSet.
   * @param index The index to remove.
   * @memberof IndexSet
   */
  void (*removeIndex)(IndexSet *self, size_t index);

  /**
   * @fn void IndexSet::removeIndexes(IndexSet *self, size_t *indexes, size_t count)
   * @brief Removes the specified indexes from this IndexSet.
   * @param self The IndexSet.
   * @param indexes The indexes to remove.
   * @param count The count of indexes.
   * @memberof IndexSet
   */
  void (*removeIndexes)(IndexSet *self, size_t *indexes, size_t count);

  /**
  * @fn void IndexSet::removeIndexesInRange(IndexSet *self, const Range range)
  * @brief Removes indexes in the specified Range from this IndexSet.
  * @param self The IndexSet.
  * @param range The Range of indexes to remove.
  * @memberof IndexSet
  */
  void (*removeIndexesInRange)(IndexSet *self, const Range range);
};

/**
 * @fn Class *IndexSet::_IndexSet(void)
 * @brief The IndexSet archetype.
 * @return The IndexSet Class.
 * @memberof IndexSet
 */
OBJECTIVELY_EXPORT Class *_IndexSet(void);

