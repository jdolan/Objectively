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

#include <Objectively/Set.h>

/**
 * @file
 * @brief Mutable sets.
 */

typedef struct MutableSetInterface MutableSetInterface;

/**
 * @brief Mutable sets.
 * @extends Set
 * @ingroup Collections
 */
struct MutableSet {

  /**
   * @brief The superclass.
   */
  Set set;

  /**
   * @brief The interface.
   * @protected
   */
  MutableSetInterface *interface;
};

/**
 * @brief The MutableSet interface.
 */
struct MutableSetInterface {

  /**
   * @brief The superclass interface.
   */
  SetInterface setInterface;

  /**
   * @fn void MutableSet::addObject(MutableSet *self, const ident obj)
   * @brief Adds the specified Object to this Set.
   * @param self The MutableSet.
   * @param obj An Object.
   * @memberof MutableSet
   */
  void (*addObject)(MutableSet *self, const ident obj);

  /**
   * @fn void MutableSet::addObjectsFromArray(MutableSet *self, const Array *array)
   * @brief Adds the Objects contained in `array` to this Set.
   * @param self The MutableSet.
   * @param array An Array.
   * @memberof MutableSet
   */
  void (*addObjectsFromArray)(MutableSet *self, const Array *array);

  /**
   * @fn void MutableSet::addObjectsFromSet(MutableSet *self, const Set *set)
   * @brief Adds the Objects contained in `set` to this Set.
   * @param self The MutableSet.
   * @param set A Set.
   * @memberof MutableSet
   */
  void (*addObjectsFromSet)(MutableSet *self, const Set *set);

  /**
   * @fn void MutableSet::filter(MutableSet *self, Predicate predicate, ident data)
   * @brief Filters this MutableSet in place using `predicate`.
   * @param self The MutableSet.
   * @param predicate A Predicate.
   * @param data User data.
   * @memberof MutableSet
   */
  void (*filter)(MutableSet *self, Predicate predicate, ident data);

  /**
   * @fn MutableSet *MutableSet::init(MutableSet *self)
   * @brief Initializes this MutableSet.
   * @param self The MutableSet.
   * @return The initialized MutableSet, or `NULL` on error.
   * @memberof MutableSet
   */
  MutableSet *(*init)(MutableSet *self);

  /**
   * @fn MutableSet *MutableSet::initWithCapacity(MutableSet *self, size_t capacity)
   * @brief Initializes this Set with the specified capacity.
   * @param self The MutableSet.
   * @param capacity The desired initial capacity.
   * @return The initialized Set, or `NULL` on error.
   * @memberof MutableSet
   */
  MutableSet *(*initWithCapacity)(MutableSet *self, size_t capacity);

  /**
   * @fn void MutableSet::removeAllObjects(MutableSet *self)
   * @brief Removes all Objects from this Set.
   * @param self The MutableSet.
   * @memberof MutableSet
   */
  void (*removeAllObjects)(MutableSet *self);

  /**
   * @fn void MutableSet::removeObject(MutableSet *self, const ident obj)
   * @brief Removes the specified Object from this Set.
   * @param self The MutableSet.
   * @param obj The Object to remove.
   * @memberof MutableSet
   */
  void (*removeObject)(MutableSet *self, const ident obj);

  /**
   * @static
   * @fn MutableSet *MutableSet::set(void)
   * @brief Returns a new MutableSet.
   * @return The new MutableSet, or `NULL` on error.
   * @memberof MutableSet
   */
  MutableSet *(*set)(void);

  /**
   * @static
   * @fn MutableSet *MutableSet::setWithCapacity(size_t capacity)
   * @brief Returns a new MutableSet with the given `capacity`.
   * @param capacity The desired initial capacity.
   * @return The new MutableSet, or `NULL` on error.
   * @memberof MutableSet
   */
  MutableSet *(*setWithCapacity)(size_t capacity);
};

/**
 * @fn Class *MutableSet::_MutableSet(void)
 * @brief The MutableSet archetype.
 * @return The MutableSet Class.
 * @memberof MutableSet
 */
OBJECTIVELY_EXPORT Class *_MutableSet(void);
