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

#include "URLSession.h"

#define _Class _URLSession

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSession *this = (URLSession *) self;

	$(this, invalidateAndCancel);

	$(this->locals.thread, join, NULL);

	release(this->locals.condition);
	release(this->locals.thread);
	release(this->locals.tasks);

	super(Object, self, dealloc);
}

#pragma mark - URLSession

/**
 * @brief URLSessionTask factory function.
 */
static ident taskWithRequest(URLSession *self, ident task, URLRequest *request, URLSessionTaskCompletion completion) {

	task = $((URLSessionTask *) task, initWithRequestInSession, request, self, completion);
	if (task) {

		synchronized(self->locals.lock, {
			$(self->locals.tasks, addObject, task);
		});

		$(self->locals.condition, signal);
	}

	return task;
}

/**
 * @fn URLSessionDataTask *URLSession::dataTaskWithRequest(URLSession *, URLRequest *, URLSessionTaskCompletion)
 * @memberof URLSession
 */
static URLSessionDataTask *dataTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion) {

	return taskWithRequest(self, alloc(URLSessionDataTask), request, completion);
}

/**
 * @fn URLSessionDataTask *URLSession::dataTaskWithURL(URLSession *, URL *, URLSessionTaskCompletion)
 * @memberof URLSession
 */
static URLSessionDataTask *dataTaskWithURL(URLSession *self, URL *url, URLSessionTaskCompletion completion) {

	URLRequest *request = $(alloc(URLRequest), initWithURL, url);

	URLSessionDataTask *task = $(self, dataTaskWithRequest, request, completion);

	release(request);

	return task;
}

/**
 * @fn URLSessionDownloadTask *URLSession::downloadTaskWithRequest(URLSession *, URLRequest *, URLSessionTaskCompletion)
 * @memberof URLSession
 */
static URLSessionDownloadTask *downloadTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion) {

	return taskWithRequest(self, alloc(URLSessionDownloadTask), request, completion);
}

/**
 * @fn URLSessionDownloadTask *URLSession::downloadTaskWithURL(URLSession *, URL *, URLSessionTaskCompletion)
 * @memberof URLSession
 */
static URLSessionDownloadTask *downloadTaskWithURL(URLSession *self, URL *url, URLSessionTaskCompletion completion) {

	URLRequest *request = $(alloc(URLRequest), initWithURL, url);

	URLSessionDownloadTask *task = $(self, downloadTaskWithRequest, request, completion);

	release(request);

	return task;
}

/**
 * @fn URLSession *URLSession::init(URLSession *)
 * @memberof URLSession
 */
static URLSession *init(URLSession *self) {

	URLSessionConfiguration *configuration = $(alloc(URLSessionConfiguration), init);

	self = $(self, initWithConfiguration, configuration);

	release(configuration);

	return self;
}

/**
 * @brief ThreadFunction for a URLSession.
 */
static ident run(Thread *thread) {

	URLSession *self = thread->data;

	self->locals.handle = curl_multi_init();
	assert(self->locals.handle);

	while (true) {
		int ret;

		Array *tasks = $(self, tasks);
		if (tasks->count == 0) {

			if (thread->isCancelled) {
				break;
			}

			$(self->locals.condition, wait);
			continue;
		}

		for (size_t i = 0; i < tasks->count; i++) {

			URLSessionTask *task = $(tasks, objectAtIndex, i);
			if (task->state == URLSESSIONTASK_SUSPENDING) {

				if (task->locals.handle) {
					const CURLcode err = curl_easy_pause(task->locals.handle, CURLPAUSE_ALL);
					assert(err == CURLE_OK);
				}

				task->state = URLSESSIONTASK_SUSPENDED;

			} else if (task->state == URLSESSIONTASK_RESUMING) {

				if (task->locals.handle == NULL) {

					$(task, setup);
					assert(task->locals.handle);

					const CURLMcode merr = curl_multi_add_handle(self->locals.handle, task->locals.handle);
					assert(merr == CURLM_OK);
				} else {
					const CURLcode err = curl_easy_pause(task->locals.handle, CURLPAUSE_CONT);
					assert(err == CURLE_OK);
				}

				task->state = URLSESSIONTASK_RESUMED;

			} else if (task->state == URLSESSIONTASK_CANCELING) {

				if (task->locals.handle) {
					const CURLMcode merr = curl_multi_remove_handle(self->locals.handle, task->locals.handle);
					assert(merr == CURLM_OK);
				}

				task->state = URLSESSIONTASK_CANCELED;

				if (task->completion) {
					task->completion(task, false);
				}

				$(task, teardown);

				synchronized(self->locals.lock, {
					$(self->locals.tasks, removeObject, task);
				});
			} else if (task->state == URLSESSIONTASK_COMPLETED) {

				synchronized(self->locals.lock, {
					$(self->locals.tasks, removeObject, task);
				});
			}
		}

		CURLMcode merr = curl_multi_wait(self->locals.handle, NULL, 0, 0, NULL);
		assert(merr == CURLM_OK);

		merr = curl_multi_perform(self->locals.handle, &ret);
		assert(merr == CURLM_OK);

		CURLMsg *message;
		while ((message = curl_multi_info_read(self->locals.handle, &ret))) {

			URLSessionTask *task = NULL;
			for (size_t i = 0; i < tasks->count; i++) {

				URLSessionTask *t = $(tasks, objectAtIndex, i);
				if (t->locals.handle == message->easy_handle) {
					task = t;
					break;
				}
			}

			assert(task);

			if (message->msg == CURLMSG_DONE) {

				merr = curl_multi_remove_handle(self->locals.handle, task->locals.handle);
				assert(merr == CURLM_OK);

				task->state = URLSESSIONTASK_COMPLETED;

				curl_easy_getinfo(task->locals.handle, CURLINFO_RESPONSE_CODE, (long *) &task->response->httpStatusCode);

				if (task->completion) {
					task->completion(task, message->data.result == CURLE_OK);
				}

				$(task, teardown);

				synchronized(self->locals.lock, {
					$(self->locals.tasks, removeObject, task);
				});
			}
		}

		release(tasks);
	}

	curl_multi_cleanup(self->locals.handle);

	return NULL;
}

