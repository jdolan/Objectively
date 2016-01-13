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

#ifndef _Objectively_JSONPath_h_
#define _Objectively_JSONPath_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief A minimal JSONPath implementation.
 */

typedef struct JSONPath JSONPath;
typedef struct JSONPathInterface JSONPathInterface;

/**
 * @brief A minimal JSONPath implementation.
 *
 * @extends Object
 *
 * @ingroup JSON
 */
struct JSONPath {

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
	JSONPathInterface *interface;
};

/**
 * @brief The JSONPath interface.
 */
struct JSONPathInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Access a nested property from JSON Data.
	 *
	 * @param root The root element.
	 * @param path The JSONPath expression.
	 *
	 * @return The nested property, or `NULL` if not found.
	 *
	 * Accessing a nested _Bool from JSON Data:
	 *
	 *     id obj = $$(JSONSerialization, objectFromData, data, 0);
	 *     _Bool *bool = $$(JSONPath, objectWithPath, obj, "$.foo.bar[1].baz");
	 *
	 * Use dot-notation (`.`) for accessing Dictionaries, and square braces (`[n]`)
	 * for accessing Arrays.
	 *
	 * @relates JSONPath
	 */
	ident (*objectWithPath)(const ident root, const char *path);
};

/**
 * @brief The JSONPath Class.
 */
extern Class _JSONPath;

#endif
