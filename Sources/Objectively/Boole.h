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

#ifndef _Objectively_Boole_h_
#define _Objectively_Boole_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief A wrapper for placing boolean primitives into collections, etc.
 */

typedef struct Boole Boole;
typedef struct BooleInterface BooleInterface;

/**
 * @brief A wrapper for placing boolean primitives into collections, etc.
 *
 * @extends Object
 */
struct Boole {

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
	BooleInterface *interface;

	/**
	 * @brief The backing _Bool.
	 */
	_Bool value;
};

/**
 * @brief The Boole interface.
 */
struct BooleInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The `false` Boole.
	 *
	 * @relates Boole
	 */
	Boole *(*False)(void);

	/**
	 * @return The `true` Boole.
	 *
	 * @relates Boole
	 */
	Boole *(*True)(void);
};

/**
 * @brief The Boole Class.
 */
extern Class _Boole;

#endif
