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
 * @brief Growable arrays of raw C pointers.
 */

typedef struct PointerArray PointerArray;
typedef struct PointerArrayInterface PointerArrayInterface;

/**
 * @brief Growable arrays of raw C pointers.
 * @details Like `Vector`, but specialized for pointer-sized elements. The
 * `destroy` callback receives the stored pointer value itself (i.e.
 * `elements[i]`), not a pointer into the backing buffer.
 * @extends Object
 * @ingroup Collections
 */
struct PointerArray {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  PointerArrayInterface *interface;

  /**
   * @brief The capacity.
   */
  size_t capacity;

  /**
   * @brief The count of elements.
   */
  size_t count;

  /**
   * @brief Optional destructor called when a element is removed.
   * @details The argument is the pointer value itself, not a pointer into the
   * backing buffer.
   */
  Consumer destroy;

  /**
   * @brief The backing array of pointers.
   */
  ident *elements;
};

/**
 * @brief The PointerArray interface.
 */
struct PointerArrayInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn void PointerArray::add(PointerArray *self, ident pointer)
   * @brief Appends the given pointer to this PointerArray.
   * @param self The PointerArray.
   * @param pointer The pointer to add.
   * @memberof PointerArray
   */
  void (*add)(PointerArray *self, ident pointer);

  /**
   * @fn ident PointerArray::get(const PointerArray *self, size_t index)
   * @param self The PointerArray.
   * @param index The index.
   * @return The element at the given index.
   * @memberof PointerArray
   */
  ident (*get)(const PointerArray *self, size_t index);

  /**
   * @fn PointerArray *PointerArray::init(PointerArray *self)
   * @brief Initializes this PointerArray.
   * @param self The PointerArray.
   * @return The initialized PointerArray, or `NULL` on error.
   * @memberof PointerArray
   */
  PointerArray *(*init)(PointerArray *self);

  /**
   * @fn PointerArray *PointerArray::initWithDestroy(PointerArray *self, Consumer destroy)
   * @brief Initializes this PointerArray with a destructor.
   * @param self The PointerArray.
   * @param destroy An optional destructor called when a pointer is removed, or `NULL`.
   * @return The initialized PointerArray, or `NULL` on error.
   * @memberof PointerArray
   */
  PointerArray *(*initWithDestroy)(PointerArray *self, Consumer destroy);

  /**
   * @fn void PointerArray::remove(PointerArray *self, ident pointer)
   * @brief Removes the first occurrence of `pointer` from this PointerArray.
   * @param self The PointerArray.
   * @param pointer The element to remove (compared by value).
   * @memberof PointerArray
   */
  void (*remove)(PointerArray *self, ident pointer);

  /**
   * @fn void PointerArray::removeAt(PointerArray *self, size_t index)
   * @brief Removes the pointer at the specified index.
   * @param self The PointerArray.
   * @param index The index of the element to remove.
   * @memberof PointerArray
   */
  void (*removeAt)(PointerArray *self, size_t index);

  /**
   * @fn void PointerArray::removeAll(PointerArray *self)
   * @brief Removes all elements from this PointerArray without modifying its capacity.
   * @param self The PointerArray.
   * @memberof PointerArray
   */
  void (*removeAll)(PointerArray *self);

  /**
   * @fn void PointerArray::sort(PointerArray *self, Comparator comparator)
   * @brief Sorts this PointerArray in place using `comparator`.
   * @param self The PointerArray.
   * @param comparator A Comparator.
   * @memberof PointerArray
   */
  void (*sort)(PointerArray *self, Comparator comparator);
};

/**
 * @fn Class *PointerArray::_PointerArray(void)
 * @brief The PointerArray archetype.
 * @return The PointerArray Class.
 * @memberof PointerArray
 */
OBJECTIVELY_EXPORT Class *_PointerArray(void);
