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
 * @brief A wrapper for placing numeric primitives into collections, etc.
 */

typedef struct Number Number;
typedef struct NumberInterface NumberInterface;

/**
 * @brief A wrapper for placing numeric primitives into collections, etc.
 * @extends Object
 * @ingroup JSON
 */
struct Number {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
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
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn bool Number::boolValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's bool value.
	 * @memberof Number
	 */
	bool (*boolValue)(const Number *self);

	/**
	 * @fn char Number::charValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's char value.
	 * @memberof Number
	 */
	char (*charValue)(const Number *self);

	/**
	 * @fn Order Number::compareTo(const Number *self, const Number *other)
	 * @brief Compares this Number to another.
	 * @param self The Number.
	 * @param other The Number to compare to.
	 * @return The ordering of this Number compared to `other`.
	 * @memberof Number
	 */
	Order (*compareTo)(const Number *self, const Number *other);

	/**
	 * @fn double Number::doubleValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's double value.
	 * @memberof Number
	 */
	double (*doubleValue)(const Number *self);

	/**
	 * @fn float Number::floatValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's floating point value.
	 * @memberof Number
	 */
	float (*floatValue)(const Number *self);

	/**
	 * @fn long Number::longValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's long value.
	 * @memberof Number
	 */
	long (*longValue)(const Number *self);

	/**
	 * @fn Number *Number::initWithValue(Number *self, double value)
	 * @brief Initializes this Number with the specified value.
	 * @param self The Number.
	 * @param value The value.
	 * @return The initialized Number, or `NULL` on error.
	 * @memberof Number
	 */
	Number *(*initWithValue)(Number *self, double value);

	/**
	 * @fn int Number::intValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's integer value.
	 * @memberof Number
	 */
	int (*intValue)(const Number *self);

	/**
	 * @static
	 * @fn Number *Number::numberWithValue(double value)
	 * @brief Returns a new Number with the given value.
	 * @param value The value.
	 * @return The new Number, or `NULL` on error.
	 * @memberof Number
	 */
	Number *(*numberWithValue)(double value);

	/**
	 * @fn short Number::shortValue(const Number *self)
	 * @param self The Number.
	 * @return This Number's short value.
	 * @memberof Number
	 */
	short (*shortValue)(const Number *self);
};

/**
 * @fn Class *Number::_Number(void)
 * @brief The Number archetype.
 * @return The Number Class.
 * @memberof Number
 */
OBJECTIVELY_EXPORT Class *_Number(void);
