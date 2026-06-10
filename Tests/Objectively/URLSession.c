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

static Condition *condition;
static bool taskDone;
static bool taskSuccess;
static bool cacheTaskDone;
static bool cacheTaskSuccess;

/**
 * @brief The completion handler.
 */
void completion(URLSessionTask *task, bool success) {

  synchronized(condition, {
    taskDone = true;
    taskSuccess = success;
    $(condition, signal);
  });
}

static void cacheCompletion(URLSessionTask *task, bool success) {

  (void) task;

  cacheTaskDone = true;
  cacheTaskSuccess = success;
}

START_TEST(asynchronous) {

  URLSession *session = $$(URLSession, sharedInstance);
  ck_assert(session != NULL);

  URL *url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/Objectively/blob/main/README.md");
  ck_assert(url != NULL);

  URLSessionDataTask *dataTask = $(session, dataTaskWithURL, url, completion);
  ck_assert(dataTask != NULL);

  condition = $(alloc(Condition), init);

  const Time timeout = { .tv_sec = 30 };
  Date *deadline = $$(Date, dateWithTimeSinceNow, &timeout);

  taskDone = false;
  $((URLSessionTask *) dataTask, resume);

  synchronized(condition, {
    while (!taskDone) {
      ck_assert($(condition, waitUntilDate, deadline));
    }
  });
  ck_assert(taskSuccess);

  release(deadline);

  ck_assert_int_eq(200, dataTask->urlSessionTask.response->httpStatusCode);
  ck_assert(dataTask->data != NULL);

  release(dataTask);

  URLSessionDownloadTask *downloadTask = $(session, downloadTaskWithURL, url, completion);

  downloadTask->file = fopen("/tmp/README.md", "w");
  ck_assert(downloadTask->file != NULL);

  deadline = $$(Date, dateWithTimeSinceNow, &timeout);

  taskDone = false;
  $((URLSessionTask *) downloadTask, resume);

  synchronized(condition, {
    while (!taskDone) {
      ck_assert($(condition, waitUntilDate, deadline));
    }
  });
  ck_assert(taskSuccess);

  release(deadline);

  ck_assert_int_eq(200, downloadTask->urlSessionTask.response->httpStatusCode);

  fclose(downloadTask->file);

  release(downloadTask);
  release(url);

  release(condition);

} END_TEST

START_TEST(synchronous) {

  URLSession *session = $$(URLSession, sharedInstance);
  ck_assert(session != NULL);

  URL *url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/Objectively/blob/main/README.md");
  ck_assert(url != NULL);

  URLSessionDataTask *dataTask = $(session, dataTaskWithURL, url, NULL);
  ck_assert(dataTask != NULL);

  $((URLSessionTask *) dataTask, execute);

  ck_assert_int_eq(200, dataTask->urlSessionTask.response->httpStatusCode);
  ck_assert(dataTask->data != NULL);

  release(dataTask);

  URLSessionDownloadTask *downloadTask = $(session, downloadTaskWithURL, url, NULL);
  ck_assert(downloadTask != NULL);

  downloadTask->file = fopen("/tmp/README.md", "w");
  ck_assert(downloadTask->file != NULL);

  $((URLSessionTask *) downloadTask, execute);

  ck_assert_int_eq(200, downloadTask->urlSessionTask.response->httpStatusCode);

  fclose(downloadTask->file);

  release(downloadTask);
  release(url);

} END_TEST

START_TEST(cachedDataTaskWithURL) {

  URLSessionConfiguration *configuration = $(alloc(URLSessionConfiguration), init);
  ck_assert(configuration != NULL);
  ck_assert(configuration->urlCache == NULL);

  configuration->urlCache = $(alloc(URLCache), init);
  ck_assert(configuration->urlCache != NULL);

  URLSession *session = $(alloc(URLSession), initWithConfiguration, configuration);
  ck_assert(session != NULL);

  release(configuration);

  URL *url = $(alloc(URL), initWithCharacters, "https://example.com/session-cache");
  ck_assert(url != NULL);

  URLRequest *request = $(alloc(URLRequest), initWithURL, url);
  request->httpMethod = HTTP_GET;

  URLResponse *response = $(alloc(URLResponse), init);
  response->httpStatusCode = 200;

  Data *data = $(alloc(Data), initWithBytes, (const uint8_t *) "cached", 6);
  $(session->configuration->urlCache, storeCachedResponseForRequest, request, response, data);
  release(request);

  release(data);
  release(response);

  cacheTaskDone = false;
  cacheTaskSuccess = false;

  URLSessionDataTask *dataTask = $(session, dataTaskWithURL, url, cacheCompletion);
  ck_assert(dataTask != NULL);
  ck_assert_int_eq(HTTP_GET, dataTask->urlSessionTask.request->httpMethod);

  $((URLSessionTask *) dataTask, resume);

  ck_assert(cacheTaskDone);
  ck_assert(cacheTaskSuccess);
  ck_assert_int_eq(200, dataTask->urlSessionTask.response->httpStatusCode);
  ck_assert(dataTask->data != NULL);
  ck_assert_int_eq(6, dataTask->data->length);
  ck_assert_int_eq(0, memcmp(dataTask->data->bytes, "cached", 6));

  release(dataTask);
  release(session);
  release(url);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("URLSession");
  tcase_add_test(tcase, asynchronous);
  tcase_add_test(tcase, cachedDataTaskWithURL);
  tcase_add_test(tcase, synchronous);

  Suite *suite = suite_create("URLSession");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
