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

#ifndef _Objectively_Number_h
#define _Objectively_Number_h

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief A wrapper for placing numeric primitives into collections, etc.
 */

typedef struct Number Number;
typedef struct NumberInterface NumberInterface;

/**
 * @brief The Number type.
 */
struct Number {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	NumberInterface *interface;

	/**
	 * @brief The backing value.
	 */
	double value;
};

/**
 * @brief The Number type.
 */
struct NumberInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return This Number's BOOL value.
	 */
	BOOL (*boolValue)(const Number *self);

	/**
	 * @return This Number's char value.
	 */
	char (*charValue)(const Number *self);

	/**
	 * @brief Compares this Number to another.
	 *
	 * @param other The Number to compare to.
	 *
	 * @return The ordering of this Number compared to `other`.
	 */
	ORDER (*compareTo)(const Number *self, const Number *other);

	/**
	 * @return This Number's double value.
	 */
	double (*doubleValue)(const Number *self);

	/**
	 * @return This Number's floating point value.
	 */
	float (*floatValue)(const Number *self);

	/**
	 * @return This Number's long value.
	 */
	long (*longValue)(const Number *self);

	/**
	 * @brief Initializes this Number with the specified value.
	 *
	 * @param value The value.
	 *
	 * @return The initialized Number, or `NULL` on error.
	 */
	Number *(*initWithValue)(Number *self, double value);

	/**
	 * @return This Number's integer value.
	 */
	int (*intValue)(const Number *self);

	/**
	 * @return This Number's short value.
	 */
	short (*shortValue)(const Number *self);
};

/**
 * @brief The Number Class.
 */
extern Class __Number;

#endif
