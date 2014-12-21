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

#include <unistd.h>
#include <check.h>

#include <Objectively.h>

START_TEST(data)
	{
		Data *data1 = $(alloc(Data), initWithBytes, (byte *) "abcdef", 6);
		Data *data2 = $(alloc(Data), initWithBytes, (byte *) "ghijkl", 6);

		ck_assert(data1);
		ck_assert(data2);

		ck_assert_int_eq(6, data1->length);

		ck_assert($((Object *) data1, isEqual, (Object *) data2) == NO);
		release(data2);

		data2 = (Data *) $((Object * ) data1, copy);
		ck_assert(data2);

		ck_assert($((Object *) data1, isEqual, (Object *) data2) == YES);
		release(data2);

		const char *path = "/tmp/Objectively_Data.test";
		ck_assert($(data1, writeToFile, path) == YES);

		data2 = $(alloc(Data), initWithContentsOfFile, path);

		unlink(path);

		ck_assert($((Object *) data1, isEqual, (Object *) data2) == YES);

		release(data1);
		release(data2);
	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("data");
	tcase_add_test(tcase, data);

	Suite *suite = suite_create("data");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
