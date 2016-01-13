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

#ifndef _Objectively_String_h_
#define _Objectively_String_h_

#include <ctype.h>
#include <wctype.h>

#include <Objectively/Array.h>
#include <Objectively/Data.h>
#include <Objectively/Locale.h>

/**
 * @file
 *
 * @brief Immutable UTF-8 strings.
 */

/**
 * @brief The Unicode type.
 */
typedef wchar_t Unicode;

/**
 * @brief Character encodings for Strings.
 */
typedef enum {
	STRING_ENCODING_ASCII = 1,
	STRING_ENCODING_LATIN1,
	STRING_ENCODING_LATIN2,
	STRING_ENCODING_MACROMAN,
	STRING_ENCODING_UTF16,
	STRING_ENCODING_UTF32,
	STRING_ENCODING_UTF8,
	STRING_ENCODING_WCHAR,
} StringEncoding;

typedef struct StringInterface StringInterface;

/**
 * @brief Immutable UTF-8 strings.
 *
 * @remark Because Strings are encoded using UTF-8, they must not be treated as
 * ASCII C strings. That is, a single Unicode code point will often span
 * multiple `char`s. Be mindful of this when executing RANGE operations.
 *
 * @extends Object
 */
struct String {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	StringInterface *interface;

	/**
	 * @brief The backing null-terminated UTF-8 encoded character array.
	 */
	char *chars;

	/**
	 * @brief The length of the String in bytes.
	 */
	size_t length;
};

typedef struct MutableString MutableString;

/**
 * @brief The String interface.
 */
struct StringInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

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
	 * @brief Returns the components of this String that were separated by `chars`.
	 *
	 * @param chars The separating characters.
	 *
	 * @return An Array of substrings that were separated by `chars`.
	 *
	 * @relates String
	 */
	Array *(*componentsSeparatedByCharacters)(const String *self, const char *chars);

	/**
	 * @brief Returns the components of this String that were separated by `string`.
	 *
	 * @param string The separating string.
	 *
	 * @return An Array of substrings that were separated by `string`.
	 *
	 * @relates String
	 */
	Array *(*componentsSeparatedByString)(const String *self, const String *string);

	/**
	 * @brief Returns a Data with this String's contents in the given encoding.
	 *
	 * @param encoding The desired StringEncoding.
	 *
	 * @return A Data with this String's contents in the given encoding.
	 */
	Data *(*getData)(const String *self, StringEncoding encoding);

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
	 * @brief Initializes this String by decoding `length` of `bytes`.
	 *
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to decode.
	 * @param encoding The character encoding.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithBytes)(String *self, const byte *bytes, size_t length, StringEncoding encoding);

	/**
	 * @brief Initializes this String by copying `chars`.
	 *
	 * @param chars The null-terminated, UTF-8 encoded C string.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithCharacters)(String *self, const char *chars);

	/**
	 * @brief Initializes this String with the contents of the FILE at `path`.
	 *
	 * @param path The path of the file to load.
	 * @param encoding The character encoding.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithContentsOfFile)(String *self, const char *path, StringEncoding encoding);

	/**
	 * @brief Initializes this String with the given Data.
	 *
	 * @param data The Data object.
	 * @param encoding The character encoding.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithData)(String *self, const Data *data, StringEncoding encoding);

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
	 * @param mem The dynamically allocated null-terminated, UTF-8 encoded buffer.
	 * @param length The length of `mem` in printable characters.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*initWithMemory)(String *self, const id mem, size_t length);

	/**
	 * @brief Initializes this String with the specified arguments list.
	 *
	 * @param fmt The format string.
	 * @param args The format arguments.
	 *
	 * @return The initialized String, or `NULL` on error.
	 *
	 * @relates String
	 *
	 * @see vasprintf
	 */
	String *(*initWithVaList)(String *string, const char *fmt, va_list args);

	/**
	 * @return A lowercase representation of this String in the default Locale.
	 *
	 * @relates String
	 */
	String *(*lowercaseString)(const String *self);

	/**
	 * @param locale The desired Locale.
	 *
	 * @return A lowercase representation of this String in the given Locale.
	 *
	 * @relates String
	 */
	String *(*lowercaseStringWithLocale)(const String *self, const Locale *locale);

	/**
	 * @return A MutableString with the contents of this String.
	 *
	 * @relates String
	 */
	MutableString *(*mutableCopy)(const String *self);

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
	RANGE (*rangeOfString)(const String *self, const String *string, const RANGE range);

	/**
	 * @brief Returns a new String by decoding `length` of `bytes` to UTF-8.
	 *
	 * @param bytes The bytes.
	 * @param length The length of `bytes` to copy.
	 * @param encoding The character encoding.
	 *
	 * @return The new String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*stringWithBytes)(const byte *bytes, size_t length, StringEncoding encoding);

	/**
	 * @brief Returns a new String by copying `chars`.
	 *
	 * @param chars The null-terminated UTF-8 encoded C string.
	 *
	 * @return The new String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*stringWithCharacters)(const char *chars);

	/**
	 * @brief Returns a new String with the contents of the FILE at `path`.
	 *
	 * @param path A path name.
	 * @param encoding The character encoding.
	 *
	 * @return The new String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*stringWithContentsOfFile)(const char *path, StringEncoding encoding);

	/**
	 * @brief Returns a new String with the the given Data.
	 *
	 * @param data A Data.
	 * @param encoding The character encoding.
	 *
	 * @return The new String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*stringWithData)(const Data *data, StringEncoding encoding);

	/**
	 * @brief Returns a new String with the given format string.
	 *
	 * @param fmt The format string.
	 *
	 * @return The new String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*stringWithFormat)(const char *fmt, ...);

	/**
	 * @brief Returns a new String with the given buffer.
	 *
	 * @param mem A dynamically allocated, null-terminated UTF-8 encoded buffer.
	 * @param length The length of `mem` in bytes.
	 *
	 * @return The new String, or `NULL` on error.
	 *
	 * @relates String
	 */
	String *(*stringWithMemory)(const id mem, size_t length);

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
	 * @return An uppercase representation of this String in the default Locale.
	 *
	 * @relates String
	 */
	String *(*uppercaseString)(const String *self);

	/**
	 * @param locale The desired Locale.
	 *
	 * @return A uppercase representation of this String in the given Locale.
	 *
	 * @relates String
	 */
	String *(*uppercaseStringWithLocale)(const String *self, const Locale *locale);

	/**
	 * @brief Writes this String to `path`.
	 *
	 * @param path The path of the file to write.
	 * @param encoding The character encoding.
	 *
	 * @return `YES` on success, `NO` on error.
	 *
	 * @relates String
	 */
	BOOL (*writeToFile)(const String *self, const char *path, StringEncoding encoding);
};

/**
 * @brief The String Class.
 */
extern Class _String;

/**
 * @param encoding A StringEncoding.
 *
 * @return The canonical name for the given encoding.
 */
const char *NameForStringEncoding(StringEncoding encoding);

/**
 * @param name The case-insensitive name of the encoding.
 *
 * @return The StringEncoding for the given `name`.
 */
StringEncoding StringEncodingForName(const char *name);

/**
 * @brief A convenience function for instantiating Strings.
 *
 * @param fmt The format string.
 *
 * @return A new String, or `NULL` on error.
 */
String *str(const char *fmt, ...);

#endif
