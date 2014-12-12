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

#include <Objectively/MutableDictionary.h>
#include <Objectively/String.h>

static BOOL enumerator(const Dictionary *dictionary, id obj, id key, id data) {

	(* (int *) data)++;

	return NO;
}

START_TEST(dictionary)
	{
		MutableDictionary *dict = $(alloc(MutableDictionary), initWithCapacity, 4);

		ck_assert(dict);
		ck_assert_ptr_eq(&__MutableDictionary, classof(dict));

		ck_assert_int_eq(0, ((Dictionary *) dict)->count);
		ck_assert_int_eq(4, ((Dictionary *) dict)->capacity);

		Object *objectOne = alloc(Object);
		Object *objectTwo = alloc(Object);
		Object *objectThree = alloc(Object);

		String *keyOne = $(alloc(String), initWithFormat, "one");
		String *keyTwo = $(alloc(String), initWithFormat, "two");
		String *keyThree = $(alloc(String), initWithFormat, "three");

		$(dict, setObjectForKey, objectOne, keyOne);
		$(dict, setObjectForKey, objectTwo, keyTwo);
		$(dict, setObjectForKey, objectThree, keyThree);

		ck_assert_int_eq(3, ((Dictionary *) dict)->count);

		ck_assert_ptr_eq(objectOne, $((Dictionary *) dict, objectForKey, keyOne));
		ck_assert_ptr_eq(objectTwo, $((Dictionary *) dict, objectForKey, keyTwo));
		ck_assert_ptr_eq(objectThree, $((Dictionary *) dict, objectForKey, keyThree));

		ck_assert_int_eq(2, objectOne->referenceCount);
		ck_assert_int_eq(2, objectTwo->referenceCount);
		ck_assert_int_eq(2, objectThree->referenceCount);

		$(dict, removeObjectForKey, keyOne);

		ck_assert_ptr_eq(NULL, $((Dictionary *) dict, objectForKey, keyOne));
		ck_assert_int_eq(1, objectOne->referenceCount);
		ck_assert_int_eq(2, ((Dictionary *) dict)->count);

		$(dict, removeAllObjects);

		ck_assert_int_eq(0, ((Dictionary *) dict)->count);

		ck_assert_ptr_eq(NULL, $((Dictionary *) dict, objectForKey, keyTwo));
		ck_assert_int_eq(1, objectTwo->referenceCount);

		ck_assert_ptr_eq(NULL, $((Dictionary *) dict, objectForKey, keyThree));
		ck_assert_int_eq(1, objectThree->referenceCount);

		release(objectOne);
		release(objectTwo);
		release(objectThree);

		release(keyOne);
		release(keyTwo);
		release(keyThree);

		for (int i = 0; i < 1024; i++) {

			Object *object = $(alloc(Object), init);
			String *key = $(alloc(String), initWithFormat, "%d", i);

			$(dict, setObjectForKey, object, key);

			release(object);
			release(key);
		}

		int counter = 0;

		$((Dictionary *) dict, enumerateObjectsAndKeys, enumerator, &counter);

		ck_assert_int_eq(1024, counter);

		$(dict, removeAllObjects);

		ck_assert_int_eq(((Dictionary *) dict)->count, 0);

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
