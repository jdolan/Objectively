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

#include "URLSession.h"
#include "URLCache.h"
#include "MutableData.h"
#include "URLSessionDataTask.h"

#define _Class _URLSessionDataTask

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  URLSessionDataTask *this = (URLSessionDataTask *) self;

  release(this->cachedResponse);
  release(this->data);

  super(Object, self, dealloc);
}

#pragma mark - URLSessionTask

#define CURL_WRITEFUNC_ABORT 0

/**
 * @brief Copies a cached response into this task's live response and data fields.
 */
static void materializeCachedResponse(URLSessionDataTask *self) {

  if (self->cachedResponse == NULL) {
    return;
  }

  release(self->urlSessionTask.response);
  self->urlSessionTask.response = (URLResponse *) $((Object *) self->cachedResponse->response, copy);

  release(self->data);
  self->data = (Data *) $((Object *) self->cachedResponse->data, copy);

  self->urlSessionTask.bytesExpectedToReceive = self->cachedResponse->size;
  self->urlSessionTask.bytesReceived = self->cachedResponse->size;
  self->urlSessionTask.bytesExpectedToSend = 0;
  self->urlSessionTask.bytesSent = 0;

  release(self->cachedResponse);
  self->cachedResponse = NULL;
}

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

/**
 * @see URLSessionTask::cancel(URLSessionTask *)
 */
static void cancel(URLSessionTask *self) {

  URLSessionDataTask *this = (URLSessionDataTask *) self;

  if (this->cachedResponse) {
    if (this->urlSessionTask.state != URLSESSIONTASK_COMPLETED) {
      release(this->cachedResponse);
      this->cachedResponse = NULL;
      this->urlSessionTask.state = URLSESSIONTASK_CANCELED;
    }

    return;
  }

  super(URLSessionTask, self, cancel);
}

/**
 * @see URLSessionTask::execute(URLSessionTask *)
 */
static void execute(URLSessionTask *self) {

  URLSessionDataTask *this = (URLSessionDataTask *) self;

  if (this->urlSessionTask.state == URLSESSIONTASK_CANCELED
      || this->urlSessionTask.state == URLSESSIONTASK_COMPLETED) {
    return;
  }

  if (this->cachedResponse) {
    materializeCachedResponse(this);
    this->urlSessionTask.state = URLSESSIONTASK_COMPLETED;
    return;
  }

  $(self, setup);

  CURLcode code = curl_easy_perform(self->locals.handle);

  curl_easy_getinfo(self->locals.handle, CURLINFO_RESPONSE_CODE, (long *) &self->response->httpStatusCode);

  this->urlSessionTask.state = URLSESSIONTASK_COMPLETED;

  if (code == CURLE_OK) {
    $(this, cacheResponse);
  }

  $(self, teardown);
}

/**
 * @fn void URLSessionDataTask::cacheResponse(URLSessionDataTask *self)
 * @memberof URLSessionDataTask
 */
static void cacheResponse(URLSessionDataTask *self) {

  if (self->cachedResponse) {
    return;
  }

  URLCache *cache = self->urlSessionTask.session->configuration->urlCache;
  if (cache) {
    $(cache, storeCachedResponseForRequest, self->urlSessionTask.request, self->urlSessionTask.response, self->data);
  }
}

/**
 * @see URLSessionTask::resume(URLSessionTask *)
 */
static void resume(URLSessionTask *self) {

  URLSessionDataTask *this = (URLSessionDataTask *) self;

  if (this->cachedResponse) {
    switch (this->urlSessionTask.state) {
      case URLSESSIONTASK_SUSPENDING:
      case URLSESSIONTASK_SUSPENDED:
      case URLSESSIONTASK_RESUMING:
        materializeCachedResponse(this);
        this->urlSessionTask.state = URLSESSIONTASK_COMPLETED;

        if (this->urlSessionTask.completion) {
          this->urlSessionTask.completion((URLSessionTask *) this, true);
        }
        break;
      default:
        break;
    }

    return;
  }

  super(URLSessionTask, self, resume);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((URLSessionTaskInterface *) clazz->interface)->cancel = cancel;
  ((URLSessionTaskInterface *) clazz->interface)->execute = execute;
  ((URLSessionTaskInterface *) clazz->interface)->setup = setup;
  ((URLSessionTaskInterface *) clazz->interface)->resume = resume;

  ((URLSessionDataTaskInterface *) clazz->interface)->cacheResponse = cacheResponse;
}

/**
 * @fn Class *URLSessionDataTask::_URLSessionDataTask(void)
 * @memberof URLSessionDataTask
 */
Class *_URLSessionDataTask(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "URLSessionDataTask",
      .superclass = _URLSessionTask(),
      .instanceSize = sizeof(URLSessionDataTask),
      .interfaceOffset = offsetof(URLSessionDataTask, interface),
      .interfaceSize = sizeof(URLSessionDataTaskInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
