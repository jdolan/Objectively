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

#ifndef _Objectively_JSON_h
#define _Objectively_JSON_h

#include <Objectively/Data.h>
#include <Objectively/Object.h>

#define JSON_PRETTY 1

typedef struct JSON JSON;
typedef struct JSONInterface JSONInterface;

/**
 * @brief The JSON type.
 */
struct JSON {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	JSONInterface *interface;

//..
};

/**
 * @brief The JSON type.
 */
struct JSONInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The shared JSON instance.
	 */
	JSON *(*sharedInstance)(void);

	/**
	 *
	 * @param obj
	 *
	 * @return
	 */
	Data *(*dataFromObject)(JSON *self, const id obj);


	/**
	 *
	 * @param options A bitwise-or of `JSON_PRETTY`, ..
	 *
	 * @return The initialized JSON, or `NULL
	 */
	JSON *(*initWithOptions)(JSON *self, int options);

	/**
	 *
	 * @param data
	 *
	 * @return
	 */
	id (*objectFromData)(JSON *self, const Data *data);

};

/**
 * @brief The JSON Class.
 */
extern Class __JSON;

#endif
