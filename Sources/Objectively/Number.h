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

#ifndef _Objectively_Number_h_
#define _Objectively_Number_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief A wrapper for placing numeric primitives into collections, etc.
 */

typedef struct Number Number;
typedef struct NumberInterface NumberInterface;

/**
 * @brief A wrapper for placing numeric primitives into collections, etc.
 *
 * @extends Object
 */
struct Number {

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
	NumberInterface *interface;

	/**
	 * @brief The backing value.
	 */
	double value;
};

/**
 * @brief The Number interface.
 */
struct NumberInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return This Number's BOOL value.
	 *
	 * @relates Number
	 */
	BOOL (*boolValue)(const Number *self);

	/**
	 * @return This Number's char value.
	 *
	 * @relates Number
	 */
	char (*charValue)(const Number *self);

	/**
	 * @brief Compares this Number to another.
	 *
	 * @param other The Number to compare to.
	 *
	 * @return The ordering of this Number compared to `other`.
	 *
	 * @relates Number
	 */
	ORDER (*compareTo)(const Number *self, const Number *other);

	/**
	 * @return This Number's double value.
	 *
	 * @relates Number
	 */
	double (*doubleValue)(const Number *self);

	/**
	 * @return This Number's floating point value.
	 *
	 * @relates Number
	 */
	float (*floatValue)(const Number *self);

	/**
	 * @return This Number's long value.
	 *
	 * @relates Number
	 */
	long (*longValue)(const Number *self);

	/**
	 * @brief Initializes this Number with the specified value.
	 *
	 * @param value The value.
	 *
	 * @return The initialized Number, or `NULL` on error.
	 *
	 * @relates Number
	 */
	Number *(*initWithValue)(Number *self, double value);

	/**
	 * @return This Number's integer value.
	 *
	 * @relates Number
	 */
	int (*intValue)(const Number *self);

	/**
	 * @brief Returns a new Number with the given value.
	 *
	 * @param value The value.
	 *
	 * @return The new Number, or `NULL` on error.
	 *
	 * @relates Number
	 */
	Number *(*numberWithValue)(double value);

	/**
	 * @return This Number's short value.
	 *
	 * @relates Number
	 */
	short (*shortValue)(const Number *self);
};

/**
 * @brief The Number Class.
 */
extern Class _Number;

#endif
