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

#ifndef _Objectively_Null_h_
#define _Objectively_Null_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief The Null sentinel.
 *
 * Use this Object when you must place `NULL` into collections.
 */

typedef struct Null Null;
typedef struct NullInterface NullInterface;

/**
 * @brief The Null sentinel.
 *
 * Use this Object when you must place `NULL` into collections.
 *
 * @extends Object
 */
struct Null {

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
	NullInterface *interface;
};

/**
 * @brief The Null interface.
 */
struct NullInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The Null singleton.
	 *
	 * @relates Null
	 */
	Null *(*null)(void);
};

/**
 * @brief The Null Class.
 */
extern Class _Null;

#endif
