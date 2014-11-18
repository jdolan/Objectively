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

#ifndef _Objectively_Date_h
#define _Objectively_Date_h

#include <sys/time.h>

#include <Objectively/Object.h>

/**
 * @brief Microseconds per second.
 */
#define MSEC_PER_SEC 1000000

/**
 * @brief Seconds per day.
 */
#define SEC_PER_DAY (60 * 60 * 24)

typedef struct Date Date;
typedef struct DateInterface DateInterface;

typedef struct timeval Time;

/**
 * @brief The Date type.
 */
struct Date {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The time.
	 */
	Time time;
};

/**
 * @brief The Date type.
 */
struct DateInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Compares this Date to another.
	 *
	 * @param other The Date to compare to.
	 *
	 * @return The ordering of this Date compared to other.
	 */
	ORDER (*compareTo)(const Date *self, const Date *other);

	/**
	 * Initializes a Date with the current time.
	 *
	 * @return The initialized Date, or NULL on error.
	 */
	Date *(*init)(Date *self);

	/**
	 * Initializes a Date by parsing the specified format string.
	 *
	 * @param str The string representation of this Date.
	 * @param fmt The format pattern.
	 *
	 * @return The initialized Date, or NULL on error.
	 */
	Date *(*initWithString)(Date *self, const char *str, const char *fmt);

	/**
	 * Initializes a Date with the specified time.
	 *
	 * @param time The desired Time.
	 *
	 * @return The initialized Date, or NULL on error.
	 */
	Date *(*initWithTime)(Date *self, const Time *time);
};

/**
 * @brief The Date Class.
 */
extern Class __Date;

#endif
