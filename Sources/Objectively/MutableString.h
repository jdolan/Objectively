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

#ifndef _Objectively_MutableString_h_
#define _Objectively_MutableString_h_

#include <Objectively/String.h>

/**
 * @file
 *
 * @brief Mutable UTF-8 strings.
 */

typedef struct MutableStringInterface MutableStringInterface;

/**
 * @brief Mutable UTF-8 strings.
 *
 * @extends String
 */
struct MutableString {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	String string;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	MutableStringInterface *interface;

	/**
	 * @brief The capacity of the String, in bytes.
	 *
	 * @remark The capacity is always `>= self->string.length`.
	 *
	 * @private
	 */
	size_t capacity;
};

/**
 * @brief The MutableString interface.
 */
struct MutableStringInterface {

	/**
	 * @brief The parent interface.
	 */
	StringInterface stringInterface;

	/**
	 * @brief Appends the specified UTF-8 encoded C string.
	 *
	 * @param chars A UTF-encoded C string.
	 *
	 * @relates MutableString
	 */
	void (*appendCharacters)(MutableString *self, const char *chars);

	/**
	 * @brief Appends the specified formatted string.
	 *
	 * @param fmt The format sequence.
	 *
	 * @relates MutableString
	 */
	void (*appendFormat)(MutableString *self, const char *fmt, ...);

	/**
	 * @brief Appends the specified String to this MutableString.
	 *
	 * @param other The String to append.
	 *
	 * @relates MutableString
	 */
	void (*appendString)(MutableString *self, const String *other);

	/**
	 * @brief Deletes the characters within `range` from this MutableString.
	 *
	 * @param range The RANGE of characters to delete.
	 *
	 * @relates MutableString
	 */
	void (*deleteCharactersInRange)(MutableString *self, const RANGE range);

	/**
	 * @brief Initializes this MutableString.
	 *
	 * @return The initialized MutableString, or `NULL` on error.
	 *
	 * @relates MutableString
	 */
	MutableString *(*init)(MutableString *self);

	/**
	 * @brief Initializes this MutableString with the given `capacity`.
	 *
	 * @param capacity The capacity, in bytes.
	 *
	 * @return The initialized MutableString, or `NULL` on error.
	 *
	 * @relates MutableString
	 */
	MutableString *(*initWithCapacity)(MutableString *self, size_t capacity);

	/**
	 * @brief Initializes this MutableString with the contents of `string`.
	 *
	 * @param string A String.
	 *
	 * @return The initialized MutableString, or `NULL` on error.
	 *
	 * @relates MutableString
	 */
	MutableString *(*initWithString)(MutableString *self, const String *string);

	/**
	 * @brief Replaces the characters in `range` with the contents of `string`.
	 *
	 * @param range The RANGE of characters to replace.
	 * @param string The String to substitute.
	 *
	 * @relates MutableString
	 */
	void (*replaceCharactersInRange)(MutableString *self, const RANGE range, const String *string);

	/**
	 * @brief Returns a new MutableString.
	 *
	 * @return The new MutableString, or `NULL` on error.
	 *
	 * @relates MutableString
	 */
	MutableString *(*string)(void);

	/**
	 * @brief Returns a new MutableString with the given `capacity`.
	 *
	 * @param capacity The desired capacity, in bytes.
	 *
	 * @return The new MutableString, or `NULL` on error.
	 *
	 * @relates MutableString
	 */
	MutableString *(*stringWithCapacity)(size_t capacity);
};

/**
 * @brief The MutableString Class.
 */
extern Class _MutableString;

#endif
