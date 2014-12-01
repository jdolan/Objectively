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

#ifndef _Objectively_JSONFormatter_h
#define _Objectively_JSONFormatter_h

#include <Objectively/Data.h>
#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief JSON serialization.
 */

/**
 * @brief Enables pretty (indented) formatting of JSON output.
 */
#define JSON_PRETTY 1

typedef struct JSONFormatter JSONFormatter;
typedef struct JSONFormatterInterface JSONFormatterInterface;

/**
 * @brief The JSONFormatter type.
 */
struct JSONFormatter {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	JSONFormatterInterface *interface;
};

/**
 * @brief The JSONFormatter type.
 */
struct JSONFormatterInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The shared JSONFormatter instance.
	 */
	JSONFormatter *(*sharedInstance)(void);

	/**
	 * @brief Serializes the given Object to JSON Data.
	 *
	 * @param obj The Object to serialize.
	 *
	 * @return The resulting JSON Data.
	 */
	Data *(*dataFromObject)(JSONFormatter *self, const id obj);

	/**
	 * @brief Initializes this JSONFormatter with the given options.
	 *
	 * @param options A bitwise-or of `JSON_PRETTY`, ..
	 *
	 * @return The initialized JSONFormatter, or `NULL
	 */
	JSONFormatter *(*initWithOptions)(JSONFormatter *self, int options);

	/**
	 * @brief Parses an Object from the specified Data.
	 *
	 * @param data The JSON Data.
	 *
	 * @return The Object, or `NULL` on error.
	 */
	id (*objectFromData)(JSONFormatter *self, const Data *data);
};

/**
 * @brief The JSONFormatter Class.
 */
extern Class __JSONFormatter;

#endif
