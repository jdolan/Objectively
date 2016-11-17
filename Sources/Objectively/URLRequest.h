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

#include <Objectively/Data.h>
#include <Objectively/Dictionary.h>
#include <Objectively/Object.h>
#include <Objectively/URL.h>

/**
 * @file
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
 * @extends Object
 * @ingroup URLSession
 */
struct URLRequest {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
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
 */
struct URLRequestInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn URLRequest *URLRequest::initWithURL(URLRequest *self, URL *url)
	 * @brief Initializes this URLRequest with the specified URL.
	 * @param self The URLRequest.
	 * @param url The URL.
	 * @return The initialized URLRequest, or `NULL` on error.
	 * @memberof URLRequest
	 */
	URLRequest *(*initWithURL)(URLRequest *self, URL *url);

	/**
	 * @fn void setValueForHTTPHeaderField(URLREquest *self, const char *value, const char *field)
	 * @brief Sets a value for the specified HTTP header.
	 * @param self The URLRequest.
	 * @param value The HTTP header value.
	 * @param field The HTTP header field.
	 * @memberof URLRequest
	 */
	void (*setValueForHTTPHeaderField)(URLRequest *self, const char *value, const char *field);
};

/**
 * @brief The URLRequest Class.
 */
extern Class _URLRequest;
