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
#include <stdio.h>
#include <unistd.h>

#include <Objectively/Condition.h>
#include <Objectively/Thread.h>

static Condition *condition;

static ident increment(Thread *self) {

	_Bool stop = false;

	while (!stop) {

		synchronized(condition, {
			if (++(*(int *) self->data) == 0xbeaf) {
				$(condition, signal);
				stop = true;
			}
		});
	}

	return (ident) true;
}

START_TEST(thread)
	{
		condition = $(alloc(Condition), init);
		ck_assert(condition != NULL);

		int criticalSection = 0;

		Thread *thread = $(alloc(Thread), initWithFunction, increment, &criticalSection);
		ck_assert(thread != NULL);

		$(thread, start);

		synchronized(condition, $(condition, wait));
		ck_assert_int_eq(0xbeaf, criticalSection);

		ident ret;
		$(thread, join, &ret);
		ck_assert_int_eq(true, (_Bool ) ret);

		release(thread);
		release(condition);

	}END_TEST

static ident signalBeforeDate(Thread *self) {

	usleep(((Date *) self->data)->time.tv_usec / 2);

	synchronized(condition, $(condition, signal));

	return NULL;
}

START_TEST(cond)
	{
		condition = $(alloc(Condition), init);
		ck_assert(condition != NULL);

		const Time time = { .tv_usec = MSEC_PER_SEC * 0.5 };
		Date *date = $$(Date, dateWithTimeSinceNow, &time);
		ck_assert(date != NULL);

		synchronized(condition, {
			ck_assert($(condition, waitUntilDate, date) == false);
		});

		release(date);

		date = $$(Date, dateWithTimeSinceNow, &time);
		ck_assert(date != NULL);

		Thread *thread = $(alloc(Thread), initWithFunction, signalBeforeDate, date);
		ck_assert(thread != NULL);

		$(thread, start);

		synchronized(condition, {
			ck_assert($(condition, waitUntilDate, date));
		});

		$(thread, join, NULL);

		release(date);
		release(thread);
		release(condition);
	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("thread");
	tcase_add_test(tcase, thread);
	tcase_add_test(tcase, cond);

	Suite *suite = suite_create("thread");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
