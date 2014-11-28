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
#include <curl/curl.h>

#include <Objectively/URLRequest.h>
#include <Objectively/URLSession.h>
#include <Objectively/URLSessionTask.h>

#define __Class __URLSessionTask

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSessionTask *this = (URLSessionTask *) self;

	if (this->locals.handle) {
		curl_easy_cleanup(this->locals.handle);
	}

	if (this->locals.httpHeaders) {
		curl_slist_free_all(this->locals.httpHeaders);
	}

	if (this->error) {
		free(this->error);
	}

	release(this->request);
	release(this->session);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionTaskInterface

/**
 * @see URLSessionTaskInterface::cancel(URLSessionTask *)
 */
static void cancel(URLSessionTask *self) {

	if (self->state == TASK_RUNNING || self->state == TASK_SUSPENDED) {
		curl_multi_remove_handle(self->session->locals.handle, self->locals.handle);

		self->state = TASK_CANCELING;
	}

}

/**
 * @brief A helper to populate the headers list for CURL. This list is
 * retained for the life of the task, and freed in `dealloc`.
 */
static BOOL httpHeaders_enumerator(const Dictionary *dictionary, id obj, id key, id data) {

	String *header = $(alloc(String), initWithFormat, "%s: %s",
			((String * ) key)->chars,
			((String * ) obj)->chars);

	struct curl_slist **headers = (struct curl_slist **) data;
	*headers = curl_slist_append(*headers, header->chars);

	release(header);
	return NO;
}

/**
 * @see URLSessionTaskInterface::initWithRequestInSession(URLSessionTask *, URLRequest *, URLSession *)
 */
static URLSessionTask *initWithRequestInSession(URLSessionTask *self,
		struct URLRequest *request,
		struct URLSession *session) {

	assert(request);
	assert(session);

	self = (URLSessionTask *) super(Object, self, init);
	if (self) {

		self->error = calloc(CURL_ERROR_SIZE, 1);
		assert(self->error);

		self->request = request;
		retain(request);

		self->session = session;
		retain(session);

		self->state = TASK_SUSPENDED;

		self->locals.handle = curl_easy_init();
		assert(self->locals.handle);

		curl_easy_setopt(self->locals.handle, CURLOPT_PRIVATE, self);
		curl_easy_setopt(self->locals.handle, CURLOPT_ERRORBUFFER, self->error);

		if (request->httpHeaders) {
			struct curl_slist *httpHeaders = NULL;

			$(request->httpHeaders, enumerateObjectsAndKeys, httpHeaders_enumerator, &httpHeaders);

			curl_easy_setopt(self->locals.handle, CURLOPT_HTTPHEADER, httpHeaders);

			self->locals.httpHeaders = httpHeaders;
		}

		switch (request->httpMethod) {
			case HTTP_GET:
				curl_easy_setopt(self->locals.handle, CURLOPT_HTTPGET, YES);
				break;
			case HTTP_POST:
				curl_easy_setopt(self->locals.handle, CURLOPT_HTTPPOST, YES);
				break;
			case HTTP_PUT:
				curl_easy_setopt(self->locals.handle, CURLOPT_CUSTOMREQUEST, "PUT");
				break;
			case HTTP_DELETE:
				curl_easy_setopt(self->locals.handle, CURLOPT_CUSTOMREQUEST, "DELETE");
				break;
			default:
				break;
		}

		curl_easy_setopt(self->locals.handle, CURLOPT_URL, request->url->urlString->chars);
	}

	return self;
}

/**
 * @see URLSessionTaskInterface::resume(URLSessionTask *)
 */
static void resume(URLSessionTask *self) {

	if (self->state == TASK_SUSPENDED) {
		const CURLMcode add = curl_multi_add_handle(self->session->locals.handle, self->locals.handle);
		assert(add == CURLM_OK);

		const CURLMcode perform = curl_multi_perform(self->session->locals.handle, NULL);
		assert(perform == CURLM_OK);

		self->state = TASK_RUNNING;
	}
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	ObjectInterface *object = (ObjectInterface *) clazz->interface;

	object->dealloc = dealloc;

	URLSessionTaskInterface *task = (URLSessionTaskInterface *) clazz->interface;

	task->cancel = cancel;
	task->initWithRequestInSession = initWithRequestInSession;
	task->resume = resume;
}

Class __URLSessionTask = {
	.name = "URLSessionTask",
	.superclass = &__Object,
	.instanceSize = sizeof(URLSessionTask),
	.interfaceOffset = offsetof(URLSessionTask, interface),
	.interfaceSize = sizeof(URLSessionTaskInterface),
	.initialize = initialize, };

#undef __Class
