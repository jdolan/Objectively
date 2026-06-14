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

#include <check.h>
#include <string.h>

#include "Objectively.h"

static bool cacheTaskDone;
static bool cacheTaskSuccess;

static void cacheCompletion(URLSessionTask *task, bool success) {

  (void) task;
  cacheTaskDone = true;
  cacheTaskSuccess = success;
}

START_TEST(urlRequest) {

  URL *url = $(alloc(URL), initWithCharacters, "https://example.com/stats");
  ck_assert(url != NULL);

  URLRequest *request = $(alloc(URLRequest), initWithURL, url);

  Data *body = $(alloc(Data), initWithBytes, (const uint8_t *) "body", 4);
  request->httpBody = body;

  $(request, setValueForHTTPHeaderField, "no-cache", "Cache-Control");

  URLRequest *copy = (URLRequest *) $((Object *) request, copy);
  ck_assert(copy != NULL);
  ck_assert_int_eq($((Object *) request, hash), $((Object *) copy, hash));
  ck_assert($((Object *) request, isEqual, (Object *) copy));

  release(copy);
  release(request);
  release(url);
}

START_TEST(cacheStore) {

  URLCache *cache = $(alloc(URLCache), init);
  ck_assert(cache != NULL);

  URL *url = $(alloc(URL), initWithCharacters, "https://example.com/cache");
  URLRequest *request = $(alloc(URLRequest), initWithURL, url);

  URLResponse *response = $(alloc(URLResponse), init);
  response->httpStatusCode = 200;

  Data *data = $(alloc(Data), initWithBytes, (const uint8_t *) "", 0);
  $(cache, storeCachedResponseForRequest, request, response, data);

  URLCachedResponse *cachedResponse = $(cache, cachedResponseForRequest, request);
  ck_assert(cachedResponse != NULL);
  ck_assert_int_eq(200, cachedResponse->response->httpStatusCode);
  ck_assert_int_eq(0, cachedResponse->data->length);

  release(cachedResponse);
  release(data);
  release(response);
  release(request);
  release(url);
  release(cache);
}

START_TEST(cacheControl) {

  URLCache *cache = $(alloc(URLCache), init);
  ck_assert(cache != NULL);

  URL *url = $(alloc(URL), initWithCharacters, "https://example.com/no-cache");
  URLRequest *request = $(alloc(URLRequest), initWithURL, url);
  $(request, setValueForHTTPHeaderField, "no-cache", "Cache-Control");

  URLResponse *response = $(alloc(URLResponse), init);
  response->httpStatusCode = 200;

  Data *data = $(alloc(Data), initWithBytes, (const uint8_t *) "body", 4);
  $(cache, storeCachedResponseForRequest, request, response, data);

  URLCachedResponse *cachedResponse = $(cache, cachedResponseForRequest, request);
  ck_assert(cachedResponse == NULL);

  release(data);
  release(response);
  release(request);
  release(url);
  release(cache);
}

START_TEST(eviction) {

  URLCache *cache = $(alloc(URLCache), init);
  ck_assert(cache != NULL);
  $(cache, setMaxSize, 5);

  URLResponse *response = $(alloc(URLResponse), init);
  response->httpStatusCode = 200;

  URL *firstURL = $(alloc(URL), initWithCharacters, "https://example.com/1");
  URLRequest *firstRequest = $(alloc(URLRequest), initWithURL, firstURL);
  firstRequest->httpMethod = HTTP_GET;
  Data *firstData = $(alloc(Data), initWithBytes, (const uint8_t *) "abcd", 4);
  $(cache, storeCachedResponseForRequest, firstRequest, response, firstData);

  URL *secondURL = $(alloc(URL), initWithCharacters, "https://example.com/2");
  URLRequest *secondRequest = $(alloc(URLRequest), initWithURL, secondURL);
  secondRequest->httpMethod = HTTP_GET;
  Data *secondData = $(alloc(Data), initWithBytes, (const uint8_t *) "efgh", 4);
  $(cache, storeCachedResponseForRequest, secondRequest, response, secondData);

  ck_assert($(cache, cachedResponseForRequest, firstRequest) == NULL);

  URLCachedResponse *cachedResponse = $(cache, cachedResponseForRequest, secondRequest);
  ck_assert(cachedResponse != NULL);
  ck_assert_int_eq(4, cachedResponse->data->length);
  release(cachedResponse);

  release(secondData);
  release(secondRequest);
  release(secondURL);
  release(firstData);
  release(firstRequest);
  release(firstURL);
  release(response);
  release(cache);
}

START_TEST(sessionCacheHit) {

  URLSessionConfiguration *configuration = $(alloc(URLSessionConfiguration), init);
  ck_assert(configuration != NULL);
  ck_assert(configuration->urlCache == NULL);
  configuration->urlCache = $(alloc(URLCache), init);
  ck_assert(configuration->urlCache != NULL);

  URL *url = $(alloc(URL), initWithCharacters, "https://example.com/session-cache");
  URLRequest *request = $(alloc(URLRequest), initWithURL, url);
  request->httpMethod = HTTP_GET;

  URLResponse *response = $(alloc(URLResponse), init);
  response->httpStatusCode = 200;

  Data *data = $(alloc(Data), initWithBytes, (const uint8_t *) "cached", 6);
  $(configuration->urlCache, storeCachedResponseForRequest, request, response, data);

  release(data);
  release(response);

  URLSession *session = $(alloc(URLSession), initWithConfiguration, configuration);
  ck_assert(session != NULL);

  release(configuration);

  cacheTaskDone = false;
  cacheTaskSuccess = false;

  URLSessionDataTask *task = $(session, dataTaskWithRequest, request, cacheCompletion);
  ck_assert(task != NULL);

  $((URLSessionTask *) task, resume);

  ck_assert(cacheTaskDone);
  ck_assert(cacheTaskSuccess);
  ck_assert_int_eq(200, task->urlSessionTask.response->httpStatusCode);
  ck_assert(task->data != NULL);
  ck_assert_int_eq(6, task->data->length);
  ck_assert_int_eq(0, memcmp(task->data->bytes, "cached", 6));

  release(task);
  release(session);
  release(request);
  release(url);
}

int main(int argc, char **argv) {

  (void) argc;
  (void) argv;

  TCase *tcase = tcase_create("URLCache");
  tcase_add_test(tcase, urlRequest);
  tcase_add_test(tcase, cacheStore);
  tcase_add_test(tcase, cacheControl);
  tcase_add_test(tcase, eviction);
  tcase_add_test(tcase, sessionCacheHit);

  Suite *suite = suite_create("URLCache");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
