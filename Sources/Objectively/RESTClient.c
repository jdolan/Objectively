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

#include "RESTClient.h"
#include "URL.h"
#include "URLRequest.h"
#include "URLSessionDataTask.h"

#define _Class _RESTClient

#pragma mark - Async internals

typedef struct {
  RESTClientCompletion completion;
  void *user_data;
} RESTClient_AsyncState;

static void RESTClient_AsyncCompletion(URLSessionTask *task, bool success) {

  RESTClient_AsyncState *state = task->data;
  URLSessionDataTask *dataTask = (URLSessionDataTask *) task;
  const int status = task->response ? task->response->httpStatusCode : 0;

  if (state->completion) {
    state->completion(status, dataTask->data, state->user_data);
  }

  free(state);
  release(task);
}

#pragma mark - Request helpers

static int request(RESTClient *self, HTTPMethod method, const char *url_string,
    const Data *body, const char **headers, Data **out_data) {

  if (out_data) {
    *out_data = NULL;
  }

  URLSessionDataTask *task;

  if (method == HTTP_GET && !headers) {
    URL *url = $(alloc(URL), initWithCharacters, url_string);
    task = $(self->session, dataTaskWithURL, url, NULL);
    release(url);
  } else {
    URL *url = $(alloc(URL), initWithCharacters, url_string);
    URLRequest *request = $(alloc(URLRequest), initWithURL, url);
    release(url);
    request->httpMethod = method;
    if (body) {
      request->httpBody = retain((Data *) body);
      $(request, setValueForHTTPHeaderField, "application/json", "Content-Type");
    }
    if (headers) {
      for (size_t i = 0; headers[i] && headers[i + 1]; i += 2) {
        $(request, setValueForHTTPHeaderField, headers[i + 1], headers[i]);
      }
    }
    task = $(self->session, dataTaskWithRequest, request, NULL);
    release(request);
  }

  $((URLSessionTask *) task, execute);

  const int status = task->urlSessionTask.response->httpStatusCode;
  if (out_data && task->data) {
    *out_data = retain(task->data);
  }

  release(task);
  return status;
}

static void requestAsync(RESTClient *self, HTTPMethod method, const char *url_string,
    const Data *body, const char **headers, RESTClientCompletion completion, void *user_data) {

  URLSessionDataTask *task;

  if (method == HTTP_GET && !headers) {
    URL *url = $(alloc(URL), initWithCharacters, url_string);
    task = $(self->session, dataTaskWithURL, url, RESTClient_AsyncCompletion);
    release(url);
  } else {
    URL *url = $(alloc(URL), initWithCharacters, url_string);
    URLRequest *request = $(alloc(URLRequest), initWithURL, url);
    release(url);
    request->httpMethod = method;
    if (body) {
      request->httpBody = retain((Data *) body);
      $(request, setValueForHTTPHeaderField, "application/json", "Content-Type");
    }
    if (headers) {
      for (size_t i = 0; headers[i] && headers[i + 1]; i += 2) {
        $(request, setValueForHTTPHeaderField, headers[i + 1], headers[i]);
      }
    }
    task = $(self->session, dataTaskWithRequest, request, RESTClient_AsyncCompletion);
    release(request);
  }

  RESTClient_AsyncState *state = calloc(1, sizeof(RESTClient_AsyncState));
  state->completion = completion;
  state->user_data = user_data;
  task->urlSessionTask.data = state;

  $((URLSessionTask *) task, resume);
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  RESTClient *this = (RESTClient *) self;

  release(this->session);

  super(Object, self, dealloc);
}

#pragma mark - RESTClient

/**
 * @fn int RESTClient::del(RESTClient *, const char *, const char **, Data **)
 * @memberof RESTClient
 */
static int del(RESTClient *self, const char *url, const char **headers, Data **data) {
  return request(self, HTTP_DELETE, url, NULL, headers, data);
}

/**
 * @fn void RESTClient::delAsync(RESTClient *, const char *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void delAsync(RESTClient *self, const char *url, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_DELETE, url, NULL, headers, completion, user_data);
}

/**
 * @fn int RESTClient::get(RESTClient *, const char *, const char **, Data **)
 * @memberof RESTClient
 */
static int get(RESTClient *self, const char *url, const char **headers, Data **data) {
  return request(self, HTTP_GET, url, NULL, headers, data);
}

/**
 * @fn void RESTClient::getAsync(RESTClient *, const char *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void getAsync(RESTClient *self, const char *url, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_GET, url, NULL, headers, completion, user_data);
}

/**
 * @fn int RESTClient::head(RESTClient *, const char *, const char **)
 * @memberof RESTClient
 */
