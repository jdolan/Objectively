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
#include <unistd.h>

#include <Objectively.h>

static void producer_func(Operation *dependency) {
	*((Object **) dependency->data) = $(alloc(Object), init);
}

static void consumer_func(Operation *operation) {
	ck_assert(cast(Object, *((Object **) operation->data)));
}

START_TEST(producerConsumer)
	{
		OperationQueue *queue = $(alloc(OperationQueue), init);
		ck_assert(queue);

		Object *object;

		Operation *producer = $(alloc(Operation), initWithFunction, producer_func, &object);
		Operation *consumer = $(alloc(Operation), initWithFunction, consumer_func, &object);

		$(consumer, addDependency, producer);

		$(queue, addOperation, consumer);
		$(queue, addOperation, producer);

		$(consumer, waitUntilFinished);
		ck_assert(object);

		release(object);
		release(producer);
		release(consumer);
		release(queue);

	}END_TEST


static void suspendResume_func(Operation *operation) {

	(*(int *) operation->data)++;
}

START_TEST(suspendResume)
	{
		OperationQueue *queue = $(alloc(OperationQueue), init);
		ck_assert(queue);

		queue->isSuspended = YES;
		int counter = 0;

		Operation *operation;
		for (int i = 0; i < 5; i++) {
			operation = $(alloc(Operation), initWithFunction, suspendResume_func, &counter);

			$(queue, addOperation, operation);

			release(operation);
		}

		ck_assert_int_eq(5, $(queue, operationCount));

		queue->isSuspended = NO;

		$(queue, waitUntilAllOperationsAreFinished);

		ck_assert_int_eq(0, $(queue, operationCount));

		release(queue);
	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("operation");
	tcase_add_test(tcase, producerConsumer);
	tcase_add_test(tcase, suspendResume);

	Suite *suite = suite_create("operation");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
