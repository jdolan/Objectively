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
#include <stdio.h>

#include <Objectively.h>

START_TEST(_log)
	{
		Log *log = new(Log, "test", DEBUG);
		ck_assert(log);

		ck_assert_str_eq("test", log->name);
		ck_assert_int_eq(DEBUG, log->level);

		log->file = fopen("/tmp/objectively-test.log", "w");
		ck_assert(log->file);
		ck_assert_int_eq(0, ftell(log->file));

		$(log, debug, "hello %s", "world!");
		$(log, flush);

		long int len = ftell(log->file);
		ck_assert_int_gt(len, 0);

		$(log, trace, "hello again");
		$(log, flush);

		long int len2 = ftell(log->file);
		ck_assert_int_eq(len, len2);

		release(log);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("log");
	tcase_add_test(tcase, _log);

	Suite *suite = suite_create("log");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
