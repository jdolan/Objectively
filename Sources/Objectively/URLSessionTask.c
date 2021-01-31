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
#include <stdlib.h>
#include <curl/curl.h>

#include "URLRequest.h"
#include "URLResponse.h"
#include "URLSession.h"
#include "URLSessionTask.h"

#define _Class _URLSessionTask

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

	return NULL;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionTask *this = (URLSessionTask *) self;

	if (this->error) {
		free(this->error);
	}

	release(this->request);
	release(this->session);
	release(this->response);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTask

/**
 * @fn void URLSessionTask::cancel(URLSessionTask *self)
 * @memberof URLSessionTask
 */
static void cancel(URLSessionTask *self) {

	switch (self->state) {
		case URLSESSIONTASK_RESUMING:
		case URLSESSIONTASK_RESUMED:
		case URLSESSIONTASK_SUSPENDING:
		case URLSESSIONTASK_SUSPENDED:
			self->state = URLSESSIONTASK_CANCELING;
			break;
		default:
			break;
	}
}

/**
 * @fn URLSessionTask *URLSessionTask::initWithRequestInSession(URLSessionTask *self, struct URLRequest *request, struct URLSession *session, URLSessionTaskCompletion completion)
 * @memberof URLSessionTask
 */
static URLSessionTask *initWithRequestInSession(URLSessionTask *self, struct URLRequest *request,
		struct URLSession *session, URLSessionTaskCompletion completion) {

	assert(request);
	assert(session);

	self = (URLSessionTask *) super(Object, self, init);
	if (self) {

		self->error = calloc(1, CURL_ERROR_SIZE);
		assert(self->error);

		self->request = retain(request);
		self->session = retain(session);

		self->response = $(alloc(URLResponse), init);

		self->completion = completion;

		self->state = URLSESSIONTASK_SUSPENDED;
	}

	return self;
}

/**
 * @fn void URLSessionTask::resume(URLSessionTask *self)
 * @memberof URLSessionTask
 */
static void resume(URLSessionTask *self) {

	switch (self->state) {
		case URLSESSIONTASK_SUSPENDING:
		case URLSESSIONTASK_SUSPENDED:
			self->state = URLSESSIONTASK_RESUMING;
			break;
		default:
			break;
	}
}

/**
 * @brief A helper to populate the request headers list for CURL.
 */
static void requestHeaders_enumerator(const Dictionary *dictionary, ident obj, ident key, ident data) {

	String *header = $(alloc(String), initWithFormat, "%s: %s", ((String * ) key)->chars, ((String * ) obj)->chars);

	struct curl_slist **headers = (struct curl_slist **) data;
	*headers = curl_slist_append(*headers, header->chars);

	release(header);
}

/**
 * @brief The `CURLOPT_HEADERFUNCTION` for parsing response headers.
 */
static size_t responseHeader(char *buffer, size_t size, size_t count, void *data) {

	URLSessionTask *this = (URLSessionTask *) data;

	char *header = calloc(count + 1, 1);
	memcpy(header, buffer, count);

	char *delim = strchr(header, ':');
	if (delim) {
		*delim = 0;

		char *field = header;
		char *value = strtrim(delim + 1);

		$(this->response, setValueForHTTPHeaderField, value, field);
	}

	free(header);
	return count;
}

/**
 * @brief The `CURLOPT_XFERINFOFUNCTION`, which updates internal state and dispatches the task's
 * progress function.
 * @remarks This is also the mechanism for resuming suspended tasks.
 */
static int progress(ident self, curl_off_t bytesExpectedToReceive, curl_off_t bytesReceived,
		curl_off_t bytesExpectedToSend, curl_off_t bytesSent) {

	URLSessionTask *this = (URLSessionTask *) self;

	curl_easy_getinfo(this->locals.handle, CURLINFO_RESPONSE_CODE, (long *) &this->response->httpStatusCode);

	this->bytesExpectedToReceive = bytesExpectedToReceive;
	this->bytesExpectedToSend = bytesExpectedToSend;

	if (this->progress) {
		this->progress(this);
	}

	return 0;
}

/**
 * @fn void URLSessionTask::setup(URLSessionTask *self)
 * @memberof URLSessionTask
 */
