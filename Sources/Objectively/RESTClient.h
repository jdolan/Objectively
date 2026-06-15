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

#pragma once

/**
 * @file
 * @brief An HTTP REST client backed by URLSession.
 */

/**
 * @defgroup REST REST
 * @brief HTTP REST client.
 */

typedef struct RESTClient RESTClient;
typedef struct RESTClientInterface RESTClientInterface;

#include <Objectively/Data.h>
#include <Objectively/Object.h>
#include <Objectively/URLSession.h>

/**
 * @brief A completion handler for asynchronous RESTClient requests.
 * @param status The HTTP response status code, or `0` on connection failure.
 * @param data The response body, or `NULL`.
 * @param user_data The user data pointer passed to the originating async method.
 * @ingroup REST
 */
typedef void (*RESTClientCompletion)(int status, Data *data, void *user_data);

/**
 * @brief An HTTP REST client backed by URLSession.
 * @details RESTClient provides synchronous and asynchronous HTTP verb methods
 * (HEAD, GET, OPTIONS, POST, PATCH, PUT, DELETE) dealing in `Data *` request
 * and response bodies. JSON marshalling is the caller's responsibility,
 * typically via JSONContext.
 * @extends Object
 * @ingroup REST
 */
struct RESTClient {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  RESTClientInterface *interface;

  /**
   * @brief The URLSession backing this client.
   */
  URLSession *session;
};

/**
 * @brief The RESTClient interface.
 */
struct RESTClientInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn RESTClient *RESTClient::init(RESTClient *self)
   * @brief Initializes this RESTClient with the shared URLSession.
   * @param self The RESTClient.
   * @return The initialized RESTClient, or `NULL` on error.
   * @memberof RESTClient
   */
  RESTClient *(*init)(RESTClient *self);

  /**
   * @fn RESTClient *RESTClient::initWithSession(RESTClient *self, URLSession *session)
   * @brief Initializes this RESTClient with the specified URLSession.
   * @param self The RESTClient.
   * @param session The URLSession.
   * @return The initialized RESTClient, or `NULL` on error.
   * @memberof RESTClient
   */
  RESTClient *(*initWithSession)(RESTClient *self, URLSession *session);

  /**
   * @fn int RESTClient::httpDelete(RESTClient *self, const char *url, Data **data)
   * @brief Synchronously performs an HTTP `DELETE` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param data Optionally receives the retained response body. Caller must `release`.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*httpDelete)(RESTClient *self, const char *url, Data **data);

  /**
   * @fn int RESTClient::head(RESTClient *self, const char *url)
   * @brief Synchronously performs an HTTP `HEAD` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*head)(RESTClient *self, const char *url);

  /**
   * @fn int RESTClient::get(RESTClient *self, const char *url, Data **data)
   * @brief Synchronously performs an HTTP `GET` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param data Optionally receives the retained response body. Caller must `release`.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*get)(RESTClient *self, const char *url, Data **data);

  /**
   * @fn int RESTClient::patch(RESTClient *self, const char *url, const Data *body, Data **data)
   * @brief Synchronously performs an HTTP `PATCH` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param body The request body, or `NULL`.
   * @param data Optionally receives the retained response body. Caller must `release`.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*patch)(RESTClient *self, const char *url, const Data *body, Data **data);

  /**
   * @fn int RESTClient::post(RESTClient *self, const char *url, const Data *body, Data **data)
   * @brief Synchronously performs an HTTP `POST` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param body The request body, or `NULL`.
   * @param data Optionally receives the retained response body. Caller must `release`.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*post)(RESTClient *self, const char *url, const Data *body, Data **data);

  /**
   * @fn int RESTClient::put(RESTClient *self, const char *url, const Data *body, Data **data)
   * @brief Synchronously performs an HTTP `PUT` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param body The request body, or `NULL`.
   * @param data Optionally receives the retained response body. Caller must `release`.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*put)(RESTClient *self, const char *url, const Data *body, Data **data);

  /**
   * @fn int RESTClient::options(RESTClient *self, const char *url, Data **data)
   * @brief Synchronously performs an HTTP `OPTIONS` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param data Optionally receives the retained response body. Caller must `release`.
   * @return The HTTP response status code, or `0` on connection failure.
   * @memberof RESTClient
   */
  int (*options)(RESTClient *self, const char *url, Data **data);

  /**
   * @fn void RESTClient::httpDeleteAsync(RESTClient *self, const char *url, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `DELETE` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*httpDeleteAsync)(RESTClient *self, const char *url,
      RESTClientCompletion completion, void *user_data);

  /**
   * @fn void RESTClient::headAsync(RESTClient *self, const char *url, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `HEAD` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*headAsync)(RESTClient *self, const char *url,
      RESTClientCompletion completion, void *user_data);

  /**
   * @fn void RESTClient::getAsync(RESTClient *self, const char *url, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `GET` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*getAsync)(RESTClient *self, const char *url,
      RESTClientCompletion completion, void *user_data);

  /**
   * @fn void RESTClient::patchAsync(RESTClient *self, const char *url, const Data *body, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `PATCH` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param body The request body, or `NULL`.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*patchAsync)(RESTClient *self, const char *url, const Data *body,
      RESTClientCompletion completion, void *user_data);

  /**
   * @fn void RESTClient::postAsync(RESTClient *self, const char *url, const Data *body, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `POST` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param body The request body, or `NULL`.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*postAsync)(RESTClient *self, const char *url, const Data *body,
      RESTClientCompletion completion, void *user_data);

  /**
   * @fn void RESTClient::putAsync(RESTClient *self, const char *url, const Data *body, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `PUT` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param body The request body, or `NULL`.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*putAsync)(RESTClient *self, const char *url, const Data *body,
      RESTClientCompletion completion, void *user_data);

  /**
   * @fn void RESTClient::optionsAsync(RESTClient *self, const char *url, RESTClientCompletion completion, void *user_data)
   * @brief Asynchronously performs an HTTP `OPTIONS` request.
   * @param self The RESTClient.
   * @param url The URL string.
   * @param completion The completion handler.
   * @param user_data User data passed through to `completion`.
   * @memberof RESTClient
   */
  void (*optionsAsync)(RESTClient *self, const char *url,
      RESTClientCompletion completion, void *user_data);

  /**
   * @static
   * @fn RESTClient *RESTClient::sharedInstance(void)
   * @return The shared RESTClient instance, backed by the shared URLSession.
   * @memberof RESTClient
   */
  RESTClient *(*sharedInstance)(void);
};

/**
 * @fn Class *RESTClient::_RESTClient(void)
 * @brief The RESTClient archetype.
 * @return The RESTClient Class.
 * @memberof RESTClient
 */
OBJECTIVELY_EXPORT Class *_RESTClient(void);
