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
 * @brief Mutable UTF-8 strings.
 */

typedef struct MutableStringInterface MutableStringInterface;

/**
 * @brief Mutable UTF-8 strings.
 * @extends String
 * @ingroup ByteStreams
 */
struct MutableString {

  /**
   * @brief The superclass.
   */
  String string;

  /**
   * @brief The interface.
   * @protected
   */
  MutableStringInterface *interface;

  /**
   * @brief The capacity of the String, in bytes.
   * @remarks The capacity is always `>= self->string.length`.
   * @private
   */
  size_t capacity;
};

/**
 * @brief The MutableString interface.
 */
struct MutableStringInterface {

  /**
   * @brief The superclass interface.
   */
  StringInterface stringInterface;

  /**
   * @fn void MutableString::appendCharacters(MutableString *self, const char *chars)
   * @brief Appends the specified UTF-8 encoded C string.
   * @param self The MutableString.
   * @param chars A UTF-encoded C string.
   * @memberof MutableString
   */
  void (*appendCharacters)(MutableString *self, const char *chars);

  /**
   * @fn void MutableString::appendFormat(MutableString *self, const char *fmt, ...)
   * @brief Appends the specified formatted string.
   * @param self The MutableString.
   * @param fmt The format string.
   * @memberof MutableString
   */
  void (*appendFormat)(MutableString *self, const char *fmt, ...);

  /**
   * @fn void MutableString::appendString(MutableString *self, const String *string)
   * @brief Appends the specified String to this MutableString.
   * @param self The MutableString.
   * @param string The String to append.
   * @memberof MutableString
   */
  void (*appendString)(MutableString *self, const String *string);

  /**
   * @fn void MutableString::appendVaList(MutableString *self, const char *fmt, va_list args)
   * @brief Appends the specified format string.
   * @param self The MutableString.
   * @param fmt The format string.
   * @param args The format arguments.
   * @memberof MutableString
   */
  void (*appendVaList)(MutableString *self, const char *fmt, va_list args);

  /**
   * @fn void MutableString::deleteCharactersInRange(MutableString *self, const Range range)
   * @brief Deletes the characters within `range` from this MutableString.
   * @param self The MutableString.
   * @param range The Range of characters to delete.
   * @memberof MutableString
   */
  void (*deleteCharactersInRange)(MutableString *self, const Range range);

  /**
   * @fn MutableString *MutableString::init(MutableString *self)
   * @brief Initializes this MutableString.
   * @param self The MutableString.
   * @return The initialized MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*init)(MutableString *self);

  /**
   * @fn MutableString *MutableString::initWithCapacity(MutableString *self, size_t capacity)
   * @brief Initializes this MutableString with the given `capacity`.
   * @param self The MutableString.
   * @param capacity The capacity, in bytes.
   * @return The initialized MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*initWithCapacity)(MutableString *self, size_t capacity);

  /**
   * @fn MutableString *MutableString::initWithCharacters(MutableString *self, const char *chars)
   * @brief Initializes this MutableString by copying `chars`.
   * @param self The MutableString.
   * @param chars The null-terminated, UTF-8 encoded C string.
   * @return The initialized MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*initWithCharacters)(MutableString *self, const char *chars);

  /**
   * @fn MutableString *MutableString::initWithString(MutableString *self, const String *string)
   * @brief Initializes this MutableString with the contents of `string`.
   * @param self The MutableString.
   * @param string A String.
   * @return The initialized MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*initWithString)(MutableString *self, const String *string);

  /**
   * @fn MutableString *MutableString::initWithString(MutableString *self, const char *fmt, ...)
   * @brief Initializes this MutableString with the specified format string.
   * @param self The MutableString.
   * @param fmt The format string.
   * @return The initialized MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*initWithFormat)(MutableString *self, const char *fmt, ...);

  /**
   * @fn MutableString *MutableString::initWithVaList(MutableString *self, const char *fmt, va_list args)
   * @brief Initializes this MutableString with the specified arguments list.
   * @param self The MutableString.
   * @param fmt The format string.
   * @param args The format arguments.
   * @return The initialized MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*initWithVaList)(MutableString *self, const char *fmt, va_list args);

  /**
   * @fn void MutableString::insertCharactersAtIndex(MutableString *self, const char *chars, size_t index)
   * @brief Inserts the specified String at the given index.
   * @param self The MutableString.
   * @param chars The null-terminated UTF-8 encoded C string to insert.
   * @param index The index.
   * @memberof MutableString
   */
  void (*insertCharactersAtIndex)(MutableString *self, const char *chars, size_t index);

  /**
   * @fn void MutableString::insertStringAtIndex(MutableString *self, const String *string, size_t index)
   * @brief Inserts the specified String at the given index.
   * @param self The MutableString.
   * @param string The String to insert.
   * @param index The index.
   * @memberof MutableString
   */
  void (*insertStringAtIndex)(MutableString *self, const String *string, size_t index);

  /**
   * @fn void MutableString::replaceCharactersInRange(MutableString *self, const Range range, const char *chars)
   * @brief Replaces the characters in `range` with the given characters.
   * @param self The MutableString.
   * @param range The Range of characters to replace.
   * @param chars The null-terminated UTF-8 encoded C string to substitute.
   * @memberof MutableString
   */
  void (*replaceCharactersInRange)(MutableString *self, const Range range, const char *chars);

