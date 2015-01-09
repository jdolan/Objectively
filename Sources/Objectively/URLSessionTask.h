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

#ifndef _Objectively_URLSessionTask_h_
#define _Objectively_URLSessionTask_h_

#include <Objectively/Object.h>
#include <Objectively/Thread.h>

/**
 * @file
 *
 * @brief URL session tasks are handles to pending URL operations.
 */

typedef struct URLSessionTask URLSessionTask;
typedef struct URLSessionTaskInterface URLSessionTaskInterface;

/**
 * @brief A function pointer for URLSessionTask completion.
 *
 * @param task The URLSessionTask.
 * @param success `YES` if the task was successful, `NO` otherwise.
 */
typedef void (*URLSessionTaskCompletion)(URLSessionTask *task, BOOL success);

/**
 * @brief The various states a URLSessionTask may be in.
 */
typedef enum {
	URLSESSIONTASK_RESUMING,
	URLSESSIONTASK_RESUMED,
	URLSESSIONTASK_SUSPENDING,
	URLSESSIONTASK_SUSPENDED,
	URLSESSIONTASK_CANCELING,
	URLSESSIONTASK_CANCELED,
	URLSESSIONTASK_COMPLETED
} URLSessionTaskState;

/**
 * @brief URL session tasks are handles to pending URL operations.
 *
 * @extends Object
 *
 * @ingroup URLSession
 */
struct URLSessionTask {

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
	URLSessionTaskInterface *interface;

	/**
	 * @private
	 */
	struct {

		/**
		 * @brief The backing libcurl handle.
		 */
		id handle;

		/**
		 * @brief HTTP headers, in libcurl list structure.
		 */
		id httpHeaders;

	} locals;

	/**
	 * @brief The count of bytes this task expects to receive.
	 */
	size_t bytesExpectedToReceive;

	/**
	 * @brief The count of bytes this task expects to send.
	 */
	size_t bytesExpectedToSend;

	/**
	 * @brief The count of bytes received.
	 */
	size_t bytesReceived;

	/**
	 * @brief The count of bytes sent.
	 */
	size_t bytesSent;

	/**
	 * @brief The completion function.
	 */
	URLSessionTaskCompletion completion;

	/**
	 * @brief The error buffer.
	 */
	char *error;

	/**
	 * @brief The request.
	 */
	struct URLRequest *request;

	/**
	 * @brief The session.
	 */
	struct URLSession *session;

	/**
	 * @brief The state.
	 */
	URLSessionTaskState state;
};

/**
 * @brief The URLSessionTask interface.
 */
struct URLSessionTaskInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Cancels this task.
	 *
	 * @relates URLSessionTask
	 */
	void (*cancel)(URLSessionTask *self);

	/**
	 * @brief Initializes this task with the given URLRequest.
	 *
	 * @param request The request.
	 * @param session The session.
	 * @param completion The completion handler.
	 *
	 * @return The initialized task, or `NULL` on error.
	 *
	 * @relates URLSessionTask
	 */
	URLSessionTask *(*initWithRequestInSession)(URLSessionTask *self, struct URLRequest *request,
			struct URLSession *session, URLSessionTaskCompletion completion);

	/**
	 * @brief Starts or resumes this task.
	 *
	 * @relates URLSessionTask
	 */
	void (*resume)(URLSessionTask *self);

	/**
	 * @brief Sets up this task.
	 *
	 * @protected
	 *
	 * @relates URLSessionTask
	 */
	void (*setup)(URLSessionTask *self);

	/**
	 * @brief Suspends this task.
	 *
	 * @relates URLSessionTask
	 */
	void (*suspend)(URLSessionTask *self);

	/**
	 * @brief Tears down this task.
	 *
	 * @protected
	 *
	 * @relates URLSessionTask
	 */
	void (*teardown)(URLSessionTask *self);
};

/**
 * @brief The URLSessionTask Class.
 */
extern Class _URLSessionTask;

#endif
