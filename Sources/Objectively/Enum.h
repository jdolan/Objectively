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

#include <Objectively/Types.h>

/**
 * @file
 * @brief Named enumerations.
 */

/**
 * @brief The EnumName type.
 */
typedef struct {
	const char *name;
	int value;
} EnumName;

/**
 * @brief Creates an EnumName for `en` for use with `valueof`.
 * @see valueof(const EnumName *, const char *, int)
 */
#define MakeEnumName(en) \
	{ \
		.name = #en, \
		.value = en, \
	}

/**
 * @brief Creates a null-terminated array of EnumNames.
 */
#define MakeEnumNames(...) \
	{ \
		__VA_ARGS__, \
		{ \
			.name = NULL, \
			.value = 0 \
		} \
	}

/**
 * @brief Parse `string`, returning the corresponding enum value.
 * @param names A null-terminated array of EnumNames.
 * @param chars A null-terminated C string.
 * @return The `value` of the EnumName matching `string`, or `-1`.
 */
OBJECTIVELY_EXPORT int valueof(const EnumName *names, const char *chars);