/**
 * @fn URLSession *URLSession::initWithConfiguration(URLSession *, URLSessionConfiguration *)
 * @memberof URLSession
 */
static URLSession *initWithConfiguration(URLSession *self, URLSessionConfiguration *configuration) {

	assert(configuration);

	self = (URLSession *) super(Object, self, init);
	if (self) {
		self->configuration = retain(configuration);

		self->locals.condition = $(alloc(Condition), init);
		self->locals.lock = $(alloc(Lock), init);
		self->locals.tasks = $(alloc(MutableArray), init);
		self->locals.thread = $(alloc(Thread), initWithFunction, run, self);

		$(self->locals.thread, start);
	}

	return self;
}

/**
 * @fn void URLSession::invalidateAndCancel(URLSession *)
 * @memberof URLSession
 */
static void invalidateAndCancel(URLSession *self) {

	if (self->locals.thread->isCancelled) {
		return;
	}

	Array *tasks = $(self, tasks);

	for (size_t i = 0; i < tasks->count; i++) {

		URLSessionTask *task = $(tasks, objectAtIndex, i);
		$(task, cancel);
	}

	release(tasks);

	$(self->locals.thread, cancel);
	$(self->locals.condition, signal);
}

static URLSession *_sharedInstance;

/**
 * @fn URLSession *URLSession::sharedInstance()
 * @memberof URLSession
 */
static URLSession *sharedInstance(void) {

	static Once once;

	do_once(&once, {
		_sharedInstance = $(alloc(URLSession), init);
	});

	return _sharedInstance;
}

/**
 * @fn Array *URLSession::tasks(const URLSession *)
 * @memberof URLSession
 */
static Array *tasks(const URLSession *self) {

	Array *array;

	synchronized(self->locals.lock, {
		array = $$(Array, arrayWithArray, (Array *) self->locals.tasks);
	});

	return array;
}

/**
 * @fn URLSessionUploadTask *URLSession::uploadTaskWithRequest(URLSession *, URLRequest *, URLSessionTaskCompletion)
 * @memberof URLSession
 */
static URLSessionUploadTask *uploadTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion) {

	return taskWithRequest(self, alloc(URLSessionUploadTask), request, completion);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((URLSessionInterface *) clazz->interface)->dataTaskWithRequest = dataTaskWithRequest;
	((URLSessionInterface *) clazz->interface)->dataTaskWithURL = dataTaskWithURL;
	((URLSessionInterface *) clazz->interface)->downloadTaskWithRequest = downloadTaskWithRequest;
	((URLSessionInterface *) clazz->interface)->downloadTaskWithURL = downloadTaskWithURL;
	((URLSessionInterface *) clazz->interface)->init = init;
	((URLSessionInterface *) clazz->interface)->initWithConfiguration = initWithConfiguration;
	((URLSessionInterface *) clazz->interface)->invalidateAndCancel = invalidateAndCancel;
	((URLSessionInterface *) clazz->interface)->sharedInstance = sharedInstance;
	((URLSessionInterface *) clazz->interface)->tasks = tasks;
	((URLSessionInterface *) clazz->interface)->uploadTaskWithRequest = uploadTaskWithRequest;

	const CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
	assert(code == CURLE_OK);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	_sharedInstance = release(_sharedInstance);

	curl_global_cleanup();
}

/**
 * @fn Class *URLSession::_URLSession(void)
 * @memberof URLSession
 */
Class *_URLSession(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "URLSession",
			.superclass = _Object(),
			.instanceSize = sizeof(URLSession),
			.interfaceOffset = offsetof(URLSession, interface),
			.interfaceSize = sizeof(URLSessionInterface),
			.initialize = initialize,
			.destroy = destroy,
		});
	});

	return clazz;
}

#undef _Class
