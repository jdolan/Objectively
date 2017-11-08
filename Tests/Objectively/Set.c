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

static void enumerator(const Set *set, ident obj, ident data) {
	(*(int *) data)++;
}

static _Bool filter(ident obj, ident data) {
	return obj == data;
}

static ident functor(const ident obj, ident data) {
	return $((Object *) obj, copy);
}

static ident reducer(const ident obj, ident accumulator, ident data) {
	return (ident) (intptr_t) accumulator + 1;
}

START_TEST(set)
	{
		Object *one = $(alloc(Object), init);
		Object *two = $(alloc(Object), init);
		Object *three = $(alloc(Object), init);

		Set *set = $$(Set, setWithObjects, one, one, two, two, three, three, NULL);

		ck_assert(set != NULL);
		ck_assert_ptr_eq(_Set(), classof(set));

		ck_assert_int_eq(3, set->count);

		ck_assert($(set, containsObject, one));
		ck_assert($(set, containsObject, two));
		ck_assert($(set, containsObject, three));

		ck_assert_int_eq(2, one->referenceCount);
		ck_assert_int_eq(2, two->referenceCount);
		ck_assert_int_eq(2, three->referenceCount);

		release(one);
		release(two);
		release(three);

		int count = 0;
		$(set, enumerateObjects, enumerator, &count);

		ck_assert_int_eq(set->count, count);

		Set *filtered = $(set, filteredSet, filter, two);

		ck_assert_int_eq(1, filtered->count);
		ck_assert($(filtered, containsObject, two));

		Set *mapped = $(set, mappedSet, functor, NULL);

		ck_assert_int_eq(set->count, mapped->count);

		int reduced = (int) (intptr_t) $(set, reduce, reducer, (ident) 0, NULL);
		ck_assert_int_eq(3, reduced);

		release(mapped);
		release(filtered);
		release(set);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("set");
	tcase_add_test(tcase, set);

	Suite *suite = suite_create("set");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
