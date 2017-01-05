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

#include <Objectively/Array.h>
#include <Objectively/Object.h>
#include <Objectively/String.h>

/**
 * @file
 * @brief Uniform Resource Locators (RFC 3986).
 */

typedef struct URL URL;
typedef struct URLInterface URLInterface;

/**
 * @brief Uniform Resource Locators (RFC 3986).
 * @see http://www.ietf.org/rfc/rfc3986.txt
 * @extends Object
 * @ingroup URLSession
 */
struct URL {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	URLInterface *interface;

	/**
	 * @brief The fragment.
	 */
	String *fragment;

	/**
	 * @brief The host.
	 */
	String *host;

	/**
	 * @brief The path.
	 */
	String *path;

	/**
	 * @brief The port.
	 */
	unsigned short port;

	/**
	 * @brief The query.
	 */
	String *query;

	/**
	 * @brief The scheme, or protocol.
	 */
	String *scheme;

	/**
	 * @brief The URL String.
	 */
	String *urlString;
};

/**
 * @brief The URL interface.
 */
struct URLInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn URL *URL::baseURL(const URL *self)
	 * @param self The URL.
	 * @return The base URL (scheme, host, and port) of this URL.
	 * @memberof URL
	 */
	URL *(*baseURL)(const URL *self);

	/**
	 * @fn URL *URL::initWithCharacters(URL *self, const char *chars)
	 * @brief Initializes this URL with the specified characters.
	 * @param self The URL.
	 * @param chars The URL characters.
	 * @return The initialized URL, or `NULL` on error.
	 * @memberof URL
	 */
	URL *(*initWithCharacters)(URL *self, const char *chars);

	/**
	 * @fn URL *URL::initWithString(URL *self, const String *string)
	 * @brief Initializes this URL with the specified String.
	 * @param self The URL.
	 * @param string The URL String.
	 * @return The initialized URL, or `NULL` on error.
	 * @memberof URL
	 */
	URL *(*initWithString)(URL *self, const String *string);

	/**
	 * @fn Array *URL::pathComponents(const URL *self)
	 * @param self The URL.
	 * @return The `path` components of this URL.
	 * @memberof URL
	 */
	Array *(*pathComponents)(const URL *self);
};

/**
 * @brief The URL Class.
 */
OBJECTIVELY_EXPORT Class *_URL(void);
