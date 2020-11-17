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

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

#if defined(_MSC_VER)
 #include "Windowly.h"
#endif

#ifndef OBJECTIVELY_EXPORT
 #define OBJECTIVELY_EXPORT extern
#endif

#undef interface

/**
 * @file
 * @brief Objectively base types.
 */

/**
 * @brief The identity type, similar to Objective-C `id`.
 */
typedef void *ident;

/**
 * @brief A location and length into contiguous collections.
 */
typedef struct {

	/**
	 * @brief The location.
	 */
	ssize_t location;

	/**
	 * @brief The length.
	 */
	size_t length;
} Range;

/**
 * @brief Comparison constants.
 */
typedef enum {
	OrderAscending = -1,
	OrderSame = 0,
	OrderDescending = 1,
} Order;

/**
 * @brief The Comparator function type for ordering Objects.
 * @param obj1 The first Object.
 * @param obj2 The second Object.
 * @return The Order of `obj1` relative to `obj2`.
 */
typedef Order (*Comparator)(const ident obj1, const ident obj2);

/**
 * @brief The Consumer function type.
 * @param data User data.
 */
typedef void (*Consumer)(ident data);

/**
 * @brief The BiConsumer function type.
 * @param data1 The first argument.
 * @param data2 The second argument.
 */
typedef void (*BiConsumer)(ident data1, ident data2);

/**
 * @brief The Functor function type for transforming Objects.
 * @param obj The Object to transform.
 * @param data User data.
 * @return The transformation result.
 */
typedef ident (*Functor)(const ident obj, ident data);

/**
 * @brief The Predicate function type for filtering Objects.
 * @param obj The Object to test.
 * @param data User data.
 * @return True if `obj` satisfies the predicate, false otherwise.
 */
typedef _Bool (*Predicate)(const ident obj, ident data);

/**
 * @brief The Producer function type.
 * @param data User data.
 * @return The product.
 */
typedef ident (*Producer)(ident data);

/**
 * @brief The Reducer function type for reducing collections.
 * @param obj The Object to reduce.
 * @param accumulator The accumulator.
 * @param data User data.
 * @return The reduction result.
 */
typedef ident (*Reducer)(const ident obj, ident accumulator, ident data);

/**
 * @return The value, clamped to the bounds.
 */
#define clamp(val, min, max) \
	({ \
		typeof(val) _val = (val); typeof(min) _min = (min); typeof(max) _max = (max); \
		_max < _min ? \
				_val < _max ? _max : _val > _min ? _min : _val \
			: \
				_val < _min ? _min : _val > _max ? _max : _val; \
	})

/**
 * @return The length of an array.
 */
#ifndef lengthof
 #define lengthof(array) (sizeof(array) / sizeof((array)[0]))
#endif

/**
 * @return The maximum of the two values.
 */
#ifndef max
 #define max(a, b) ({ typeof (a) _a = (a); typeof (b) _b = (b); _a > _b ? _a : _b; })
#endif

/**
 * @return The minimum of the two values.
 */
#ifndef min
 #define min(a, b) ({ typeof (a) _a = (a); typeof (b) _b = (b); _a < _b ? _a : _b; })
#endif
