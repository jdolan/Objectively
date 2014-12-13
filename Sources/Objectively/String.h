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
#include <Objectively/Data.h>

/**
 * @file
 *
 * @brief Mutable strings.
 */

typedef struct StringInterface StringInterface;

/**
 * @brief Mutable strings.
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
	char *chars;
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
	 *
	 * @relates String
	 */
	String *(*appendFormat)(String *self, const char *fmt, ...);

	/**
	 * @brief Appends the specified String.
	 *
	 * @param other The String to append.
	 *
	 * @return The resulting String.
	 *
	 * @relates String
	 */
	String *(*appendString)(String *self, const String *other);

	/**
	 * @brief Compares this String lexicographically to another.
	 *
	 * @param other The String to compare to.
	 * @param range The character range to compare.
	 *
	 * @return The ordering of this String compared to `other`.
	 *
	 * @relates String
	 */
	ORDER (*compareTo)(const String *self, const String *other, const RANGE range);

	/**
	 * Returns the components of this String that were separated by `chars`.
	 *
	 * @param chars The separating characters.
	 *
	 * @return An Array of substrings that were separated by `chars`.
	 *
	 * @relates String
	 */
	Array *(*componentsSeparatedByCharacters)(const String *self, const char *chars);

	/**
	 * Returns the components of this String that were separated by `string`.
	 *
	 * @param string The separating string.
	 *
	 * @return An Array of substrings that were separated by `string`.
	 *
	 * @relates String
	 */
	Array *(*componentsSeparatedByString)(const String *self, const String *string);

	/**
	 * @brief Checks this String for the given prefix.
	 *
	 * @param prefix The Prefix to check.
	 *
	 * @return YES if this String starts with prefix, NO otherwise.
	 *
	 * @relates String
	 */
	BOOL (*hasPrefix)(const String *self, const String *prefix);

	/**
	 * @brief Checks this String for the given suffix.
	 *
	 * @param suffix The suffix to check.
	 *
	 * @return YES if this String ends with suffix, NO otherwise.
	 *
	 * @relates String
	 */
	BOOL (*hasSuffix)(const String *self, const String *suffix);

	/**
	 * @brief Initializes this String.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*init)(String *self);

	/**
	 * @brief Initializes this String by copying `length` of `bytes`. The
	 * resulting String will always be null-terminated.
	 *
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to copy.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithBytes)(String *self, const byte *bytes, size_t length);

	/**
	 * @brief Initializes this String with the specified characters.
	 *
	 * @param chars The characters.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithCharacters)(String *self, const char *chars);

	/**
	 * @brief Initializes this String with the contents of `path`.
	 *
	 * @param path The path of the file to load.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithContentsOfFile)(String *self, const char *path);

	/**
	 * @brief Initializes this String with the given Data.
	 *
	 * @param data The Data object.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithData)(String *self, const Data *data);

	/**
	 * @brief Initializes this String with the specified format string.
	 *
	 * @param fmt The formatted string.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithFormat)(String *self, const char *fmt, ...);

	/**
	 * @brief Initializes this String with the specified buffer.
	 *
	 * @param mem The null-terminated, dynamically allocated memory.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithMemory)(String *self, id mem);

	/**
	 * @return A lowercase representation of this String.
	 *
	 * @relates String
	 */
	String *(*lowercaseString)(const String *self);

	/**
	 * Finds and returns the first occurrence of `chars` in this String.
	 *
	 * @param chars The characters to search for.
	 * @param range The range in which to search.
	 *
	 * @return A RANGE specifying the first occurrence of `chars` in this String.
	 *
	 * @relates String
	 */
	RANGE (*rangeOfCharacters)(const String *self, const char *chars, const RANGE range);

	/**
	 * Finds and returns the first occurrence of `string` in this String.
	 *
	 * @param string The String to search for.
	 * @param range The range in which to search.
	 *
	 * @return A RANGE specifying the first occurrence of `string` in this String.
	 *
	 * @relates String
	 */
	RANGE (*rangeOfString)(const String *self, const String *string, RANGE range);

	/**
	 * @brief Creates a new String from a subset of this one.
	 *
	 * @param range The character range.
	 *
	 * @return The new String.
	 *
	 * @relates String
	 */
	String *(*substring)(const String *self, RANGE range);

	/**
	 * @return An uppercase representation of this String.
	 *
	 * @relates String
	 */
	String *(*uppercaseString)(const String *self);

	/**
	 * @brief Writes this String to `path`.
	 *
	 * @param path The path of the file to write.
	 *
	 * @return `YES` on success, `NO` on error.
	 *
	 * @relates String
	 */
	BOOL (*writeToFile)(const String *self, const char *path);
};

/**
 * @brief The String Class.
 */
extern Class __String;

#endif
