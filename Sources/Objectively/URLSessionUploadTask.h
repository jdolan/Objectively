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

#ifndef _Objectively_URLSessionUploadTask_h_
#define _Objectively_URLSessionUploadTask_h_

#include <stdio.h>

#include <Objectively/Data.h>
#include <Objectively/URLSessionTask.h>

/**
 * @file
 *
 * @brief Use upload tasks to send files directly from disk.
 */
typedef struct URLSessionUploadTask URLSessionUploadTask;
typedef struct URLSessionUploadTaskInterface URLSessionUploadTaskInterface;

/**
 * @brief Use upload tasks to send files directly from disk.
 *
 * @extends URLSessionTask
 *
 * @ingroup URLSession
 */
struct URLSessionUploadTask {

	/**
	 * @brief The parent.
	 *
	 * @private
	 */
	URLSessionTask urlSessionTask;

	/**
	 * @brief The typed interface.
	 *
	 * @private
	 */
	URLSessionUploadTaskInterface *interface;

	/**
	 * @brief The FILE to upload.
	 */
	FILE *file;
};

/**
 * @brief The URLSessionUploadTask interface.
 */
struct URLSessionUploadTaskInterface {

	/**
	 * @brief The parent interface.
	 */
	URLSessionTaskInterface urlSessionTaskInterface;
};

/**
 * @brief The URLSessionUploadTask Class.
 */
extern Class _URLSessionUploadTask;

#endif