static void setup(URLSessionTask *self) {

	self->locals.handle = curl_easy_init();
	assert(self->locals.handle);

	curl_easy_setopt(self->locals.handle, CURLOPT_ERRORBUFFER, self->error);
	curl_easy_setopt(self->locals.handle, CURLOPT_FOLLOWLOCATION, true);

	curl_easy_setopt(self->locals.handle, CURLOPT_HEADERFUNCTION, responseHeader);
	curl_easy_setopt(self->locals.handle, CURLOPT_HEADERDATA, self);

	curl_easy_setopt(self->locals.handle, CURLOPT_XFERINFOFUNCTION, progress);
	curl_easy_setopt(self->locals.handle, CURLOPT_XFERINFODATA, self);

	Data *body = self->request->httpBody;
	if (body) {
		curl_easy_setopt(self->locals.handle, CURLOPT_POSTFIELDS, body->bytes);
		curl_easy_setopt(self->locals.handle, CURLOPT_POSTFIELDSIZE, body->length);
	}

	struct curl_slist *httpHeaders = NULL;
	const Dictionary *headers = NULL;

	headers = self->session->configuration->httpHeaders;
	if (headers) {
		$(headers, enumerateObjectsAndKeys, requestHeaders_enumerator, &httpHeaders);
	}

	headers = self->request->httpHeaders;
	if (headers) {
		$(headers, enumerateObjectsAndKeys, requestHeaders_enumerator, &httpHeaders);
	}

	curl_easy_setopt(self->locals.handle, CURLOPT_HTTPHEADER, httpHeaders);

	self->locals.requestHeaders = httpHeaders;

	switch (self->request->httpMethod) {
		case HTTP_POST:
			curl_easy_setopt(self->locals.handle, CURLOPT_POST, true);
			break;
		case HTTP_PUT:
			curl_easy_setopt(self->locals.handle, CURLOPT_PUT, true);
			break;
		case HTTP_DELETE:
			curl_easy_setopt(self->locals.handle, CURLOPT_CUSTOMREQUEST, "DELETE");
			break;
		case HTTP_HEAD:
			curl_easy_setopt(self->locals.handle, CURLOPT_NOBODY, true);
			break;
		default:
			break;
	}

	curl_easy_setopt(self->locals.handle, CURLOPT_URL, self->request->url->urlString->chars);
}

/**
 * @fn void URLSessionTask::suspend(URLSessionTask *self)
 * @memberof URLSessionTask
 */
static void suspend(URLSessionTask *self) {

	switch (self->state) {
		case URLSESSIONTASK_RESUMING:
		case URLSESSIONTASK_RESUMED:
			self->state = URLSESSIONTASK_SUSPENDING;
			break;
		default:
			break;
	}
}

/**
 * @fn void URLSessionTask::teardown(URLSessionTask *self)
 * @memberof URLSessionTask
 */
static void teardown(URLSessionTask *self) {

	if (self->locals.handle) {
		curl_easy_cleanup(self->locals.handle);
		self->locals.handle = NULL;
	}

	if (self->locals.requestHeaders) {
		curl_slist_free_all(self->locals.requestHeaders);
		self->locals.requestHeaders = NULL;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->copy = copy;
	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((URLSessionTaskInterface *) clazz->interface)->cancel = cancel;
	((URLSessionTaskInterface *) clazz->interface)->initWithRequestInSession = initWithRequestInSession;
	((URLSessionTaskInterface *) clazz->interface)->resume = resume;
	((URLSessionTaskInterface *) clazz->interface)->setup = setup;
	((URLSessionTaskInterface *) clazz->interface)->suspend = suspend;
	((URLSessionTaskInterface *) clazz->interface)->teardown = teardown;
}

/**
 * @fn Class *URLSessionTask::_URLSessionTask(void)
 * @memberof URLSessionTask
 */
Class *_URLSessionTask(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "URLSessionTask",
			.superclass = _Object(),
			.instanceSize = sizeof(URLSessionTask),
			.interfaceOffset = offsetof(URLSessionTask, interface),
			.interfaceSize = sizeof(URLSessionTaskInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
