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

#ifndef _Objectively_URLSessionTask_h
#define _Objectively_URLSessionTask_h

#include <Objectively/Object.h>
#include <Objectively/Thread.h>

/**
 * @file
 *
 * URL session tasks are handles to pending URL operations.
 */

typedef struct URLSessionTask URLSessionTask;
typedef struct URLSessionTaskInterface URLSessionTaskInterface;

/**
 * @brief Task states.
 */
typedef enum {
	TASK_RUNNING,
	TASK_CANCELING,
	TASK_SUSPENDED,
	TASK_COMPLETED
} URLSessionTaskState;

/**
 * @brief A function pointer for URLSessionTask completion.
 *
 * @param task The URLSessionTask.
 * @param success `YES` if the task was successful, `NO` otherwise.
 */
typedef void (*URLSessionTaskCompletion)(URLSessionTask *task, BOOL success);

/**
 * @brief The URLSessionTask type.
 */
struct URLSessionTask {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The typed interface.
	 */
	URLSessionTaskInterface *interface;

	/**
	 * Local members.
	 *
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

		/**
		 * @brief The Thread that executes this task.
		 */
		Thread *thread;
	} locals;

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
 * @brief The URLSessionTask type.
 */
struct URLSessionTaskInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Cancels this task.
	 */
	void (*cancel)(URLSessionTask *self);

	/**
	 * @brief Initializes this task with the given URLRequest.
	 *
	 * @param request The URLRequest.
	 * @param session The URLSession.
	 *
	 * @return The initialized task, or `NULL` on error.
	 */
	URLSessionTask *(*initWithRequestInSession)(URLSessionTask *self,
			struct URLRequest *request,
			struct URLSession *session);

	/**
	 * @brief Starts or resumes this task.
	 */
	void (*resume)(URLSessionTask *self);

	/**
	 * @brief Sets up this task.
	 *
	 * @private
	 */
	void (*setup)(URLSessionTask *self);

	/**
	 * @brief Tears down this task.
	 *
	 * @private
	 */
	void (*teardown)(URLSessionTask *self);
};

/**
 * @brief The URLSessionTask Class.
 */
extern Class __URLSessionTask;

#endif
