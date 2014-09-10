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

#ifndef _objectively_thread_h_
#define _objectively_thread_h_

#include <objectively/object.h>

/**
 * @file
 *
 * Mediate access to a critical section by enforcing mutual exclusion.
 */

typedef struct Thread Thread;
typedef struct ThreadInterface ThreadInterface;

/**
 * @brief A function pointer for Thread execution.
 *
 * @param data User data.
 */
typedef id (*ThreadFunction)(id data);

/**
 * @brief The Thread type.
 */
struct Thread {

	/**
	 * @brief The parent.
	 */
	Object object;

	/**
	 * @brief The user data.
	 */
	id data;

	/**
	 * @brief The Thread function.
	 */
	ThreadFunction function;

	/**
	 * @brief The backing thread.
	 */
	id thread;

	/**
	 * @brief The typed interface.
	 */
	ThreadInterface *interface;
};

/**
 * @brief The Thread interface.
 */
struct ThreadInterface {

	/**
	 * @brief The parent.
	 */
	ObjectInterface objectInterface;

	/**
	 * @brief Cancel this Thread from another Thread.
	 */
	void (*cancel)(Thread *self);

	/**
	 * @brief Daemonize this Thread.
	 */
	void (*detach)(Thread *self);

	/**
	 * @brief Wait for the specified Thread to terminate.
	 *
	 * @param status If not NULL, the exit status is returned here.
	 */
	void (*join)(Thread *self, id *status);
};

/**
 * @brief The Thread Class.
 */
extern Class __Thread;

#endif
