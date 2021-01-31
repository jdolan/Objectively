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

#include <Objectively/Dictionary.h>
#include <Objectively/Object.h>

/**
 * @file
 * @brief A protocol-agnostic abstraction for receiving resources via URLs.
 */

typedef struct URLResponse URLResponse;
typedef struct URLResponseInterface URLResponseInterface;

/**
 * @brief A protocol-agnostic abstraction for URLSessionTask responses.
 * @extends Object
 * @ingroup URLSession
 */
struct URLResponse {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	URLResponseInterface *interface;

	/**
	 * @brief The HTTP response headers.
	 */
	Dictionary *httpHeaders;

	/**
	 * @brief The HTTP response status code.
	 */
	int httpStatusCode;
};

/**
 * @brief The URLResponse interface.
 */
struct URLResponseInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn URLResponse *URLResponse::init(URLResponse *self)
	 * @brief Initializes this URLResponse.
	 * @param self The URLResponse.
	 * @return The initialized URLResponse, or `NULL` on error.
	 * @memberof URLResponse
	 */
	URLResponse *(*init)(URLResponse *self);

	/**
	 * @fn void setValueForHTTPHeaderField(URLResponse *self, const char *value, const char *field)
	 * @brief Sets a value for the specified HTTP header.
	 * @param self The URLRequest.
	 * @param value The HTTP header value.
	 * @param field The HTTP header field.
	 * @memberof URLResponse
	 */
	void (*setValueForHTTPHeaderField)(URLResponse *self, const char *value, const char *field);
};

/**
 * @fn Class *URLResponse::_URLResponse(void)
 * @brief The URLResponse archetype.
 * @return The URLResponse Class.
 * @memberof URLResponse
 */
OBJECTIVELY_EXPORT Class *_URLResponse(void);
