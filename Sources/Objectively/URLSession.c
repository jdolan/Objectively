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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>

#include <curl/curl.h>

#include <Objectively/Log.h>
#include <Objectively/URLSession.h>

#define __Class __URLSession

static Log *log;

#pragma mark - ObjectInterface

/**
 * @see ObjectInterface::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSession *this = (URLSession *) self;

	$(this, invalidateAndCancel);

	super(Object, self, dealloc);
}

#pragma mark - URLSessionInterface

/**
 * @see URLSessionInterface::dataTaskWithRequest(URLSession *, URLRequest *)
 */
static URLSessionDataTask *dataTaskWithRequest(URLSession *self, URLRequest *request) {

	URLSessionTask *task = (URLSessionTask *) alloc(URLSessionDataTask);
	return (URLSessionDataTask *) $(task, initWithRequestInSession, request, self);
}

/**
 * @see URLSessionInterface::dataTaskWithURL(URLSession *, URL *)
 */
static URLSessionDataTask *dataTaskWithURL(URLSession *self, URL *url) {

	URLRequest *request = $(alloc(URLRequest), initWithURL, url);
	URLSessionDataTask *task = $(self, dataTaskWithRequest, request);

	release(request);
	return task;
}

/**
 * @see URLSessionInterface::downloadTaskWithRequest(URLSession *, URLRequest *)
 */
static URLSessionDownloadTask *downloadTaskWithRequest(URLSession *self, URLRequest *request) {

	URLSessionTask *task = (URLSessionTask *) alloc(URLSessionDownloadTask);
	return (URLSessionDownloadTask *) $(task, initWithRequestInSession, request, self);
}

/**
 * @see URLSessionInterface::downloadTaskWithURL(URLSession *, URL *)
 */
static URLSessionDownloadTask *downloadTaskWithURL(URLSession *self, URL *url) {

	URLRequest *request = $(alloc(URLRequest), initWithURL, url);
	URLSessionDownloadTask *task = $(self, downloadTaskWithRequest, request);

	release(request);
	return task;
}

/**
 * @brief The ThreadFunction for this URLSession.
 */
static id run(Thread *thread) {

	URLSession *self = (URLSession *) thread->data;

	self->locals.handle = curl_multi_init();
	assert(self->locals.handle);

	$(log, info, "begin");
	while (YES) {

		struct timeval timeout = { 1, 0 };

		long millis;
		curl_multi_timeout(self->locals.handle, &millis);

		if (millis >= 0) {
			timeout.tv_sec = millis / 1000;
			timeout.tv_usec = (millis % 1000) * 1000;
		}

		fd_set read, write, except;
		int max;

		FD_ZERO(&read);
		FD_ZERO(&write);
		FD_ZERO(&except);

		curl_multi_fdset(self->locals.handle, &read, &write, &except, &max);

		if (max == -1) {
			select(0, NULL, NULL, NULL, &timeout);
			continue;
		}

		$(log, debug, "select %d", max);
		const int err = select(max + 1, &read, &write, &except, &timeout);
		if (err >= 0) {

			$(log, debug, "perform");
			int dontCare;
			const CURLMcode code = curl_multi_perform(self->locals.handle, &dontCare);
			if (code == CURLM_OK) {

				while (YES) {

					$(log, debug, "info");
					CURLMsg *msg = curl_multi_info_read(self->locals.handle, &dontCare);
					if (msg == NULL) {
						break;
					}

					URLSessionTask *task;
					curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, (char ** ) &task);

					assert(task);

					if (msg->msg == CURLMSG_DONE) {
						if (task->completion) {
							$(log, debug, "completion");
							task->completion(task, msg->data.result == CURLE_OK);
						}
					}
				}
			} else {
				$(log, error, "%s: curl_multi_perform: %s", __func__, curl_multi_strerror(code));
			}
		} else {
			switch (errno) {
				case EAGAIN:
				case EINTR:
					break;
				default:
					$(log, error, "%s: select: %s", __func__, strerror(errno));
					break;
			}
		}
	}

	return NULL;
}

/**
 * @see URLSessionInterface::init(URLSession *)
 */
static URLSession *init(URLSession *self) {

	self = (URLSession *) super(Object, self, init);
	if (self) {
		self->thread = $(alloc(Thread), initWithFunction, run, self);
		$(self->thread, start);
	}

	return self;
}

/**
 * @see URLSessionInterface::init(URLSession *)
 */
static void invalidateAndCancel(URLSession *self) {

	if (self->locals.handle) {
		curl_multi_cleanup(self->locals.handle);
	}
}

#pragma mark - Class lifecycle

/**
 * see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	URLSessionInterface *session = (URLSessionInterface *) clazz->interface;

	session->dataTaskWithRequest = dataTaskWithRequest;
	session->dataTaskWithURL = dataTaskWithURL;
	session->downloadTaskWithRequest = downloadTaskWithRequest;
	session->downloadTaskWithURL = downloadTaskWithURL;
	session->init = init;
	session->invalidateAndCancel = invalidateAndCancel;

	log = $(alloc(Log), initWithName, clazz->name);
	log->level = DEBUG;

	const CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
	assert(code == CURLE_OK);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(log);

	curl_global_cleanup();
}

Class __URLSession = {
	.name = "URLSession",
	.superclass = &__Object,
	.instanceSize = sizeof(URLSession),
	.interfaceOffset = offsetof(URLSession, interface),
	.interfaceSize = sizeof(URLSessionInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef __Class
