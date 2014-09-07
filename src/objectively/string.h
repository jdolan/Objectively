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

#ifndef _objectively_string_h_
#define _objectively_string_h_

#include <objectively/object.h>

typedef struct String String;

/**
 * @brief A mutable string implementation.
 */
struct String {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The backing character array.
	 */
	char *str;

	/**
	 * @brief The backing character array length.
	 */
	size_t len;
};

typedef struct StringClass StringClass;

/**
 * @brief The String Class.
 */
struct StringClass {

	/**
	 * @brief The parent.
	 */
	ObjectClass object;

	/**
	 * @brief Appends the specified formatted string.
	 *
	 * @param fmt The format sequence.
	 *
	 * @return The resulting String.
	 */
	String *(*appendFormat)(String *self, const char *fmt, ...);

	/**
	 * @brief Appends the specified String.
	 *
	 * @param other The String to append.
	 *
	 * @return The resulting String.
	 */
	String *(*appendString)(String *self, const String *other);

	/**
	 * @brief Compares this String lexicographically to another.
	 *
	 * @param other The String to compare to.
	 * @param range The character range to compare.
	 *
	 * @return Greater than, equal to, or less than zero.
	 */
	int (*compareTo)(const String *self, const String *other, RANGE range);

	/**
	 * @brief Checks this String for the given prefix.
	 *
	 * @param prefix The Prefix to check.
	 *
	 * @return YES if this String starts with prefix, NO otherwise.
	 */
	BOOL (*hasPrefix)(const String *self, const String *prefix);

	/**
	 * @brief Checks this String for the given suffix.
	 *
	 * @param suffix The suffix to check.
	 *
	 * @return YES if this String ends with suffix, NO otherwise.
	 */
	BOOL (*hasSuffix)(const String *self, const String *suffix);

	/**
	 * @brief Creates a new String from a subset of this one.
	 *
	 * @param range The character range.
	 *
	 * @return The new String.
	 */
	String *(*substring)(const String *self, RANGE range);
};

/**
 * @brief The String Class.
 */
extern StringClass __String;

#endif
