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

#ifndef _Objectively_Boolean_h_
#define _Objectively_Boolean_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief A wrapper for placing boolean primitives into collections, etc.
 */

typedef struct Boolean Boolean;
typedef struct BooleanInterface BooleanInterface;

/**
 * @brief A wrapper for placing boolean primitives into collections, etc.
 *
 * @extends Object
 */
struct Boolean {

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
	BooleanInterface *interface;

	/**
	 * @brief The backing _Bool.
	 */
	_Bool value;
};

/**
 * @brief The Boolean interface.
 */
struct BooleanInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The `false` Boolean.
	 *
	 * @relates Boolean
	 */
	Boolean *(*False)(void);

	/**
	 * @return The `true` Boolean.
	 *
	 * @relates Boolean
	 */
	Boolean *(*True)(void);
};

/**
 * @brief The Boolean Class.
 */
extern Class _Boolean;

#endif
