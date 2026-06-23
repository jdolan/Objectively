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
 * @brief Pointers provide Object encapsulation for raw C pointers.
 */

typedef struct Pointer Pointer;
typedef struct PointerInterface PointerInterface;

/**
 * @brief Pointers provide Object encapsulation for raw C pointers.
 * @extends Object
 */
struct Pointer {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  PointerInterface *interface;

  /**
   * @brief An optional destructor that, if set, is called on `dealloc`.
   */
  Consumer destroy;

  /**
   * @brief The backing pointer.
   */
  ident pointer;
};

/**
 * @brief The Pointer interface.
 */
struct PointerInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn Pointer *Pointer::initWithBytes(Pointer *self, const uint8_t *bytes, size_t length)
   * @brief Initializes this Pointer by copying `length` of `bytes`.
   * @param self The Pointer.
   * @param bytes The bytes to copy.
   * @param length The count of `bytes` to copy.
   * @return The initialized Pointer, or `NULL` on error.
   * @remarks The copied bytes will be freed on `dealloc`.
   * @memberof Pointer
   */
  Pointer *(*initWithBytes)(Pointer *self, const uint8_t *bytes, size_t length);

  /**
   * @fn Pointer *Pointer::initWithPointer(Pointer *self, ident pointer, Consumer destroy)
   * @brief Initializes this Pointer.
   * @param self The Pointer.
   * @param pointer The backing pointer.
   * @param destroy An optional destructor, or `NULL`.
   * @return The initialized Pointer, or `NULL` on error.
   * @memberof Pointer
   */
  Pointer *(*initWithPointer)(Pointer *self, ident pointer, Consumer destroy);
};

/**
 * @fn Class *Pointer::_Pointer(void)
 * @brief The Pointer archetype.
 * @return The Pointer Class.
 * @memberof Pointer
 */
OBJECTIVELY_EXPORT Class *_Pointer(void);

/**
 * @brief A convenience function for instantiating Pointers.
 * @param pointer The backing pointer.
 * @param destroy An optional destructor, or `NULL`.
 * @return A new Pointer, or `NULL` on error.
 * @relates Pointer
 */
OBJECTIVELY_EXPORT Pointer *ptr(ident pointer, Consumer destroy);
