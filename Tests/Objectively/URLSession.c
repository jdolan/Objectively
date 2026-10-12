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
#include <time.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

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

/**
 * @brief Opens a listening socket on the loopback interface that accepts connections, through
 * its backlog, but never responds to them.
 * @return The socket, and its URL in `url`.
 */
static int silentServer(char *url, size_t size) {

  const int sock = socket(AF_INET, SOCK_STREAM, 0);
  ck_assert_int_ge(sock, 0);

  struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_addr.s_addr = htonl(INADDR_LOOPBACK),
  };

  ck_assert_int_eq(0, bind(sock, (struct sockaddr *) &addr, sizeof(addr)));
  ck_assert_int_eq(0, listen(sock, 8));

  socklen_t len = sizeof(addr);
  ck_assert_int_eq(0, getsockname(sock, (struct sockaddr *) &addr, &len));

  snprintf(url, size, "http://127.0.0.1:%d/", ntohs(addr.sin_port));
  return sock;
}

static double now(void) {

  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec + ts.tv_nsec / 1e9;
}

static ident invalidateLater(Thread *thread) {

  usleep(500000);
  $((URLSession *) thread->data, invalidateAndCancel);
  return NULL;
}

START_TEST(executeCancel) {

  char address[64];
  const int sock = silentServer(address, sizeof(address));

  URLSession *session = $(alloc(URLSession), init);
  URL *url = $(alloc(URL), initWithCharacters, address);

  URLSessionDataTask *task = $(session, dataTaskWithURL, url, NULL);
  ck_assert(task != NULL);

  Thread *thread = $(alloc(Thread), initWithFunction, invalidateLater, session);
  $(thread, start);

  const double start = now();
  $((URLSessionTask *) task, execute);
  const double elapsed = now() - start;

  ck_assert_int_eq(URLSESSIONTASK_CANCELED, task->urlSessionTask.state);
  ck_assert(elapsed < 3.0);

  $(thread, join, NULL);

  release(thread);
  release(task);
  release(url);
  release(session);

  close(sock);

} END_TEST

START_TEST(resumeCancel) {

  char address[64];
  const int sock = silentServer(address, sizeof(address));

  condition = $(alloc(Condition), init);
  taskDone = false;
  taskSuccess = true;

  URLSession *session = $(alloc(URLSession), init);
  URL *url = $(alloc(URL), initWithCharacters, address);

  URLSessionDataTask *task = $(session, dataTaskWithURL, url, completion);
  ck_assert(task != NULL);

  $((URLSessionTask *) task, resume);

  usleep(200000);

  $((URLSessionTask *) task, cancel);

  const Time timeout = { .tv_sec = 3 };
  Date *date = $$(Date, dateWithTimeSinceNow, &timeout);

  synchronized(condition, {
    while (taskDone == false) {
      ck_assert($(condition, waitUntilDate, date));
    }
  });

  ck_assert(taskSuccess == false);
  ck_assert_int_eq(URLSESSIONTASK_CANCELED, task->urlSessionTask.state);

  release(date);
  release(task);
  release(url);
  release(session);
  release(condition);

  close(sock);

} END_TEST

START_TEST(suspendCancel) {

  char address[64];
  const int sock = silentServer(address, sizeof(address));

  URLSession *session = $(alloc(URLSession), init);
  URL *url = $(alloc(URL), initWithCharacters, address);

  URLSessionDataTask *task = $(session, dataTaskWithURL, url, NULL);
  ck_assert(task != NULL);

  $((URLSessionTask *) task, resume);
  usleep(200000);
  $((URLSessionTask *) task, suspend);

  const double start = now();
  release(session);
  ck_assert(now() - start < 3.0);

  ck_assert_int_eq(URLSESSIONTASK_CANCELED, task->urlSessionTask.state);

  release(task);
  release(url);

  close(sock);

} END_TEST

static int restStatus;
static Data *restData;

static void restCompletion(int status, Data *data, void *userData) {

  restStatus = status;
  restData = data;
}

START_TEST(invalidated) {

  URLSession *session = $(alloc(URLSession), init);
  $(session, invalidateAndCancel);

  URL *url = $(alloc(URL), initWithCharacters, "http://127.0.0.1:1/");
  ck_assert($(session, dataTaskWithURL, url, NULL) == NULL);
  ck_assert($(session, downloadTaskWithURL, url, NULL) == NULL);

  RESTClient *client = $(alloc(RESTClient), initWithSession, session);

  Data *data = (Data *) 0x1;
  ck_assert_int_eq(0, $(client, get, "http://127.0.0.1:1/", NULL, &data));
  ck_assert(data == NULL);

  restStatus = -1;
  restData = (Data *) 0x1;
  $(client, getAsync, "http://127.0.0.1:1/", NULL, restCompletion, NULL);
  ck_assert_int_eq(0, restStatus);
  ck_assert(restData == NULL);

  release(client);
  release(url);
  release(session);

} END_TEST

START_TEST(restClientCancel) {

  char address[64];
  const int sock = silentServer(address, sizeof(address));

  URLSession *session = $(alloc(URLSession), init);
  RESTClient *client = $(alloc(RESTClient), initWithSession, session);

  Thread *thread = $(alloc(Thread), initWithFunction, invalidateLater, session);
  $(thread, start);

  Data *data = NULL;
  const double start = now();
  const int status = $(client, get, address, NULL, &data);
  const double elapsed = now() - start;

  ck_assert_int_eq(0, status);
  ck_assert(data == NULL);
  ck_assert(elapsed < 3.0);

  $(thread, join, NULL);

  release(thread);
  release(client);
  release(session);

  close(sock);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("URLSession");
  tcase_set_timeout(tcase, 30);
  tcase_add_test(tcase, asynchronous);
  tcase_add_test(tcase, cachedDataTaskWithURL);
  tcase_add_test(tcase, executeCancel);
  tcase_add_test(tcase, invalidated);
  tcase_add_test(tcase, restClientCancel);
  tcase_add_test(tcase, resumeCancel);
  tcase_add_test(tcase, suspendCancel);
  tcase_add_test(tcase, synchronous);

  Suite *suite = suite_create("URLSession");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
