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

#ifndef _Objectively_Error_h_
#define _Objectively_Error_h_

#include <Objectively/Object.h>
#include <Objectively/String.h>

/**
 * @file
 *
 * @brief Encapsulation for error conditions.
 */

typedef struct Error Error;
typedef struct ErrorInterface ErrorInterface;

/**
 * @brief Encapsulation for error conditions.
 *
 * @extends Object
 */
struct Error {

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
	ErrorInterface *interface;

	/**
	 * @brief The error code.
	 */
	int code;

	/**
	 * @brief The error domain.
	 */
	String *domain;

	/**
	 * @brief The error message.
	 */
	String *message;
};

/**
 * @brief The Error interface.
 */
struct ErrorInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Initializes an Error with the given details.
	 *
	 * @param domain The Error domain (required).
	 * @param code The error code.
	 * @param message The error message.
	 *
	 * @return The initialized Error, or `NULL` on error.
	 *
	 * @relates Error
	 */
	Error *(*initWithDomain)(Error *self, const char *domain, int code, const char *message);
};

/**
 * @brief The Error Class.
 */
extern Class _Error;

#endif