static int head(RESTClient *self, const char *url, const char **headers) {
  return request(self, HTTP_HEAD, url, NULL, headers, NULL);
}

/**
 * @fn void RESTClient::headAsync(RESTClient *, const char *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void headAsync(RESTClient *self, const char *url, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_HEAD, url, NULL, headers, completion, user_data);
}

/**
 * @fn RESTClient *RESTClient::init(RESTClient *)
 * @memberof RESTClient
 */
static RESTClient *init(RESTClient *self) {
  return $(self, initWithSession, $$(URLSession, sharedInstance));
}

/**
 * @fn RESTClient *RESTClient::initWithSession(RESTClient *, URLSession *)
 * @memberof RESTClient
 */
static RESTClient *initWithSession(RESTClient *self, URLSession *session) {

  self = (RESTClient *) super(Object, self, init);
  if (self) {
    assert(session);
    self->session = retain(session);
  }
  return self;
}

/**
 * @fn int RESTClient::options(RESTClient *, const char *, const char **, Data **)
 * @memberof RESTClient
 */
static int options(RESTClient *self, const char *url, const char **headers, Data **data) {
  return request(self, HTTP_OPTIONS, url, NULL, headers, data);
}

/**
 * @fn void RESTClient::optionsAsync(RESTClient *, const char *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void optionsAsync(RESTClient *self, const char *url, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_OPTIONS, url, NULL, headers, completion, user_data);
}

/**
 * @fn int RESTClient::patch(RESTClient *, const char *, const Data *, const char **, Data **)
 * @memberof RESTClient
 */
static int patch(RESTClient *self, const char *url, const Data *body, const char **headers, Data **data) {
  return request(self, HTTP_PATCH, url, body, headers, data);
}

/**
 * @fn void RESTClient::patchAsync(RESTClient *, const char *, const Data *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void patchAsync(RESTClient *self, const char *url, const Data *body, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_PATCH, url, body, headers, completion, user_data);
}

/**
 * @fn int RESTClient::post(RESTClient *, const char *, const Data *, const char **, Data **)
 * @memberof RESTClient
 */
static int post(RESTClient *self, const char *url, const Data *body, const char **headers, Data **data) {
  return request(self, HTTP_POST, url, body, headers, data);
}

/**
 * @fn void RESTClient::postAsync(RESTClient *, const char *, const Data *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void postAsync(RESTClient *self, const char *url, const Data *body, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_POST, url, body, headers, completion, user_data);
}

/**
 * @fn int RESTClient::put(RESTClient *, const char *, const Data *, const char **, Data **)
 * @memberof RESTClient
 */
static int put(RESTClient *self, const char *url, const Data *body, const char **headers, Data **data) {
  return request(self, HTTP_PUT, url, body, headers, data);
}

/**
 * @fn void RESTClient::putAsync(RESTClient *, const char *, const Data *, const char **, RESTClientCompletion, void *)
 * @memberof RESTClient
 */
static void putAsync(RESTClient *self, const char *url, const Data *body, const char **headers,
    RESTClientCompletion completion, void *user_data) {
  requestAsync(self, HTTP_PUT, url, body, headers, completion, user_data);
}

static RESTClient *_sharedInstance;

/**
 * @fn RESTClient *RESTClient::sharedInstance(void)
 * @memberof RESTClient
 */
static RESTClient *sharedInstance(void) {

  static Once once;

  do_once(&once, {
    _sharedInstance = $(alloc(RESTClient), init);
  });

  return _sharedInstance;
}

#pragma mark - Class lifecycle

static void destroy(Class *clazz) {
  _sharedInstance = release(_sharedInstance);
}

static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  RESTClientInterface *rest = (RESTClientInterface *) clazz->interface;

  rest->init = init;
  rest->initWithSession = initWithSession;

  rest->del = del;
  rest->get = get;
  rest->head = head;
  rest->options = options;
  rest->patch = patch;
  rest->post = post;
  rest->put = put;

  rest->delAsync = delAsync;
  rest->getAsync = getAsync;
  rest->headAsync = headAsync;
  rest->optionsAsync = optionsAsync;
  rest->patchAsync = patchAsync;
  rest->postAsync = postAsync;
  rest->putAsync = putAsync;

  rest->sharedInstance = sharedInstance;
}

Class *_RESTClient(void) {

  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "RESTClient",
      .superclass = _Object(),
      .instanceSize = sizeof(RESTClient),
      .interfaceSize = sizeof(RESTClientInterface),
      .initialize = initialize,
      .destroy = destroy,
    });
  });

  return clazz;
}

#undef _Class
