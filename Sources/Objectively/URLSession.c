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

#include <Objectively/Once.h>
#include <Objectively/URLSession.h>

#define _Class _URLSession

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	URLSession *this = (URLSession *) self;

	$(this, invalidateAndCancel);

	$(this->locals.thread, join, NULL);
	release(this->locals.thread);

	release(this->locals.tasks);

	super(Object, self, dealloc);
}

#pragma mark - URLSession

/**
 * @fn URLSessionDataTask *URLSession::dataTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion)
 *
 * @memberof URLSession
 */
static URLSessionDataTask *dataTaskWithRequest(URLSession *self, URLRequest *request,
		URLSessionTaskCompletion completion) {

	URLSessionTask *task = (URLSessionTask *) alloc(URLSessionDataTask);
	task = $(task, initWithRequestInSession, request, self, completion);

	WithLock(self->locals.lock, {
		$(self->locals.tasks, addObject, task);
	});

	return (URLSessionDataTask *) task;
}

/**
 * @fn URLSessionDataTask *URLSession::dataTaskWithURL(URLSession *self, URL *url, URLSessionTaskCompletion completion)
 *
 * @memberof URLSession
 */
static URLSessionDataTask *dataTaskWithURL(URLSession *self, URL *url,
		URLSessionTaskCompletion completion) {

	URLRequest *request = $(alloc(URLRequest), initWithURL, url);

	URLSessionDataTask *task = $(self, dataTaskWithRequest, request, completion);

	release(request);

	return task;
}

/**
 * @fn URLSessionDownloadTask *URLSession::downloadTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion)
 *
 * @memberof URLSession
 */
static URLSessionDownloadTask *downloadTaskWithRequest(URLSession *self, URLRequest *request,
		URLSessionTaskCompletion completion) {

	URLSessionTask *task = (URLSessionTask *) alloc(URLSessionDownloadTask);
	task = $(task, initWithRequestInSession, request, self, completion);

	WithLock(self->locals.lock, {
		$(self->locals.tasks, addObject, task);
	})

	return (URLSessionDownloadTask *) task;
}

/**
 * @fn URLSessionDownloadTask *URLSession::downloadTaskWithURL(URLSession *self, URL *url, URLSessionTaskCompletion completion)
 *
 * @memberof URLSession
 */
static URLSessionDownloadTask *downloadTaskWithURL(URLSession *self, URL *url,
		URLSessionTaskCompletion completion) {

	URLRequest *request = $(alloc(URLRequest), initWithURL, url);

	URLSessionDownloadTask *task = $(self, downloadTaskWithRequest, request, completion);

	release(request);

	return task;
}

/**
 * @fn URLSession *URLSession::init(URLSession *self)
 *
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

		if (tasks->count == 0 && thread->isCancelled) {
			break;
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

				WithLock(self->locals.lock, {
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

				if (task->completion) {
					task->completion(task, message->data.result == CURLE_OK);
				}

				$(task, teardown);

				WithLock(self->locals.lock, {
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
 * @fn URLSession *URLSession::initWithConfiguration(URLSession *self, URLSessionConfiguration *configuration)
 *
 * @memberof URLSession
 */
static URLSession *initWithConfiguration(URLSession *self, URLSessionConfiguration *configuration) {

	assert(configuration);

	self = (URLSession *) super(Object, self, init);
	if (self) {
		self->configuration = retain(configuration);

		self->locals.lock = $(alloc(Lock), init);
		self->locals.tasks = $(alloc(MutableArray), init);
		self->locals.thread = $(alloc(Thread), initWithFunction, run, self);

		$(self->locals.thread, start);
	}

	return self;
}

/**
 * @fn void URLSession::invalidateAndCancel(URLSession *self)
 *
 * @memberof URLSession
 */
static void invalidateAndCancel(URLSession *self) {

	Array *tasks = $(self, tasks);

	for (size_t i = 0; i < tasks->count; i++) {

		URLSessionTask *task = $(tasks, objectAtIndex, i);
		$(task, cancel);
	}

	release(tasks);

	$(self->locals.thread, cancel);
}

static URLSession *_sharedInstance;

/**
 * @fn URLSession *URLSession::sharedInstance(void)
 *
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
 * @fn Array *URLSession::tasks(const URLSession *self)
 *
 * @memberof URLSession
 */
static Array *tasks(const URLSession *self) {

	Array *array;

	WithLock(self->locals.lock, {
		array = $$(Array, arrayWithArray, (Array *) self->locals.tasks);
	});

	return array;
}

/**
 * @fn URLSessionUploadTask *URLSession::uploadTaskWithRequest(URLSession *self, URLRequest *request, URLSessionTaskCompletion completion)
 *
 * @memberof URLSession
 */
static URLSessionUploadTask *uploadTaskWithRequest(URLSession *self, URLRequest *request,
		URLSessionTaskCompletion completion) {

	URLSessionTask *task = (URLSessionTask *) alloc(URLSessionUploadTask);
	task = $(task, initWithRequestInSession, request, self, completion);

	WithLock(self->locals.lock, {
		$(self->locals.tasks, addObject, task);
	})

	return (URLSessionUploadTask *) task;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	URLSessionInterface *session = (URLSessionInterface *) clazz->interface;

	session->dataTaskWithRequest = dataTaskWithRequest;
	session->dataTaskWithURL = dataTaskWithURL;
	session->downloadTaskWithRequest = downloadTaskWithRequest;
	session->downloadTaskWithURL = downloadTaskWithURL;
	session->init = init;
	session->initWithConfiguration = initWithConfiguration;
	session->invalidateAndCancel = invalidateAndCancel;
	session->sharedInstance = sharedInstance;
	session->tasks = tasks;
	session->uploadTaskWithRequest = uploadTaskWithRequest;

	const CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
	assert(code == CURLE_OK);
}

/**
 * @see Class::destroy(Class *)
 */
static void destroy(Class *clazz) {

	release(_sharedInstance);

	curl_global_cleanup();
}

Class _URLSession = {
	.name = "URLSession",
	.superclass = &_Object,
	.instanceSize = sizeof(URLSession),
	.interfaceOffset = offsetof(URLSession, interface),
	.interfaceSize = sizeof(URLSessionInterface),
	.initialize = initialize,
	.destroy = destroy,
};

#undef _Class
