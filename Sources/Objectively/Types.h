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

#ifndef _Objectively_Types_h_
#define _Objectively_Types_h_

#if defined(_linux_)
#define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @file
 *
 * @brief Objectively base types.
 */

/**
 * @brief The identity type, similar to Objective-C `id`.
 */
typedef void *ident;

/**
 * @brief The byte type.
 */
typedef unsigned char byte;

/**
 * @brief A location and length into contiguous collections.
 */
typedef struct {

	/**
	 * @brief The location.
	 */
	int location;

	/**
	 * @brief The length.
	 */
	int length;
} RANGE;

/**
 * @brief Comparison constants.
 */
typedef enum {
	ASCENDING = -1,
	SAME = 0,
	DESCENDING = 1,
} ORDER;

/**
 * @brief The Comparator function type for ordering Objects.
 *
 * @return The ORDER of `obj1` relative to `obj2`.
 */
typedef ORDER (*Comparator)(const ident obj1, const ident obj2);

/**
 * @return The length of an array.
 */
#define lengthof(array) \
	(sizeof(array) / sizeof((array)[0]))

#endif
