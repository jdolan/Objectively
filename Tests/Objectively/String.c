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

START_TEST(string)
	{
		String *string = str("hello %s!", "world");

		ck_assert(string != NULL);
		ck_assert_ptr_eq(_String(), classof(string));

		String *copy = (String *) $((Object * ) string, copy);
		ck_assert_str_eq("hello world!", copy->chars);

		ck_assert($((Object *) string, isEqual, (Object *) copy));
		ck_assert_int_eq($((Object *) string, hash), $((Object *) copy, hash));

		String *prefix = str("hello");
		ck_assert($(string, hasPrefix, prefix));

		String *suffix = str("world!");
		ck_assert($(string, hasSuffix, suffix));

		Range range = { 6, 5 };
		String *substring = $(string, substring, range);
		ck_assert_str_eq("world", substring->chars);

		Range match = $(string, rangeOfString, substring, range);
		ck_assert_int_eq(range.location, match.location);
		ck_assert_int_eq(range.length, match.length);

		String *sep = str(" ");
		Array *components = $(string, componentsSeparatedByString, sep);
		ck_assert_int_eq(2, components->count);

		for (int i = 0; i < components->count; i++) {
			String *component = $(components, objectAtIndex, i);

			switch (i) {
				case 0:
					ck_assert_str_eq("hello", component->chars);
					break;
				case 1:
					ck_assert_str_eq("world!", component->chars);
					break;
				default:
					break;
			}
		}

		String *upper = $(string, uppercaseString);
		ck_assert_str_eq("HELLO WORLD!", upper->chars);

		String *lower = $(upper, lowercaseString);
		ck_assert_str_eq("hello world!", lower->chars);

		const char *path = "/tmp/Objectively_String.test";
		ck_assert($(string, writeToFile, path, STRING_ENCODING_UTF8));

		String *fromFile = $$(String, stringWithContentsOfFile, path, STRING_ENCODING_UTF8);
		ck_assert_str_eq("hello world!", fromFile->chars);

		unlink(path);

		release(fromFile);
		release(upper);
		release(lower);
		release(sep);
		release(components);
		release(substring);
		release(prefix);
		release(suffix);
		release(copy);
		release(string);

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
