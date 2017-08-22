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

#include <Objectively/String.h>

/**
 * @file
 * @brief StringReaders provide convenient parsing of text based files.
 */

typedef enum {
	StringReaderPeek,
	StringReaderRead
} StringReaderMode;

typedef struct StringReader StringReader;
typedef struct StringReaderInterface StringReaderInterface;

/**
 * @brief The StringReader type.
 * @extends Object
 */
struct StringReader {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	StringReaderInterface *interface;

	/**
	 * @brief The StringReader head.
	 */
	char *head;

	/**
	 * @brief The String to read.
	 */
	String *string;
};

/**
 * @brief The StringReader interface.
 */
struct StringReaderInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn StringReader *StringReader::initWithCharacters(StringReader *self, const char *chars)
	 * @brief Initializes this StringReader with the specified C string.
	 * @param self The StringReader.
	 * @param chars The null-terminated C string to read.
	 * @return The initialized StringReader, or `NULL` on error.
	 * @memberof StringReader
	 */
	StringReader *(*initWithCharacters)(StringReader *self, const char *chars);

	/**
	 * @fn StringReader *StringReader::initWithString(StringReader *self, String *string)
	 * @brief Initializes this StringReader with the specified String.
	 * @param self The StringReader.
	 * @param string The String to read.
	 * @return The initialized StringReader, or `NULL` on error.
	 * @memberof StringReader
	 */
	StringReader *(*initWithString)(StringReader *self, String *string);

	/**
	 * @fn Unicode StringReader::next(StringReader *self, StringReaderMode mode)
	 * @brief Consumes the next Unicode code point in this StringReader using the given mode.
	 * @param self The StringReader.
	 * @param mode The StringReaderMode.
	 * @return The next Unicode code point, or `-1` if the StringReader is exhausted.
	 * @memberof StringReader
	 */
	Unicode (*next)(StringReader *self, StringReaderMode mode);

	/**
	 * @fn Unicode StringReader::peek(StringReader *self)
	 * @brief Peeks at the next Unicode code point from this StringReader without advancing `head`.
	 * @param self The StringReader.
	 * @return The next Unicode code point, or `-1` if the StringReader is exhausted.
	 * @remarks This method is shorthand for `$(reader, next, StringReaderPeek)`.
	 * @memberof StringReader
	 */
	Unicode (*peek)(StringReader *self);

	/**
	 * @fn Unicode StringReader::read(StringReader *self)
	 * @brief Reads a single Unicode code point from this StringReader.
	 * @param self The StringReader.
	 * @return The Unicode code point, or `-1` if the StringReader is exhausted.
	 * @remarks This method is shorthand for `$(reader, next, StringReaderRead)`.
	 * @memberof StringReader
	 */
	Unicode (*read)(StringReader *self);

	/**
	 * @fn String *StringReader::readToken(StringReader *self, const Unicode *charset, Unocide *stop)
	 * @brief Reads characters from this StringReader until a character in `charset` is encountered.
	 * @param self The StringReader.
	 * @param charset The array of potential `stop` characters.
	 * @param stop Optionally returns the stop character from `charset`.
	 * @return The token accumulated before `stop`, or `NULL` if the StringReader is exhausted.
	 * @memberof StringReader
	 */
	String *(*readToken)(StringReader *self, const Unicode *charset, Unicode *stop);

	/**
	 * @fn void StringReader::reset(StringReader *self)
	 * @brief Resets this StringReader, placing the `head` before the beginning of the String.
	 * @param self The StringReader.
	 * @memberof StringReader
	 */
	void (*reset)(StringReader *self);
};

/**
 * @fn Class *StringReader::_StringReader(void)
 * @brief The StringReader archetype.
 * @return The StringReader Class.
 * @memberof StringReader
 */
OBJECTIVELY_EXPORT Class *_StringReader(void);
