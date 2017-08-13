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

START_TEST(indexSet)
	{
		size_t indexes[] = { 1, 2, 3, 3, 2 };
		IndexSet *indexSet = $(alloc(IndexSet), initWithIndexes, indexes, lengthof(indexes));

		ck_assert(indexSet != NULL);
		ck_assert_int_eq(3, indexSet->count);
		ck_assert($(indexSet, containsIndex, 1));
		ck_assert($(indexSet, containsIndex, 2));
		ck_assert($(indexSet, containsIndex, 3));

		String *description = $((Object *) indexSet, description);
		ck_assert_str_eq("[1, 2, 3]", description->chars);

		Object *object = (Object *) indexSet;
		Object *copy = $(object, copy);

		ck_assert(copy != NULL);
		ck_assert_int_eq($(object, hash), $(copy, hash));
		ck_assert($(object, isEqual, copy));

		release(description);
		release(copy);
		release(indexSet);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("indexSet");
	tcase_add_test(tcase, indexSet);

	Suite *suite = suite_create("indexSet");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
