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

#ifndef _Objectively_URLSession_h_
#define _Objectively_URLSession_h_

/**
 * @file
 *
 * @brief URLSession provides an API for loading resources via URLs.
 */

/**
 * @defgroup URLSession
 *
 * @brief Asynchronous loading of resources via URLs.
 */

typedef struct URLSession URLSession;
typedef struct URLSessionInterface URLSessionInterface;

#include <Objectively/Object.h>
#include <Objectively/URLRequest.h>
#include <Objectively/URLSessionConfiguration.h>
#include <Objectively/URLSessionTask.h>
#include <Objectively/URLSessionDataTask.h>
#include <Objectively/URLSessionDownloadTask.h>

/**
 * @brief The URLSession type.
 *
 * @ingroup URLSession
 */
struct URLSession {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	URLSessionInterface *interface;

	/**
	 * @brief The session configuration.
	 */
	URLSessionConfiguration *configuration;
};

/**
 * @brief The URLSession interface.
 *
 * @ingroup URLSession
 */
struct URLSessionInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The shared URLSession instance.
	 *
	 * @relates URLSession
	 */
	URLSession *(*sharedInstance)(void);

	/**
	 * @brief Creates a URLSessionDataTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDataTask, or `NULL` on error.
	 *
	 * @relates URLSession
	 */
	URLSessionDataTask *(*dataTaskWithRequest)(URLSession *self, URLRequest *request,
			URLSessionTaskCompletion completion);

	/**
	 * @brief Creates a URLSessionDataTask for the given URL.
	 *
	 * @param url The URL to `GET`.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDataTask, or `NULL` on error.
	 *
	 * @relates URLSession
	 */
	URLSessionDataTask *(*dataTaskWithURL)(URLSession *self, URL *url,
			URLSessionTaskCompletion completion);

	/**
	 * @brief Creates a URLSessionDownloadTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDownloadTask, or `NULL` on error.
	 *
	 * @relates URLSession
	 */
	URLSessionDownloadTask *(*downloadTaskWithRequest)(URLSession *self, URLRequest *request,
			URLSessionTaskCompletion completion);

	/**
	 * @brief Creates a URLSessionDownloadTask for the given URL.
	 *
	 * @param url The URL to `GET`.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDownloadTask, or `NULL` on error.
	 *
	 * @relates URLSession
	 */
	URLSessionDownloadTask *(*downloadTaskWithURL)(URLSession *self, URL *url,
			URLSessionTaskCompletion completion);

	/**
	 * @brief Initializes this URLSession with a default configuration.
	 *
	 * @return The initialized URLSession, or `NULL` on error.
	 *
	 * @relates URLSession
	 */
	URLSession *(*init)(URLSession *self);

	/**
	 * @brief Initializes this URLSession with the given configuration.
	 *
	 * @param configuration The URLSessionConfiguration.
	 *
	 * @return The initialized URLSession, or `NULL` on error.
	 *
	 * @relates URLSession
	 */
	URLSession *(*initWithConfiguration)(URLSession *self, URLSessionConfiguration *configuration);

	/**
	 * Invalidates this URLSession and cancels all pending tasks.
	 *
	 * @relates URLSession
	 */
	void (*invalidateAndCancel)(URLSession *self);
};

/**
 * @brief The URLSession Class.
 */
extern Class __URLSession;

#endif
