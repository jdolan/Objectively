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

#include <Objectively.h>

START_TEST(url) {

	URL *url = $(alloc(URL), initWithCharacters, "http://example.com");
	ck_assert(url != NULL);

	ck_assert_str_eq("http", url->scheme->chars);
	ck_assert_str_eq("example.com", url->host->chars);

	release(url);

	url = $(alloc(URL), initWithCharacters, "http://example.com:80/path?query#fragment");
	ck_assert(url != NULL);

	ck_assert_str_eq("http", url->scheme->chars);
	ck_assert_str_eq("example.com", url->host->chars);
	ck_assert_int_eq(80, url->port);
	ck_assert_str_eq("/path", url->path->chars);
	ck_assert_str_eq("query", url->query->chars);
	ck_assert_str_eq("fragment", url->fragment->chars);

	release(url);

	url = $(alloc(URL), initWithCharacters, "http://example.com:80/path#fragment");
	ck_assert(url != NULL);

	ck_assert_str_eq("http", url->scheme->chars);
	ck_assert_str_eq("example.com", url->host->chars);
	ck_assert_int_eq(80, url->port);
	ck_assert_str_eq("/path", url->path->chars);
	ck_assert(url->query == NULL);
	ck_assert_str_eq("fragment", url->fragment->chars);

	release(url);

	url = $(alloc(URL), initWithCharacters, "http://example.com?query");
	ck_assert(url != NULL);

	ck_assert_str_eq("http", url->scheme->chars);
	ck_assert_str_eq("example.com", url->host->chars);
	ck_assert_int_eq(0, url->port);
	ck_assert(url->path == NULL);
	ck_assert_str_eq("query", url->query->chars);
	ck_assert(url->fragment == NULL);

	release(url);

	url = $(alloc(URL), initWithCharacters, "file:///path");
	ck_assert(url != NULL);

	ck_assert_str_eq("file", url->scheme->chars);
	ck_assert(NULL == url->host);
	ck_assert_int_eq(0, url->port);
	ck_assert_str_eq("/path", url->path->chars);
	ck_assert(url->query == NULL);
	ck_assert(url->fragment == NULL);

	release(url);

	url = $(alloc(URL), initWithCharacters, "malformed");
	ck_assert(url == NULL);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("URL");
	tcase_add_test(tcase, url);

	Suite *suite = suite_create("URL");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
