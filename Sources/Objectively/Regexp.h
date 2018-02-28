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

#include <Objectively/Object.h>
#include <Objectively/String.h>

/**
 * @file
 * @brief Extended POSIX regular expressions.
 */

typedef struct Regexp Regexp;
typedef struct RegexpInterface RegexpInterface;

/**
 * @brief Extended POSIX regular expressions.
 * @extends Object
 */
struct Regexp {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	RegexpInterface *interface;

	/**
	 * @brief The pattern
	 */
	const char *pattern;

	/**
	 * @brief A bitwise-or of `REG_ICASE`, `REG_NEWLINE`.
	 */
	int options;

	/**
	 * @brief The number of parenthesized sub-expressions.
	 */
	size_t numberOfSubExpressions;

	/**
	 * @brief The backing regular expression.
	 */
	ident regex;
};

/**
 * @brief The Regexp interface.
 */
struct RegexpInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Regexp *Regexp::initWithPattern(Regexp *self, const char *pattern, int options)
	 * @brief Initializes this regular expression.
	 * @param self The Regexp.
	 * @param pattern The POSIX regular expression pattern.
	 * @param options A bitwise-or of `REG_ICASE`, `REG_NEWLINE`.
	 * @return The initialized regular expression, or `NULL` on error.
	 * @memberof Regexp
	 */
	Regexp *(*initWithPattern)(Regexp *self, const char *pattern, int options);

	/**
	 * @fn _Bool Regexp::matchesCharacters(const Regexp *self, const char *chars, int options, Range **matches)
	 * @brief Matches this regular expression against the given characters.
	 * @param self The Regexp.
	 * @param chars The characters to match.
	 * @param options A bitwise-or of `REG_NOTBOL`, `REG_NOTEOL`.
	 * @param matches An optional pointer to return matched sub-expressions.
	 * @return `true` if this Regexp matched `chars`, `false` otherwise.
	 * @remarks If provided, `matches` will be dynamically allocated and contain
	 * `numberOfSubExpressions + 1` Ranges. `matches[0]` will identify the Range of `chars` that
	 * matched the entire pattern. `matches[1..n]` will identify the Range of each corresponding
	 * sub-expression. The caller must free `matches` when done with it.
	 * @see regexec(3)
	 * @memberof Regexp
	 */
	_Bool (*matchesCharacters)(const Regexp *self, const char *chars, int options, Range **matches);

	/**
	 * @fn _Bool Regexp::matchesString(const Regexp *self, const String *string, int options, Range **matches)
	 * @brief Matches this regular expression against the given String.
	 * @param self The Regexp.
	 * @param string The String to match.
	 * @param options A bitwise-or of `REG_NOTBOL`, `REG_NOTEOL`.
	 * @param matches An optional pointer to return matched sub-expressions.
	 * @return `true` if this expression matches `string`, `false` otherwise.
	 * @remarks If provided, `matches` will be dynamically allocated and contain
	 * `numberOfSubExpressions + 1` Ranges. `matches[0]` will identify the Range of `chars` that
	 * matched the entire pattern. `matches[1..n]` will identify the Range of each corresponding
	 * sub-expression. The caller must free `matches` when done with it.
	 * @memberof Regexp
	 */
	_Bool (*matchesString)(const Regexp *self, const String *string, int options, Range **matches);
};

/**
 * @fn Class *Regexp::_Regexp(void)
 * @brief The Regexp archetype.
 * @return The Regexp Class.
 * @memberof Regexp
 */
OBJECTIVELY_EXPORT Class *_Regexp(void);

/**
 * @brief A convenience function for instantiating Regexps.
 * @param pattern The POSIX regular expression pattern.
 * @param options A bitwise-or of `REG_ICASE`, `REG_NEWLINE`.
 * @return A new Regexp, or `NULL` on error.
 * @relates Regexp
 */
OBJECTIVELY_EXPORT Regexp *re(const char *pattern, int options);
