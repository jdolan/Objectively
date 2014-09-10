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

START_TEST(array)
	{
		Array *array = new(Array, 5);

		ck_assert(array);
		ck_assert_ptr_eq(&__Array, classof(array));

		ck_assert_int_eq(0, array->count);
		ck_assert_int_eq(5, array->capacity);

		Object *one = new(Object);
		Object *two = new(Object);
		Object *three = new(Object);

		$(array, addObject, one);
		$(array, addObject, two);
		$(array, addObject, three);

		ck_assert_int_eq(3, array->count);

		ck_assert($(array, containsObject, one));
		ck_assert($(array, containsObject, two));
		ck_assert($(array, containsObject, three));

		ck_assert_int_eq(0, $(array, indexOfObject, one));
		ck_assert_int_eq(1, $(array, indexOfObject, two));
		ck_assert_int_eq(2, $(array, indexOfObject, three));

		ck_assert_int_eq(2, one->referenceCount);
		ck_assert_int_eq(2, two->referenceCount);
		ck_assert_int_eq(2, three->referenceCount);

		$(array, removeObject, one);

		ck_assert(!$(array, containsObject, one));
		ck_assert_int_eq(1, one->referenceCount);
		ck_assert_int_eq(2, array->count);

		$(array, removeAllObjects);

		ck_assert_int_eq(0, array->count);

		ck_assert(!$(array, containsObject, two));
		ck_assert_int_eq(1, two->referenceCount);

		ck_assert(!$(array, containsObject, three));
		ck_assert_int_eq(1, three->referenceCount);

		release(one);
		release(two);
		release(three);

		for (int i = 0; i < 1024; i++) {
			$(array, addObject, new(Object));
		}

		int counter = 0;

		BOOL enumerator(const Array *array, id obj, id data) {
			release(obj); counter++; return NO;
		}

		$(array, enumerateObjects, enumerator, NULL);

		ck_assert_int_eq(1024, counter);

		$(array, removeAllObjects);

		ck_assert_int_eq(array->count, 0);

		release(array);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("array");
	tcase_add_test(tcase, array);

	Suite *suite = suite_create("array");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
