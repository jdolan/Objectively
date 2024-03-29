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

#include <Objectively/Object.h>
#include <Objectively/Thread.h>

/**
 * @file
 * @brief URL session tasks are handles to pending URL operations.
 */

typedef struct URLSessionTask URLSessionTask;
typedef struct URLSessionTaskInterface URLSessionTaskInterface;

/**
 * @brief A function pointer for URLSessionTask progress.
 * @param task The URLSessionTask.
 */
typedef void (*URLSessionTaskProgress)(URLSessionTask *task);

/**
 * @brief A function pointer for URLSessionTask completion.
 * @param task The URLSessionTask.
 * @param success `true` if the task completed successfully, `false` otherwise.
 */
typedef void (*URLSessionTaskCompletion)(URLSessionTask *task, bool success);

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
 * @extends Object
 * @ingroup URLSession
 */
struct URLSessionTask {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	URLSessionTaskInterface *interface;

	/**
	 * @private
	 */
	struct {
		/**
		 * @brief The backing libcurl handle.
		 */
		ident handle;

		/**
		 * @brief HTTP headers, in libcurl list structure.
		 */
		ident requestHeaders;
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
	 * @brief User data.
	 */
	ident data;

	/**
	 * @brief The error buffer.
	 */
	char *error;

	/**
	 * @brief The progress function.
	 */
	URLSessionTaskProgress progress;

	/**
	 * @brief The request.
	 */
	struct URLRequest *request;

	/**
	 * @brief The response.
	 */
	struct URLResponse *response;

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
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void URLSessionTask::cancel(URLSessionTask *self)
	 * @brief Cancels this task.
	 * @param self The URLSessionTask.
	 * @memberof URLSessionTask
	 */
	void (*cancel)(URLSessionTask *self);

	/**
	 * @fn void URLSessionTask::execute(URLSessionTask *self)
	 * @brief Executes this task synchronously, on the calling thread.
	 * @param self The URLSessionTask.
	 * @memberof URLSessionTask
	 * @remarks Do not mix calls to execute with calls to resume.
	 */
	void (*execute)(URLSessionTask *self);

	/**
	 * @fn URLSessionTask *URLSessionTask::initWithRequestInSession(URLSessionTask *self, struct URLRequest *request, struct URLSession *session, URLSessionTaskCompletion completion)
	 * @brief Initializes this task with the given URLRequest.
	 * @param self The URLSessionTask.
	 * @param request The request.
	 * @param session The session.
	 * @param completion The completion handler.
	 * @return The initialized task, or `NULL` on error.
	 * @memberof URLSessionTask
	 */
	URLSessionTask *(*initWithRequestInSession)(URLSessionTask *self, struct URLRequest *request,
			struct URLSession *session, URLSessionTaskCompletion completion);

	/**
	 * @fn void URLSessionTask::resume(URLSessionTask *self)
	 * @brief Starts or resumes this task.
	 * @param self The URLSessionTask.
	 * @memberof URLSessionTask
	 */
	void (*resume)(URLSessionTask *self);

	/**
	 * @protected
	 * @fn void URLSessionTask::setup(URLSessionTask *self)
	 * @brief Sets up this task.
	 * @param self The URLSessionTask.
	 * @memberof URLSessionTask
	 */
	void (*setup)(URLSessionTask *self);

	/**
	 * @fn void URLSessionTask::suspend(URLSessionTask *self)
	 * @brief Suspends this task.
	 * @param self The URLSessionTask.
	 * @memberof URLSessionTask
	 */
	void (*suspend)(URLSessionTask *self);

	/**
	 * @protected
	 * @fn void URLSessionTask::teardown(URLSessionTask *self)
	 * @brief Tears down this task.
	 * @param self The URLSessionTask.
	 * @memberof URLSessionTask
	 */
	void (*teardown)(URLSessionTask *self);
};

/**
 * @fn Class *URLSessionTask::_URLSessionTask(void)
 * @brief The URLSessionTask archetype.
 * @return The URLSessionTask Class.
 * @memberof URLSessionTask
 */
OBJECTIVELY_EXPORT Class *_URLSessionTask(void);
