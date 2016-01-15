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

#ifndef _Objectively_URLSession_h_
#define _Objectively_URLSession_h_

/**
 * @file
 *
 * @brief A management context for loading resources via URLs.
 */

/**
 * @defgroup URLSession URL Loading
 *
 * @brief Asynchronous loading of resources via URLs.
 */

typedef struct URLSession URLSession;
typedef struct URLSessionInterface URLSessionInterface;

#include <Objectively/Lock.h>
#include <Objectively/MutableArray.h>
#include <Objectively/Object.h>
#include <Objectively/URLRequest.h>
#include <Objectively/URLSessionConfiguration.h>
#include <Objectively/URLSessionDataTask.h>
#include <Objectively/URLSessionDownloadTask.h>
#include <Objectively/URLSessionTask.h>
#include <Objectively/URLSessionUploadTask.h>

/**
 * @brief A management context for loading resources via URLs.
 *
 * @extends Object
 *
 * @ingroup URLSession
 */
struct URLSession {

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
	URLSessionInterface *interface;

	/**
	 * @private
	 */
	struct {

		/**
		 * @brief The libcurl handle.
		 */
		ident handle;

		/**
		 * @brief The Lock guarding access to `tasks`.
		 */
		Lock *lock;

		/**
		 * @brief The URLSessionTasks.
		 */
		MutableArray *tasks;

		/**
		 * @brief The backing Thread.
		 */
		Thread *thread;

	} locals;

	/**
	 * @brief The session configuration.
	 */
	URLSessionConfiguration *configuration;
};

/**
 * @brief The URLSession interface.
 */
struct URLSessionInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn URLSessionDataTask *URLSession::dataTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion)
	 *
	 * @brief Creates a URLSessionDataTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDataTask, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSessionDataTask *(*dataTaskWithRequest)(URLSession *self, URLRequest *request,
			URLSessionTaskCompletion completion);

	/**
	 * @fn URLSessionDataTask *URLSession::dataTaskWithURL(URLSession *self, URL *url, URLSessionTaskCompletion completion)
	 *
	 * @brief Creates a URLSessionDataTask for the given URL.
	 *
	 * @param url The URL to `GET`.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDataTask, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSessionDataTask *(*dataTaskWithURL)(URLSession *self, URL *url,
			URLSessionTaskCompletion completion);

	/**
	 * @fn URLSessionDownloadTask *URLSession::downloadTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion)
	 *
	 * @brief Creates a URLSessionDownloadTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDownloadTask, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSessionDownloadTask *(*downloadTaskWithRequest)(URLSession *self, URLRequest *request,
			URLSessionTaskCompletion completion);

	/**
	 * @fn URLSessionDownloadTask *URLSession::downloadTaskWithURL(URLSession *self, URL *url, URLSessionTaskCompletion completion)
	 *
	 * @brief Creates a URLSessionDownloadTask for the given URL.
	 *
	 * @param url The URL to `GET`.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionDownloadTask, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSessionDownloadTask *(*downloadTaskWithURL)(URLSession *self, URL *url,
			URLSessionTaskCompletion completion);

	/**
	 * @fn URLSession *URLSession::init(URLSession *self)
	 *
	 * @brief Initializes this URLSession with a default configuration.
	 *
	 * @return The initialized URLSession, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSession *(*init)(URLSession *self);

	/**
	 * @fn URLSession *URLSession::initWithConfiguration(URLSession *self, URLSessionConfiguration *configuration)
	 *
	 * @brief Initializes this URLSession with the given configuration.
	 *
	 * @param configuration The URLSessionConfiguration.
	 *
	 * @return The initialized URLSession, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSession *(*initWithConfiguration)(URLSession *self, URLSessionConfiguration *configuration);

	/**
	 * @fn void URLSession::invalidateAndCancel(URLSession *self)
	 *
	 * @brief Invalidates this URLSession and cancels all pending tasks.
	 *
	 * @memberof URLSession
	 */
	void (*invalidateAndCancel)(URLSession *self);
	
	/**
	 * @static
	 *
	 * @fn URLSession *URLSession::sharedInstance(void)
	 *
	 * @return The shared URLSession instance.
	 *
	 * @memberof URLSession
	 */
	URLSession *(*sharedInstance)(void);

	/**
	 * @fn Array *URLSession::tasks(const URLSession *self)
	 *
	 * @return An instantaneous copy of this URLSession's URLSessionTasks.
	 *
	 * @memberof URLSession
	 */
	Array *(*tasks)(const URLSession *self);

	/**
	 * @fn URLSessionUploadTask *URLSession::uploadTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion)
	 *
	 * @brief Creates a URLSessionUploadTask for the given URLRequest.
	 *
	 * @param request The URLRequest to perform.
	 * @param completion The completion handler.
	 *
	 * @return The URLSessionUploadTask, or `NULL` on error.
	 *
	 * @memberof URLSession
	 */
	URLSessionUploadTask *(*uploadTaskWithRequest)(URLSession *self, URLRequest *request,
			URLSessionTaskCompletion completion);
};

/**
 * @brief The URLSession Class.
 */
extern Class _URLSession;

#endif
