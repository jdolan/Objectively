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

#ifndef _Objectively_Regex_h_
#define _Objectively_Regex_h_

#include <regex.h>

#include <Objectively/Object.h>
#include <Objectively/String.h>

/**
 * @file
 *
 * @brief Extended POSIX regular expressions.
 */

typedef struct Regex Regex;
typedef struct RegexInterface RegexInterface;

/**
 * @brief Extended POSIX regular expressions.
 *
 * @extends Object
 */
struct Regex {

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
	RegexInterface *interface;

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
	id regex;
};

/**
 * @brief The Regex type.
 */
struct RegexInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Initializes this regular expression.
	 *
	 * @param pattern The POSIX regular expression pattern.
	 * @param options A bitwise-or of `REG_ICASE`, `REG_NEWLINE`.
	 *
	 * @return The initialized regular expression, or `NULL` on error.
	 *
	 * @relates Regex
	 */
	Regex *(*initWithPattern)(Regex *self, const char *pattern, const int cflags);

	/**
	 * @brief Matches this regular expression against the given characters.
	 *
	 * @param chars The characters to match.
	 * @param options A bitwise-or of `REG_NOTBOL`, `REG_NOTEOL`.
	 * @param matches An optional pointer to return matched sub-expressions.
	 *
	 * @return `YES` if this Regex matched `chars`, `NO` otherwise.
	 *
	 * @remark If provided, `matches` will be dynamically allocated and contain
	 * `numberOfSubExpressions + 1` RANGEs. `matches[0]` will identify the
	 * RANGE of `chars` that matched the entire pattern. `matches[1..n]` will
	 * identify the RANGE of each corresponding sub-expression. The caller must
	 * free `matches` when done with it.
	 *
	 * @see regexec(3)
	 *
	 * @relates Regex
	 */
	BOOL (*matchesCharacters)(const Regex *self, const char *chars, int options, RANGE **matches);

	/**
	 * @brief Matches this regular expression against the given String.
	 *
	 * @param string The String to match.
	 * @param options A bitwise-or of `REG_NOTBOL`, `REG_NOTEOL`.
	 * @param matches An optional pointer to return matched sub-expressions.
	 *
	 * @return `YES` if this expression matches `string`, `NO` otherwise.
	 *
	 * @remark If provided, `matches` will be dynamically allocated and contain
	 * `numberOfSubExpressions + 1` RANGEs. `matches[0]` will identify the
	 * RANGE of `chars` that matched the entire pattern. `matches[1..n]` will
	 * identify the RANGE of each corresponding sub-expression. The caller must
	 * free `matches` when done with it.
	 *
	 * @relates Regex
	 */
	BOOL (*matchesString)(const Regex *self, const String *string, int options, RANGE **matches);
};

/**
 * @brief The Regex Class.
 */
extern Class _Regex;

#endif
