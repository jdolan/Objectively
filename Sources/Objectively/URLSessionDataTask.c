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

#define BLOCK_SIZE CURL_MAX_WRITE_SIZE

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return NULL;
}

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionDataTask *this = (URLSessionDataTask *) self;

	if (this->data) {
		free(this->data);
	}

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTaskInterface

/**
 * @brief The `CURLOPT_WRITEFUNCTION` callback.
 */
static size_t writeFunction(char *data, size_t size, size_t count, id self) {

	URLSessionDataTask *this = (URLSessionDataTask *) self;

	const size_t bytesReceived = size * count;

	const size_t newBytesReceived = this->urlSessionTask.bytesReceived + bytesReceived;
	const size_t newSize = (newBytesReceived / BLOCK_SIZE + 1) * BLOCK_SIZE;

	if (newSize != this->size) {

		if (this->data == NULL) {
			this->data = malloc(newSize);
		} else {
			this->data = realloc(this->data, newSize);
		}

		assert(this->data);
		this->size = newSize;
	}

	id ptr = this->data + this->urlSessionTask.bytesReceived;
	memcpy(ptr, data, bytesReceived);

	this->urlSessionTask.bytesReceived = newBytesReceived;
	return bytesReceived;
}

/**
 * @see URLSessionTaskInterface::initWithRequestInSession(URLSessionTask *, struct URLRequest *, struct URLSession *)
 */
static URLSessionTask *initWithRequestInSession(URLSessionTask *self,
		struct URLRequest *request,
		struct URLSession *session) {

	self = super(URLSessionTask, self, initWithRequestInSession, request, session);
	if (self) {

		curl_easy_setopt(self->locals.handle, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(self->locals.handle, CURLOPT_WRITEDATA, self);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->copy = copy;
	object->dealloc = dealloc;

	URLSessionTaskInterface *sessionTask = (URLSessionTaskInterface *) clazz->interface;

	sessionTask->initWithRequestInSession = initWithRequestInSession;
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
