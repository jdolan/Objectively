/*
 * Objectively: Ultra-lightweight object oriented framework for c99.
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

#include <Objectively.h>

static Condition *condition;

/**
 * @brief The completion handler.
 */
void completion(URLSessionTask *task, BOOL success) {

	ck_assert(success);

	$(condition, signal);
}

START_TEST(urlSession)
	{
		URLSession *session = $(alloc(URLSession), init);
		ck_assert(session);

		URL *url = $(alloc(URL), initWithCharacters, "https://github.com/jdolan/objectively");
		ck_assert(url);

		URLSessionDataTask *task = $(session, dataTaskWithURL, url);
		ck_assert(task);

		task->urlSessionTask.completion = completion;

		condition = $(alloc(Condition), init);

		$((URLSessionTask *) task, resume);

		$(condition, wait);

		release(condition);
		release(task);
		release(url);
		release(session);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("urlSession");
	tcase_add_test(tcase, urlSession);

	Suite *suite = suite_create("urlSession");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
