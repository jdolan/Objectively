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

#include <assert.h>
#include <curl/curl.h>

#include "URLSessionDownloadTask.h"

#define _Class _URLSessionDownloadTask

#pragma mark - URLSessionTask

#define CURL_WRITEFUNC_ABORT 0

/**
 * @brief The `CURLOPT_WRITEFUNCTION` callback.
 */
static size_t writeFunction(char *data, size_t size, size_t count, ident self) {

	URLSessionDownloadTask *this = (URLSessionDownloadTask *) self;

	const size_t bytesWritten = fwrite(data, size, count, this->file);
	this->urlSessionTask.bytesReceived += bytesWritten;

	return bytesWritten;
}

/**
 * @see URLSessionTask::setup(URLSessionTask *)
 */
static void setup(URLSessionTask *self) {

	super(URLSessionTask, self, setup);

	URLSessionDownloadTask *this = (URLSessionDownloadTask *) self;

	assert(this->file);

	curl_easy_setopt(self->locals.handle, CURLOPT_WRITEFUNCTION, writeFunction);
	curl_easy_setopt(self->locals.handle, CURLOPT_WRITEDATA, self);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((URLSessionTaskInterface *) clazz->interface)->setup = setup;
}

/**
 * @fn Class *URLSessionDownloadTask::_URLSessionDownloadTask(void)
 * @memberof URLSessionDownloadTask
 */
Class *_URLSessionDownloadTask(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "URLSessionDownloadTask",
			.superclass = _URLSessionTask(),
			.instanceSize = sizeof(URLSessionDownloadTask),
			.interfaceOffset = offsetof(URLSessionDownloadTask, interface),
			.interfaceSize = sizeof(URLSessionDownloadTaskInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
