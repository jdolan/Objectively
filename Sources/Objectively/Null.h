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
 * @brief The Null sentinel.
 */

typedef struct Null Null;
typedef struct NullInterface NullInterface;

/**
 * @brief The Null sentinel.
 * @details Use this Object when you must place `NULL` into collections.
 * @extends Object
 * @ingroup JSON
 */
struct Null {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  NullInterface *interface;
};

/**
 * @brief The Null interface.
 */
struct NullInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @static
   * @fn Null *Null::null(void)
   * @return The Null singleton.
   * @memberof Null
   */
  Null *(*null)(void);
};

/**
 * @fn Class *Null::_Null(void)
 * @brief The Null archetype.
 * @return The Null Class.
 * @memberof Null
 */
OBJECTIVELY_EXPORT Class *_Null(void);
