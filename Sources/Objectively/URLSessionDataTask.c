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

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include <Objectively/URLSessionDataTask.h>

#define __Class __URLSessionDataTask

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionDataTask *this = (URLSessionDataTask *) self;

	release(this->data);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTaskInterface

/**
 * @brief The `CURLOPT_WRITEFUNCTION` callback.
 */
static size_t writeFunction(char *data, size_t size, size_t count, id self) {

	URLSessionDataTask *this = (URLSessionDataTask *) self;

	const byte *bytes = (byte *) data;
	const size_t bytesReceived = size * count;

	if (this->data == NULL) {
		this->data = $(alloc(Data), initWithBytes, (byte *) data, bytesReceived);
	} else {
		$(this->data, appendBytes, (byte *) data, bytesReceived);
	}

	this->urlSessionTask.bytesReceived += bytesReceived;
	return bytesReceived;
}

/**
 * @see URLSessionTaskInterface::setup(URLSessionTask *)
 */
static void setup(URLSessionTask *self) {

	super(URLSessionTask, self, setup);

	curl_easy_setopt(self->locals.handle, CURLOPT_WRITEFUNCTION, writeFunction);
	curl_easy_setopt(self->locals.handle, CURLOPT_WRITEDATA, self);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	URLSessionTaskInterface *sessionTask = (URLSessionTaskInterface *) clazz->interface;

	sessionTask->setup = setup;
}

Class __URLSessionDataTask = {
	.name = "URLSessionDataTask",
	.superclass = &__URLSessionTask,
	.instanceSize = sizeof(URLSessionDataTask),
	.interfaceOffset = offsetof(URLSessionDataTask, interface),
	.interfaceSize = sizeof(URLSessionDataTaskInterface),
	.initialize = initialize,
};

#undef __Class
