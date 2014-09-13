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

#ifndef _Objectively_Types_h_
#define _Objectively_Types_h_

#if defined(__linux__)
#define _GNU_SOURCE
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief The id type from Objective-C.
 */
typedef void *id;

/**
 * @brief The boolean type from Objective-C.
 */
typedef enum {
	NO, YES
} BOOL;

/**
 * @brief An offset and length into contiguous collections.
 */
typedef struct {

	/**
	 * @brief The offset.
	 */
	unsigned offset;

	/**
	 * @brief The length.
	 */
	unsigned length;
} RANGE;

/**
 * @return The length of an array.
 */
#define lengthof(array) \
	(sizeof(array) / sizeof((array)[0]))

/**
 * @return The maximum of the two parameters.
 */
#define max(a, b) \
   ({ typeof(a) _a = (a); typeof(b) _b = (b); _a > _b ? _a : _b; })

/**
 * @return The minimum of the two parameters.
 */
#define min(a, b) \
   ({ typeof(a) _a = (a); typeof(b) _b = (b); _a < _b ? _a : _b; })

#endif
