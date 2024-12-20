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

#include "Objectively.h"

static Condition *condition;

/**
 * @brief The completion handler.
 */
void completion(URLSessionTask *task, bool success) {

	ck_assert(success);

	$(condition, signal);
}

START_TEST(asynchronous) {

	URLSession *session = $$(URLSession, sharedInstance);
	ck_assert(session != NULL);

	URL *url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/Objectively");
	ck_assert(url != NULL);

	URLSessionDataTask *dataTask = $(session, dataTaskWithURL, url, completion);
	ck_assert(dataTask != NULL);

	condition = $(alloc(Condition), init);

	$((URLSessionTask *) dataTask, resume);

	$(condition, wait);

	ck_assert_int_eq(200, dataTask->urlSessionTask.response->httpStatusCode);
	ck_assert(dataTask->data != NULL);

	release(dataTask);
	release(url);

	url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/Objectively/raw/master/README.md");
	ck_assert(url != NULL);

	URLSessionDownloadTask *downloadTask = $(session, downloadTaskWithURL, url, completion);

	downloadTask->file = fopen("/tmp/README.md", "w");
	ck_assert(downloadTask->file != NULL);

	$((URLSessionTask *) downloadTask, resume);

	$(condition, wait);

	ck_assert_int_eq(200, downloadTask->urlSessionTask.response->httpStatusCode);

	fclose(downloadTask->file);

	release(downloadTask);
	release(url);

	release(condition);

} END_TEST

START_TEST(synchronous) {

	URLSession *session = $$(URLSession, sharedInstance);
	ck_assert(session != NULL);

	URL *url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/Objectively");
	ck_assert(url != NULL);

	URLSessionDataTask *dataTask = $(session, dataTaskWithURL, url, NULL);
	ck_assert(dataTask != NULL);

	$((URLSessionTask *) dataTask, execute);

	ck_assert_int_eq(200, dataTask->urlSessionTask.response->httpStatusCode);
	ck_assert(dataTask->data != NULL);

	release(dataTask);
	release(url);

	url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/Objectively/raw/master/README.md");
	ck_assert(url != NULL);

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

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("URLSession");
	tcase_add_test(tcase, asynchronous);
	tcase_add_test(tcase, synchronous);

	Suite *suite = suite_create("URLSession");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
