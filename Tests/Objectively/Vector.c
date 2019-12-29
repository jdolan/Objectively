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

typedef struct {
	int bar;
} Foo;

START_TEST(vector) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	ck_assert_ptr_ne(NULL, vector);
	ck_assert_ptr_eq(_Vector(), classof(vector));

	ck_assert_int_eq(0, ((Array *) vector)->count);

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	ck_assert_int_eq(3, vector->count);

	$(vector, removeElementAtIndex, 0);

	ck_assert_int_eq(2, vector->count);
	ck_assert_int_eq(2, ((Foo *) vector->elements)[0].bar);
	ck_assert_int_eq(3, ((Foo *) vector->elements)[1].bar);

	$(vector, insertElementAtIndex, &one, 0);

	ck_assert_int_eq(1, ((Foo *) vector->elements)[0].bar);
	ck_assert_int_eq(2, ((Foo *) vector->elements)[1].bar);
	ck_assert_int_eq(3, ((Foo *) vector->elements)[2].bar);

	release(vector);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Vector");
	tcase_add_test(tcase, vector);

	Suite *suite = suite_create("Vector");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
