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

#ifndef _Objectively_Thread_h_
#define _Objectively_Thread_h_

#include <Objectively/Object.h>

/**
 * @file
 *
 * @brief POSIX Threads.
 *
 * Asynchronous computing via multiple threads of execution.
 */

/**
 * @defgroup Concurrency Concurrency
 *
 * @brief Parallel computing and synchronization via POSIX Threads.
 */
typedef struct Thread Thread;
typedef struct ThreadInterface ThreadInterface;

/**
 * @brief The function type for Thread execution.
 *
 * @param thread The executing Thread.
 */
typedef ident (*ThreadFunction)(Thread *thread);

/**
 * @brief POSIX Threads.
 *
 * Asynchronous computing via multiple threads of execution.
 *
 * @extends Object
 *
 * @ingroup Threads
 */
struct Thread {

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
	ThreadInterface *interface;

	/**
	 * @brief The user data.
	 */
	ident data;

	/**
	 * @brief The Thread function.
	 */
	ThreadFunction function;

	/**
	 * @brief `true` when this Thread has been cancelled, `false` otherwise.
	 */
	_Bool isCancelled;

	/**
	 * @brief `true` when this Thread has been detached, `false` otherwise.
	 */
	_Bool isDetached;

	/**
	 * @brief `true` when this Thread is executing, `false` otherwise.
	 */
	_Bool isExecuting;

	/**
	 * @brief `true` when this Thread is finished, `false` otherwise.
	 */
	_Bool isFinished;

	/**
	 * @brief The backing thread.
	 *
	 * @private
	 */
	ident thread;
};

/**
 * @brief The Thread interface.
 */
struct ThreadInterface {

	/**
	 * @brief The parent interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Cancel this Thread from another Thread.
	 *
	 * @relates Thread
	 */
	void (*cancel)(Thread *self);

	/**
	 * @brief Returns the currently executing Thread.
	 *
	 * @return The currently executing Thread.
	 *
	 * @relates Thread
	 */
	Thread *(*currentThread)(void);

	/**
	 * @brief Daemonize this Thread.
	 *
	 * @relates Thread
	 */
	void (*detach)(Thread *self);

	/**
	 * Initializes this Thread.
	 *
	 * @return The initialized Thread, or `NULL` on error.
	 *
	 * @relates Thread
	 */
	Thread *(*init)(Thread *self);

	/**
	 * Initializes this Thread with the specified ThreadFunction and data.
	 *
	 * @param function The ThreadFunction to run.
	 * @param data The user data.
	 *
	 * @return The initialized Thread, or `NULL` on error.
	 *
	 * @relates Thread
	 */
	Thread *(*initWithFunction)(Thread *self, ThreadFunction function, ident data);

	/**
	 * @brief Wait for the specified Thread to terminate.
	 *
	 * @param status If not NULL, the return value of this Thread's
	 * ThreadFunction is returned here.
	 *
	 * @relates Thread
	 */
	void (*join)(Thread *self, ident *status);

	/**
	 * @brief Sends the given signal to this Thread.
	 *
	 * @param signal The signal to send.
	 *
	 * @relates Thread
	 */
	void (*kill)(Thread *self, int signal);

	/**
	 * @brief Start this Thread.
	 *
	 * @relates Thread
	 */
	void (*start)(Thread *self);
};

/**
 * @brief The Thread Class.
 */
extern Class _Thread;

#endif
