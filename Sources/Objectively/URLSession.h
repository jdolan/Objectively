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

#ifndef _Objectively_URLSession_h
#define _Objectively_URLSession_h

typedef struct URLSession URLSession;
typedef struct URLSessionInterface URLSessionInterface;

#include <Objectively/Object.h>
#include <Objectively/URLRequest.h>
#include <Objectively/URLSessionTask.h>
#include <Objectively/URLSessionDataTask.h>
#include <Objectively/URLSessionDownloadTask.h>

/**
 * @brief The URLSession type.
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
};

/**
 * @brief The URLSession type.
 */
struct URLSessionInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @return The shared URLSession instance.
	 */
	URLSession *(*sharedInstance)(void);

	/**
	 * @brief Creates a URLSessionDataTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 *
	 * @return The URLSessionDataTask, or `NULL` on error.
	 */
	URLSessionDataTask *(*dataTaskWithRequest)(URLSession *self, URLRequest *request);

	/**
	 * @brief Creates a URLSessionDataTask for the given URL.
	 *
	 * @param url The URL to `GET`.
	 *
	 * @return The URLSessionDataTask, or `NULL` on error.
	 */
	URLSessionDataTask *(*dataTaskWithURL)(URLSession *self, URL *url);

	/**
	 * @brief Creates a URLSessionDownloadTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 *
	 * @return The URLSessionDownloadTask, or `NULL` on error.
	 */
	URLSessionDownloadTask *(*downloadTaskWithRequest)(URLSession *self, URLRequest *request);

	/**
	 * @brief Creates a URLSessionDownloadTask for the given URL.
	 *
	 * @param url The URL to `GET`.
	 *
	 * @return The URLSessionDownloadTask, or `NULL` on error.
	 */
	URLSessionDownloadTask *(*downloadTaskWithURL)(URLSession *self, URL *url);

	/**
	 * @brief Initializes this URLSession.
	 *
	 * @return The initialized URLSession, or `NULL` on error.
	 */
	URLSession *(*init)(URLSession *self);

	/**
	 * Invalidates this URLSession and cancels all pending tasks.
	 */
	void (*invalidateAndCancel)(URLSession *self);
};

/**
 * @brief The URLSession Class.
 */
extern Class __URLSession;

#endif