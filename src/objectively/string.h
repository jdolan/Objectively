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

/*
 * @brief A mutable string implementation.
 */
struct String {
	struct Object object;

	/*
	 * @brief The backing character array.
	 */
	char *str;

	/*
	 * @brief The backing character array length.
	 */
	size_t len;

	/*
	 * @brief Appends the specified formatted string.
	 *
	 * @return The resulting String.
	 */
	id (*appendFormat)(id self, const char *fmt, ...);

	/*
	 * @brief Appends the specified String.
	 *
	 * @return The resulting String.
	 */
	id (*appendString)(id self, const id other);

	/*
	 * @brief Compares this object to other.
	 *
	 * @return Greater than, equal to, or less than zero.
	 */
	int (*compareTo)(const id self, const id other, RANGE range);

	/*
	 *
	 */
	BOOL (*hasPrefix)(const id self, const id prefix);

	BOOL (*hasSuffix)(const id self, const id suffix);

	id (*substring)(const id self, RANGE range);
};

extern const Class *String;

#endif
