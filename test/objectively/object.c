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

#include <objectively.h>

START_TEST(object)
	{
		Object *object = new(Object);

		ck_assert(object);
		ck_assert_ptr_eq(&__Object, classof(object));

		ck_assert($(object, isEqual, object));
		ck_assert($(object, isKindOfClass, &__Object));

		Object *copy = $(object, copy);

		ck_assert(copy);
		ck_assert_ptr_eq(&__Object, classof(object));

		ck_assert($(copy, isKindOfClass, &__Object));
		ck_assert(!$(copy, isEqual, object));

		release(copy);
		release(object);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("object");
	tcase_add_test(tcase, object);

	Suite *suite = suite_create("object");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
