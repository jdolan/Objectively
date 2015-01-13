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

START_TEST(number)
	{
		Number *number1 = $(alloc(Number), initWithValue, 1.001);
		ck_assert(number != NULL);

		const float f = $(number1, floatValue);
		ck_assert(f > 1.0009 && f < 1.0011);
		ck_assert(1 == $(number1, intValue));
		ck_assert(YES == $(number1, boolValue));

		Number *number2 = $(alloc(Number), initWithValue, 1.001);
		ck_assert(number2 != NULL);

		ck_assert_int_eq($((Object *) number1, hash), $((Object *) number2, hash));
		ck_assert($((Object *) number1, isEqual, (Object *) number2));

		release(number1);
		release(number2);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("number");
	tcase_add_test(tcase, number);

	Suite *suite = suite_create("number");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
