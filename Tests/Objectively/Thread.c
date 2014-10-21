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

Condition *condition;

static id run(Thread *self) {

	BOOL stop = NO;
	while (!stop) {

		$((Lock * ) condition, lock);

		if (++(*(int *) self->data) == 0xbeaf) {
			$(condition, signal);
			stop = YES;
		}

		$((Lock * ) condition, unlock);
	}

	return (id) YES;
}

START_TEST(thread)
	{
		condition = new(Condition);
		ck_assert(condition);

		$((Lock * ) condition, lock);

		int criticalSection = 0;

		Thread *thread = new(Thread, run, &criticalSection);
		ck_assert(thread);

		$(thread, start);

		$(condition, wait);
		ck_assert_int_eq(0xbeaf, criticalSection);

		id ret;
		$(thread, join, &ret);
		ck_assert_int_eq(YES, (BOOL) ret);

		release(thread);
		release(condition);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("thread");
	tcase_add_test(tcase, thread);

	Suite *suite = suite_create("thread");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
