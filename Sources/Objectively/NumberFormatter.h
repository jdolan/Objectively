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

#include <Objectively/Number.h>
#include <Objectively/String.h>

/**
 * @file
 * @brief Number formatting and parsing.
 */

/**
 * @brief Currency format.
 */
#define NUMBERFORMAT_CURRENCY "%'.2lf"

/**
 * @brief Decimal format.
 */
#define NUMBERFORMAT_DECIMAL "%lf"

/**
 * @brief Integer format.
 */
#define NUMBERFORMAT_INTEGER "%ld"

typedef struct NumberFormatter NumberFormatter;
typedef struct NumberFormatterInterface NumberFormatterInterface;

/**
 * @brief Number formatting and parsing.
 * @extends Object
 * @ingroup Localization
 */
struct NumberFormatter {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 * @protected
	 */
	NumberFormatterInterface *interface;

	/**
	 * @brief The format string.
	 */
	const char *fmt;
};

/**
 * @brief The NumberFormatter interface.
 */
struct NumberFormatterInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn NumberFormatter *NumberFormatter::initWithFormat(NumberFormatter *self, const char *fmt)
	 * @brief Initializes a NumberFormatter with the specified format string.
	 * @param self The NumberFormatter.
	 * @param fmt The format string.
	 * @return The initialized NumberFormatter, or `NULL` on error.
	 * @see strftime(3)
	 * @memberof NumberFormatter
	 */
	NumberFormatter *(*initWithFormat)(NumberFormatter *self, const char *fmt);

	/**
	 * @fn Number *NumberFormatter::numberFromString(const NumberFormatter *self, const String *string)
	 * @brief Parses a Number from the specified String.
	 * @param self The NumberFormatter.
	 * @param string The String to parse.
	 * @return A Number instance, or `NULL` on error.
	 * @memberof NumberFormatter
	 */
	Number *(*numberFromString)(const NumberFormatter *self, const String *string);
	
	/**
	 * @fn String *NumberFormatter::stringFromNumber(const NumberFormatter *self, const Number *number)
	 * @brief Yields a String representation of the specified Number instance.
	 * @param self The NumberFormatter.
	 * @param number The Number to format.
	 * @return The String representation of the Number, or `NULL` on error.
	 * @memberof NumberFormatter
	 */
	String *(*stringFromNumber)(const NumberFormatter *self, const Number *number);
};

/**
 * @brief The NumberFormatter Class.
 */
extern Class _NumberFormatter;
