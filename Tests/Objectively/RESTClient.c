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

/**
 * @brief Subset of the httpbin.org GET/HEAD response.
 */
typedef struct {
  char url[256];
} GetResponse;

/**
 * @brief A simple JSON payload sent as a request body.
 */
typedef struct {
  char name[128];
  int32_t count;
} Payload;

/**
 * @brief httpbin.org echo response for body-bearing verbs (POST, PATCH, …).
 * The `json` field is the echoed deserialized request body.
 */
typedef struct {
  Payload json;
  char url[256];
} EchoResponse;

static const JSONProperties get_response_properties = MakeJSONProperties(GetResponse,
  MakeJSONProperty(GetResponse, url, NULL, JSONDeserializeCharacters, NULL)
);

static const JSONProperties payload_properties = MakeJSONProperties(Payload,
  MakeJSONProperty(Payload, count, JSONSerializeInt32,      JSONDeserializeInt32,      NULL),
  MakeJSONProperty(Payload, name,  JSONSerializeCharacters, JSONDeserializeCharacters, NULL)
);

static const JSONProperties echo_response_properties = MakeJSONProperties(EchoResponse,
  MakeJSONProperty(EchoResponse, json, NULL, JSONDeserializeStruct,     (ident) &payload_properties),
  MakeJSONProperty(EchoResponse, url,  NULL, JSONDeserializeCharacters, NULL)
);

#define HTTPBIN         "https://httpbin.org"
#define HTTPBIN_GET     HTTPBIN "/get"
#define HTTPBIN_POST    HTTPBIN "/post"
#define HTTPBIN_PATCH   HTTPBIN "/patch"

START_TEST(get) {

  RESTClient *client = $(alloc(RESTClient), init);

  Data *data = NULL;
  const int32_t status = $(client, get, HTTPBIN_GET, &data);

  ck_assert_int_eq(200, status);
  ck_assert_ptr_ne(NULL, data);

  GetResponse response = {0};
  JSONContext *ctx = $(alloc(JSONContext), init);
  ck_assert($(ctx, structFromData, &get_response_properties, data, &response));
  release(ctx);
  release(data);

  ck_assert_str_eq(HTTPBIN_GET, response.url);

  release(client);

} END_TEST

START_TEST(post) {

  RESTClient *client = $(alloc(RESTClient), init);

  Payload payload = { .name = "Objectively", .count = 42 };

  JSONContext *ctx = $(alloc(JSONContext), init);
  Data *body = $(ctx, dataFromStruct, &payload_properties, &payload);
  release(ctx);
  ck_assert_ptr_ne(NULL, body);

  Data *data = NULL;
  const int32_t status = $(client, post, HTTPBIN_POST, body, &data);
  release(body);

  ck_assert_int_eq(200, status);
  ck_assert_ptr_ne(NULL, data);

  EchoResponse response = {0};
  ctx = $(alloc(JSONContext), init);
  ck_assert($(ctx, structFromData, &echo_response_properties, data, &response));
  release(ctx);
  release(data);

  ck_assert_str_eq("Objectively", response.json.name);
  ck_assert_int_eq(42, response.json.count);
  ck_assert_str_eq(HTTPBIN_POST, response.url);

  release(client);

} END_TEST

START_TEST(patch) {

  RESTClient *client = $(alloc(RESTClient), init);

  Payload payload = { .name = "Objectively", .count = 7 };

  JSONContext *ctx = $(alloc(JSONContext), init);
  Data *body = $(ctx, dataFromStruct, &payload_properties, &payload);
  release(ctx);
  ck_assert_ptr_ne(NULL, body);

  Data *data = NULL;
  const int32_t status = $(client, patch, HTTPBIN_PATCH, body, &data);
  release(body);

  ck_assert_int_eq(200, status);
  ck_assert_ptr_ne(NULL, data);

  EchoResponse response = {0};
  ctx = $(alloc(JSONContext), init);
  ck_assert($(ctx, structFromData, &echo_response_properties, data, &response));
  release(ctx);
  release(data);

  ck_assert_str_eq("Objectively", response.json.name);
  ck_assert_int_eq(7, response.json.count);
  ck_assert_str_eq(HTTPBIN_PATCH, response.url);

  release(client);

} END_TEST

static Condition *condition;
static int32_t async_status;
static char async_url[256];

static void getAsync_completion(int32_t status, Data *data, void *user_data) {

  async_status = status;

  if (data) {
    GetResponse response = {0};
    JSONContext *ctx = $(alloc(JSONContext), init);
    $(ctx, structFromData, &get_response_properties, data, &response);
    release(ctx);
    strncpy(async_url, response.url, sizeof(async_url) - 1);
  }

  synchronized(condition, {
    $(condition, signal);
  });
}

START_TEST(getAsync) {

  condition = $(alloc(Condition), init);

  RESTClient *client = $(alloc(RESTClient), init);

  async_status = 0;
  async_url[0] = '\0';

  const Time timeout = { .tv_sec = 30 };
  Date *deadline = $$(Date, dateWithTimeSinceNow, &timeout);

  synchronized(condition, {
    $(client, getAsync, HTTPBIN_GET, getAsync_completion, NULL);
    ck_assert($(condition, waitUntilDate, deadline));
  });

  release(deadline);
  release(client);
  release(condition);

  ck_assert_int_eq(200, async_status);
  ck_assert_str_eq(HTTPBIN_GET, async_url);

} END_TEST

START_TEST(head) {

  RESTClient *client = $(alloc(RESTClient), init);

  const int32_t status = $(client, head, HTTPBIN_GET);
  ck_assert_int_eq(200, status);

  release(client);

} END_TEST

START_TEST(options) {

  RESTClient *client = $(alloc(RESTClient), init);

  Data *data = NULL;
  const int32_t status = $(client, options, HTTPBIN_GET, &data);
  ck_assert(status == 200 || status == 204);

  release(data);
  release(client);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("RESTClient");
  tcase_set_timeout(tcase, 30);
  tcase_add_test(tcase, get);
  tcase_add_test(tcase, post);
  tcase_add_test(tcase, patch);
  tcase_add_test(tcase, getAsync);
  tcase_add_test(tcase, head);
  tcase_add_test(tcase, options);

  Suite *suite = suite_create("RESTClient");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
