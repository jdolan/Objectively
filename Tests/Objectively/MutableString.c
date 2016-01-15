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

START_TEST(string)
	{
		MutableString *string = $$(MutableString, string);

		ck_assert(string != NULL);
		ck_assert_ptr_eq(&_MutableString, classof(string));

		String *hello = str("hello");

		$(string, appendString, hello);
		ck_assert_str_eq("hello", string->string.chars);

		$(string, appendFormat, " %s", "world!");
		ck_assert_str_eq("hello world!", string->string.chars);

		String *goodbye = str("goodbye cruel");

		Range range = { 0, 5 };
		$(string, replaceCharactersInRange, range, goodbye);
		ck_assert_str_eq("goodbye cruel world!", string->string.chars);

		String *copy = (String *) $((Object * ) string, copy);
		ck_assert(classof(copy) == &_MutableString);
		ck_assert($((Object *) string, isEqual, (Object *) copy));

		release(hello);
		release(goodbye);
		release(string);
		release(copy);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("string");
	tcase_add_test(tcase, string);

	Suite *suite = suite_create("string");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