  /**
   * @fn void MutableString::replaceOccurrencesOfCharacters(MutableString *self, const char *chars, const char *replacement)
   * @brief Replaces all occurrences of `chars` with the given `replacement`.
   * @param self The MutableString.
   * @param chars The null-terminated UTF-8 encoded C string to replace.
   * @param replacement The null-terminated UTF-8 encoded C string replacement.
   * @memberof MutableString
   */
  void (*replaceOccurrencesOfCharacters)(MutableString *self, const char *chars, const char *replacement);

  /**
   * @fn void MutableString::replaceOccurrencesOfCharactersInRange(MutableString *self, const char *chars, const Range range, const char *replacement)
   * @brief Replaces occurrences of `chars` in `range` with the given `replacement`.
   * @param self The MutableString.
   * @param chars The null-terminated UTF-8 encoded C string to replace.
   * @param range The Range in which to replace.
   * @param replacement The null-terminated UTF-8 encoded C string replacement.
   * @memberof MutableString
   */
  void (*replaceOccurrencesOfCharactersInRange)(MutableString *self, const char *chars, const Range range, const char *replacement);

  /**
   * @fn void MutableString::replaceOccurrencesOfString(MutableString *self, const String *string, const String *replacement)
   * @brief Replaces all occurrences of `string` with the given `replacement`.
   * @param self The MutableString.
   * @param string The String to replace.
   * @param replacement The String replacement.
   * @memberof MutableString
   */
  void (*replaceOccurrencesOfString)(MutableString *self, const String *string, const String *replacement);

  /**
   * @fn void MutableString::replaceOccurrencesOfStringInRange(MutableString *self, const String *string, const Range range, const String *replacement)
   * @brief Replaces occurrences of `string` in `range` with the given `replacement`.
   * @param self The MutableString.
   * @param string The String to replace.
   * @param range The Range in which to replace.
   * @param replacement The String replacement.
   * @memberof MutableString
   */
  void (*replaceOccurrencesOfStringInRange)(MutableString *self, const String *string, const Range range, const String *replacement);

  /**
   * @fn void MutableString::replaceStringInRange(MutableString *self, const Range range, const String *string)
   * @brief Replaces the characters in `range` with the contents of `string`.
   * @param self The MutableString.
   * @param range The Range of characters to replace.
   * @param string The String to substitute.
   * @memberof MutableString
   */
  void (*replaceStringInRange)(MutableString *self, const Range range, const String *string);

  /**
   * @fn void MutableString::setCharacters(MutableString *self, const char *chars)
   * @brief Sets the contents of this MutableString to `chars`.
   * @param self The MutableString.
   * @param chars The characters to set, or NULL.
   * @memberof MutableString
   */
  void (*setCharacters)(MutableString *self, const char *chars);

  /**
   * @fn void MutableString::setFormat(MutableString *self, const char *chars)
   * @brief Sets the contents of this MutableString to the formatted string.
   * @param self The MutableString.
   * @param fmt The format string.
   * @memberof MutableString
   */
  void (*setFormat)(MutableString *self, const char *fmt, ...);

  /**
   * @fn void MutableString::setLength(MutableString *self, size_t length)
   * @brief Sets the length of this MutableString to `length`.
   * @param self The MutableString.
   * @param length The length, which must be less than or equal to `capacity`.
   * @memberof MutableString
   */
  void (*setLength)(MutableString *self, size_t length);

  /**
   * @fn void MutableString::setString(MutableString *self, const String *string)
   * @brief Sets the contents of this MutableString to that of `string`.
   * @param self The MutableString.
   * @param chars The String to set the contents from, or NULL.
   * @memberof MutableString
   */
  void (*setString)(MutableString *self, const String *string);

  /**
   * @static
   * @fn MutableString *MutableString::string(void)
   * @brief Returns a new MutableString.
   * @return The new MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*string)(void);

  /**
   * @static
   * @fn MutableString *MutableString::stringWithCapacity(size_t capacity)
   * @brief Returns a new MutableString with the given `capacity`.
   * @param capacity The desired capacity, in bytes.
   * @return The new MutableString, or `NULL` on error.
   * @memberof MutableString
   */
  MutableString *(*stringWithCapacity)(size_t capacity);

  /**
   * @fn void MutableString::trim(MutableString *self)
   * @brief Trims leading and trailing whitespace from this MutableString.
   * @param self The MutableString.
   * @memberof MutableString
   */
  void (*trim)(MutableString *self);
};

/**
 * @fn Class *MutableString::_MutableString(void)
 * @brief The MutableString archetype.
 * @return The MutableString Class.
 * @memberof MutableString
 */
OBJECTIVELY_EXPORT Class *_MutableString(void);

/**
 * @brief A convenience function for instantiating MutableStrings.
 * @param fmt The format string.
 * @return A new MutableString, or `NULL` on error.
 * @relates String
 */
OBJECTIVELY_EXPORT MutableString *mstr(const char *fmt, ...);
