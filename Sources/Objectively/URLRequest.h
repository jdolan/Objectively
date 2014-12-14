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

#ifndef _Objectively_URLRequest_h_
#define _Objectively_URLRequest_h_

#include <Objectively/Data.h>
#include <Objectively/Dictionary.h>
#include <Objectively/URL.h>

/**
 * @file
 *
 * @brief A protocol-agnostic abstraction for requesting resources via URLs.
 */

typedef struct URLRequest URLRequest;
typedef struct URLRequestInterface URLRequestInterface;

/**
 * The HTTP method verbs.
 */
typedef enum {
	HTTP_NONE,
	HTTP_GET,
	HTTP_POST,
	HTTP_PUT,
	HTTP_DELETE,
	HTTP_HEAD
} HTTPMethod;

/**
 * @brief A protocol-agnostic abstraction for requesting resources via URLs.
 *
 * @extends Object
 *
 * @ingroup URLSession
 */
struct URLRequest {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	URLRequestInterface *interface;

	/**
	 * @brief The HTTP body, sent as `POST` or `PUT` data.
	 */
	Data *httpBody;

	/**
	 * @brief The HTTP headers.
	 */
	Dictionary *httpHeaders;

	/**
	 * @brief The HTTP method.
	 */
	HTTPMethod httpMethod;

	/**
	 * @brief The URL.
	 */
	URL *url;
};

/**
 * @brief The URLRequest interface.
 *
 * @ingroup URLSession
 */
struct URLRequestInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Initializes this URLRequest with the specified URL.
	 *
	 * @param url The URL.
	 *
	 * @return The initialized URLRequest, or `NULL` on error.
	 *
	 * @relates URLRequest
	 */
	URLRequest *(*initWithURL)(URLRequest *self, URL *url);

	/**
	 * @brief Sets a value for the specified HTTP header.
	 *
	 * @param value The HTTP header value.
	 * @param field The HTTP header field.
	 *
	 * @relates URLRequest
	 */
	void (*setValueForHTTPHeaderField)(URLRequest *self, const char *value, const char *field);
};

/**
 * @brief The URLRequest Class.
 */
extern Class _URLRequest;

#endif
