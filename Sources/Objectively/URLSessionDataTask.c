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
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include <Objectively/MutableData.h>
#include <Objectively/URLSessionDataTask.h>

#define _Class _URLSessionDataTask

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionDataTask *this = (URLSessionDataTask *) self;

	release(this->data);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTask

#define CURL_WRITEFUNC_ABORT 0

/**
 * @brief The `CURLOPT_WRITEFUNCTION` callback.
 */
static size_t writeFunction(char *data, size_t size, size_t count, ident self) {

	URLSessionDataTask *this = (URLSessionDataTask *) self;

	const uint8_t *bytes = (uint8_t *) data;
	const size_t bytesReceived = size * count;

	if (this->data == NULL) {
		this->data = (Data *) $(alloc(MutableData), init);
	}

	$((MutableData *) this->data, appendBytes, bytes, bytesReceived);

	this->urlSessionTask.bytesReceived += bytesReceived;
	return bytesReceived;
}

/**
 * @see URLSessionTask::setup(URLSessionTask *)
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

	ObjectInterface *object = (ObjectInterface *) clazz->def->interface;

	object->dealloc = dealloc;

	URLSessionTaskInterface *sessionTask = (URLSessionTaskInterface *) clazz->def->interface;

	sessionTask->setup = setup;
}

Class *_URLSessionDataTask(void) {
	static Class clazz;
	
	if (!clazz.name) {
		clazz.name = "URLSessionDataTask";
		clazz.superclass = _URLSessionTask();
		clazz.instanceSize = sizeof(URLSessionDataTask);
		clazz.interfaceOffset = offsetof(URLSessionDataTask, interface);
		clazz.interfaceSize = sizeof(URLSessionDataTaskInterface);
		clazz.initialize = initialize;
	}

	return &clazz;
}

#undef _Class
