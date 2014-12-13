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

#ifndef _Objectively_URLSessionConfiguration_h_
#define _Objectively_URLSessionConfiguration_h_

#include <Objectively/Dictionary.h>
#include <Objectively/Object.h>
#include <Objectively/String.h>

/**
 * @file
 *
 * @brief Configuration bundle for URLSession.
 */

typedef struct URLSessionConfiguration URLSessionConfiguration;
typedef struct URLSessionConfigurationInterface URLSessionConfigurationInterface;

/**
 * @brief Configuration bundle for URLSession.
 *
 * @extends Object
 *
 * @ingroup URLSession
 */
struct URLSessionConfiguration {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	URLSessionConfigurationInterface *interface;

	/**
	 * @brief Credentials for URLRequests requiring authentication.
	 */
	struct {
		/**
		 * @brief The username.
		 */
		String *username;

		/**
		 * @brief The password.
		 */
		String *password;
	} credentials;

	/**
	 * @brief The HTTP headers added to every HTTP URLRequest.
	 */
	Dictionary *httpHeaders;

	/**
	 * @brief The maximum number of HTTP connections to open per host.
	 */
	int httpMaximumConnectionsPerHost;
};

/**
 * @brief The URLSessionConfiguration interface.
 *
 * @ingroup URLSession
 */
struct URLSessionConfigurationInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Initializes this URLSessionConfiguration.
	 *
	 * @return The initialized configuration, or `NULL` on error.
	 *
	 * @relates URLSessionConfiguration
	 */
	URLSessionConfiguration *(*init)(URLSessionConfiguration *self);
};

/**
 * @brief The URLSessionConfiguration Class.
 */
extern Class __URLSessionConfiguration;

#endif
