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

#ifndef _Objectively_DateFormatter_h
#define _Objectively_DateFormatter_h

#include <Objectively/Date.h>
#include <Objectively/String.h>

/**
 * @file
 *
 * @brief Date formatting and parsing.
 */

/**
 * @brief ISO8601 date format.
 */
#define DATEFORMAT_ISO8601 "%Y-%m-%dT%H:%M:%S%z"

typedef struct DateFormatter DateFormatter;
typedef struct DateFormatterInterface DateFormatterInterface;

/**
 * @brief The DateFormatter type.
 */
struct DateFormatter {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	DateFormatterInterface *interface;

	/**
	 * @brief The format string.
	 */
	const char *fmt;
};

/**
 * @brief The DateFormatter type.
 */
struct DateFormatterInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * Parses a Date from the specified string.
	 *
	 * @param str The string to parse.
	 *
	 * @return A Date instance, or `NULL` on error.
	 */
	Date *(*dateFromString)(const DateFormatter *self, const char *str);

	/**
	 * Initializes a DateFormatter with the specified format string.
	 *
	 * @param fmt The format string.
	 *
	 * @return The initialized DateFormatter, or `NULL` on error.
	 *
	 * @see strftime
	 */
	DateFormatter *(*initWithFormat)(DateFormatter *self, const char *fmt);

	/**
	 * Yields a String representation of the specified Date instance.
	 *
	 * @param date The Date to format.
	 *
	 * @return The String representation of the Date, or `NULL` on error.
	 */
	String *(*stringFromDate)(const DateFormatter *self, const Date *date);
};

/**
 * @brief The DateFormatter Class.
 */
extern Class __DateFormatter;

#endif
