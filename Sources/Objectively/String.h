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

#ifndef _Objectively_String_h_
#define _Objectively_String_h_

#include <ctype.h>
#include <xlocale.h>

#include <Objectively/Array.h>

/**
 * @file
 *
 * @brief A mutable string implementation.
 */

typedef struct StringInterface StringInterface;

/**
 * @brief The String type.
 */
struct String {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	StringInterface *interface;

	/**
	 * @brief The backing character array length.
	 */
	size_t length;

	/**
	 * @brief The locale.
	 */
	locale_t locale;

	/**
	 * @brief The backing character array.
	 */
	char *str;
};

/**
 * @brief The String interface.
 */
struct StringInterface {

	/**
	 * @brief The super Interface.
	 */
	ObjectInterface objectInterface;

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
	 * @return The ordering of this String compared to other.
	 */
	ORDER (*compareTo)(const String *self, const String *other, RANGE range);

	/**
	 * Returns the components of this String that were separated by `chars`.
	 *
	 * @param chars The separating characters.
	 *
	 * @return An Array of substrings that were separated by `chars`.
	 */
	Array *(*componentsSeparatedByCharacters)(const String *self, const char *chars);

	/**
	 * Returns the components of this String that were separated by `string`.
	 *
	 * @param string The separating string.
	 *
	 * @return An Array of substrings that were separated by `string`.
	 */
	Array *(*componentsSeparatedByString)(const String *self, const String *string);

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
	 * @brief Initializes this String.
	 *
	 * @return The initialized String, or `NULL` on error.
	 */
	String *(*init)(String *self);

	/**
	 * @brief Initializes this String with the specified characters.
	 *
	 * @param chars The characters.
	 *
	 * @return The initialized String, or `NULL` on error.
	 */
	String *(*initWithCharacters)(String *self, const char *chars);

	/**
	 * @brief Initializes this String with the specified format string.
	 *
	 * @param self The newly allocated instance.
	 * @param fmt The formatted string.
	 *
	 * @return The initialized String, or `NULL` on error.
	 */
	String *(*initWithFormat)(String *self, const char *fmt, ...);

	/**
	 * @brief Initializes this String with the specified buffer.
	 *
	 * @param mem The dynamically allocated memory.
	 *
	 * @return The initialized String, or `NULL` on error.
	 */
	String *(*initWithMemory)(String *self, id mem);

	/**
	 * @return A lowercase representation of this String.
	 */
	String *(*lowercaseString)(const String *self);

	/**
	 * Finds and returns the first occurrence of `chars` in this String.
	 *
	 * @param chars The characters to search for.
	 * @param range The range in which to search.
	 *
	 * @return A RANGE specifying the first occurrence of `chars` in this String.
	 */
	RANGE (*rangeOfCharacters)(const String *self, const char *chars, const RANGE range);

	/**
	 * Finds and returns the first occurrence of `string` in this String.
	 *
	 * @param string The String to search for.
	 * @param range The range in which to search.
	 *
	 * @return A RANGE specifying the first occurrence of `string` in this String.
	 */
	RANGE (*rangeOfString)(const String *self, const String *string, RANGE range);

	/**
	 * @brief Creates a new String from a subset of this one.
	 *
	 * @param range The character range.
	 *
	 * @return The new String.
	 */
	String *(*substring)(const String *self, RANGE range);

	/**
	 * @return An uppercase representation of this String.
	 */
	String *(*uppercaseString)(const String *self);
};

/**
 * @brief The String Class.
 */
extern Class __String;

#endif
