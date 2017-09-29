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
#include <string.h>

#define timeval __real_timeval
#include <curl/curl.h>
#undef timeval

#include <Objectively/URLSessionUploadTask.h>

#define _Class _URLSessionUploadTask

#pragma mark - URLSessionTask

/**
 * @brief The `CURLOPT_READFUNCTION` callback.
 */
static size_t readFunction(char *data, size_t size, size_t count, ident self) {

	URLSessionUploadTask *this = (URLSessionUploadTask *) self;

	const size_t bytesRead = fread(data, size, count, this->file);
	this->urlSessionTask.bytesSent += bytesRead;

	return bytesRead;
}

/**
 * @see URLSessionTask::setup(URLSessionTask *)
 */
static void setup(URLSessionTask *self) {

	super(URLSessionTask, self, setup);

	URLSessionUploadTask *this = (URLSessionUploadTask *) self;

	assert(this->file);

	int err = fseek(this->file, 0, SEEK_END);
	assert(err == 0);

	self->bytesExpectedToSend = ftell(this->file);

	err = fseek(this->file, 0, SEEK_SET);
	assert(err == 0);

	curl_easy_setopt(self->locals.handle, CURLOPT_INFILESIZE_LARGE, self->bytesExpectedToSend);

	curl_easy_setopt(self->locals.handle, CURLOPT_READFUNCTION, readFunction);
	curl_easy_setopt(self->locals.handle, CURLOPT_READDATA, self);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((URLSessionTaskInterface *) clazz->def->interface)->setup = setup;
}

/**
 * @fn Class *URLSessionUploadTask::_URLSessionUploadTask(void)
 * @memberof URLSessionUploadTask
 */
Class *_URLSessionUploadTask(void) {
	static Class clazz;
	static Once once;

	do_once(&once, {
		clazz.name = "URLSessionUploadTask";
		clazz.superclass = _URLSessionTask();
		clazz.instanceSize = sizeof(URLSessionUploadTask);
		clazz.interfaceOffset = offsetof(URLSessionUploadTask, interface);
		clazz.interfaceSize = sizeof(URLSessionUploadTaskInterface);
		clazz.initialize = initialize;
	});

	return &clazz;
}

#undef _Class
