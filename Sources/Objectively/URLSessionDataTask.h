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

#include <Objectively/Data.h>
#include <Objectively/URLSessionTask.h>

/**
 * @file
 * @brief Use data tasks to send and receive Data in-memory.
 */

typedef struct URLSessionDataTask URLSessionDataTask;
typedef struct URLSessionDataTaskInterface URLSessionDataTaskInterface;

/**
 * @brief Use data tasks to send and receive Data in-memory.
 * @details Data tasks are well suited for web service invocations.
 * @extends URLSessionTask
 * @ingroup URLSession
 */
struct URLSessionDataTask {

	/**
	 * @brief The superclass.
	 */
	URLSessionTask urlSessionTask;

	/**
	 * @brief The interface.
	 * @protected
	 */
	URLSessionDataTaskInterface *interface;

	/**
	 * @brief The data received.
	 */
	Data *data;
};

/**
 * @brief The URLSessionDataTask interface.
 */
struct URLSessionDataTaskInterface {

	/**
	 * @brief The superclass interface.
	 */
	URLSessionTaskInterface urlSessionTaskInterface;
};

/**
 * @brief The URLSessionDataTask Class.
 */
OBJECTIVELY_EXPORT Class *_URLSessionDataTask(void);
