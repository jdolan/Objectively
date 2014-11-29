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

#ifndef _Objectively_URLRequest_h
#define _Objectively_URLRequest_h

#include <Objectively/Data.h>
#include <Objectively/Dictionary.h>
#include <Objectively/URL.h>

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
	HTTP_DELETE
} HTTPMethod;

/**
 * @brief The URLRequest type.
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
 * @brief The URLRequest type.
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
	 */
	URLRequest *(*initWithURL)(URLRequest *self, URL *url);

	/**
	 * @brief Sets the specified HTTP header for this URLRequest.
	 *
	 * @param name The HTTP header name.
	 * @param value The HTTP header value.
	 */
	void (*setHTTPHeader)(URLRequest *self, const char *name, const char *value);
};

/**
 * @brief The URLRequest Class.
 */
extern Class __URLRequest;

#endif
