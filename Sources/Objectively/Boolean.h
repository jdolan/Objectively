/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
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
 * @brief The Boolean type.
 */
struct Boolean {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	BooleanInterface *interface;

	/**
	 * @brief The backing BOOL.
	 */
	BOOL bool;
};

/**
 * @brief The Boolean interface.
 */
struct BooleanInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The `NO` Boolean.
	 *
	 * @relates Boolean
	 */
	Boolean *(*no)(void);

	/**
	 * @return The `YES` Boolean.
	 *
	 * @relates Boolean
	 */
	Boolean *(*yes)(void);
};

/**
 * @brief The Boolean Class.
 */
extern Class __Boolean;

#endif
