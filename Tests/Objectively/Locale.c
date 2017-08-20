/*
 * Objectively: Ultra-lightweight locale oriented framework for GNU C.
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

START_TEST(locale)
	{
		Locale *C = $$(Locale, currentLocale);

		ck_assert(C != NULL);
		ck_assert_ptr_eq(_Locale(), classof(C));

		Locale *en_US = $(alloc(Locale), initWithIdentifier, "en_US");

		ck_assert(en_US != NULL);
		ck_assert_ptr_eq(_Locale(), classof(en_US));

		release(C);
		release(en_US);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("locale");
	tcase_add_test(tcase, locale);

	Suite *suite = suite_create("locale");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
