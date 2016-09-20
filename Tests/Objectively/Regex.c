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
#include <stdlib.h>

#include <Objectively.h>

START_TEST(regex)
	{
		Regex *regex = $alloc(Regex, initWithPattern, "([a-z]*)://(.*)", 0);
		ck_assert(regex != NULL);

		ck_assert_int_eq(2, regex->numberOfSubExpressions);

		String *url = str("http://github.com");

		Range *matches;
		ck_assert($(regex, matchesString, url, 0, &matches));

		String *scheme = $(url, substring, matches[1]);
		ck_assert_str_eq("http", scheme->chars);

		String *host = $(url, substring, matches[2]);
		ck_assert_str_eq("github.com", host->chars);

		free(matches);
		release(scheme);
		release(host);
		release(url);
		release(regex);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("regex");
	tcase_add_test(tcase, regex);

	Suite *suite = suite_create("regex");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
