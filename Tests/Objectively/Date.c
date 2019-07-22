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

START_TEST(date) {
	DateFormatter *dateFormatter = $(alloc(DateFormatter), initWithFormat, NULL);

	ck_assert(dateFormatter != NULL);
	ck_assert_ptr_eq(_DateFormatter(), classof(dateFormatter));

	ck_assert_str_eq(DATEFORMAT_ISO8601, dateFormatter->fmt);

	String *dateString1 = str("1980-06-24T10:37:00-0400");
	String *dateString2 = str("1985-03-15T06:48:00-0500");

	Date *date1 = $(dateFormatter, dateFromString, dateString1);
	Date *date2 = $(dateFormatter, dateFromString, dateString2);

	ck_assert(date1 != NULL);
	ck_assert(date2 != NULL);

	ck_assert($(date1, compareTo, date1) == OrderSame);
	ck_assert($(date1, compareTo, date2) == OrderAscending);

	Time time = $(date1, timeSinceDate, date2);
	ck_assert(time.tv_sec < 0.0);

	$(dateFormatter, initWithFormat, "%B");

	String *string = $(dateFormatter, stringFromDate, date1);
	ck_assert_str_eq("June", string->chars);

	release(dateString1);
	release(dateString2);
	release(string);
	release(date1);
	release(date2);
	release(dateFormatter);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Date");
	tcase_add_test(tcase, date);

	Suite *suite = suite_create("Date");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
