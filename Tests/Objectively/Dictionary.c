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

static BOOL enumerator(const Dictionary *dictionary, id obj, id key, id data) {

	(* (int *) data)++;

	release(obj);
	release(key);

	return NO;
}

START_TEST(dictionary)
	{
		Dictionary *dict = $(Dictionary, alloc(Dictionary), initWithCapacity, 128);

		ck_assert(dict);
		ck_assert_ptr_eq(&__Dictionary, classof(dict));

		ck_assert_int_eq(0, dict->count);
		ck_assert_int_eq(128, dict->capacity);

		Object *objectOne = alloc(Object);
		Object *objectTwo = alloc(Object);
		Object *objectThree = alloc(Object);

		String *keyOne = $(String, alloc(String), initWithFormat, "one");
		String *keyTwo = $(String, alloc(String), initWithFormat, "two");
		String *keyThree = $(String, alloc(String), initWithFormat, "three");

		$(Dictionary, dict, setObjectForKey, objectOne, keyOne);
		$(Dictionary, dict, setObjectForKey, objectTwo, keyTwo);
		$(Dictionary, dict, setObjectForKey, objectThree, keyThree);

		ck_assert_int_eq(3, dict->count);

		ck_assert_ptr_eq(objectOne, $(Dictionary, dict, objectForKey, keyOne));
		ck_assert_ptr_eq(objectTwo, $(Dictionary, dict, objectForKey, keyTwo));
		ck_assert_ptr_eq(objectThree, $(Dictionary, dict, objectForKey, keyThree));

		ck_assert_int_eq(2, objectOne->referenceCount);
		ck_assert_int_eq(2, objectTwo->referenceCount);
		ck_assert_int_eq(2, objectThree->referenceCount);

		$(Dictionary, dict, removeObjectForKey, keyOne);

		ck_assert_ptr_eq(NULL, $(Dictionary, dict, objectForKey, keyOne));
		ck_assert_int_eq(1, objectOne->referenceCount);
		ck_assert_int_eq(2, dict->count);

		$(Dictionary, dict, removeAllObjects);

		ck_assert_int_eq(0, dict->count);

		ck_assert_ptr_eq(NULL, $(Dictionary, dict, objectForKey, keyTwo));
		ck_assert_int_eq(1, objectTwo->referenceCount);

		ck_assert_ptr_eq(NULL, $(Dictionary, dict, objectForKey, keyThree));
		ck_assert_int_eq(1, objectThree->referenceCount);

		release(objectOne);
		release(objectTwo);
		release(objectThree);

		release(keyOne);
		release(keyTwo);
		release(keyThree);

		for (int i = 0; i < 1024; i++) {

			Object *object = $(Object, alloc(Object), init);
			String *key = $(String, alloc(String), initWithFormat, "%d", i);

			$(Dictionary, dict, setObjectForKey, object, key);

			//release(object);
			//release(key);
		}

		int counter = 0;

		$(Dictionary, dict, enumerateObjectsAndKeys, enumerator, &counter);

		ck_assert_int_eq(1024, counter);

		$(Dictionary, dict, removeAllObjects);

		ck_assert_int_eq(dict->count, 0);

		release(dict);

	}END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("dictionary");
	tcase_add_test(tcase, dictionary);

	Suite *suite = suite_create("dictionary");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
