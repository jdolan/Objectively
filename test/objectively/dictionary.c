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

#include <objectively.h>

START_TEST(dictionary)
	{
		Dictionary *dictionary = new(Dictionary, 128);

		ck_assert(dictionary);
		ck_assert_ptr_eq(&__Dictionary, classof(dictionary));

		ck_assert_int_eq(0, dictionary->count);
		ck_assert_int_eq(9, dictionary->capacity);

		Object *objectOne = new(Object);
		Object *objectTwo = new(Object);
		Object *objectThree = new(Object);

		String *keyOne = new(String, "one");
		String *keyTwo = new(String, "two");
		String *keyThree = new(String, "three");

		$(dictionary, setObjectForKey, objectOne, keyOne);
		$(dictionary, setObjectForKey, objectTwo, keyTwo);
		$(dictionary, setObjectForKey, objectThree, keyThree);

		ck_assert_int_eq(3, dictionary->count);

		ck_assert_ptr_eq(objectOne, $(dictionary, objectForKey, keyOne));
		ck_assert_ptr_eq(objectTwo, $(dictionary, objectForKey, keyTwo));
		ck_assert_ptr_eq(objectThree, $(dictionary, objectForKey, keyThree));

		ck_assert_int_eq(2, objectOne->referenceCount);
		ck_assert_int_eq(2, objectTwo->referenceCount);
		ck_assert_int_eq(2, objectThree->referenceCount);

		$(dictionary, removeObjectForKey, keyOne);

		ck_assert_ptr_eq(NULL, $(dictionary, objectForKey, keyOne));
		ck_assert_int_eq(1, objectOne->referenceCount);
		ck_assert_int_eq(2, dictionary->count);

		$(dictionary, removeAllObjects);

		ck_assert_int_eq(0, dictionary->count);

		ck_assert_ptr_eq(NULL, $(dictionary, objectForKey, keyTwo));
		ck_assert_int_eq(1, objectTwo->referenceCount);

		ck_assert_ptr_eq(NULL, $(dictionary, objectForKey, keyThree));
		ck_assert_int_eq(1, objectThree->referenceCount);

		delete(objectOne);
		delete(objectTwo);
		delete(objectThree);

		delete(keyOne);
		delete(keyTwo);
		delete(keyThree);

		for (int i = 0; i < 1024; i++) {
			$(dictionary, setObjectForKey, new(Object), new(String, "%d", i));
		}

		int counter = 0;

		BOOL enumerator(const Dictionary *dictionary, id obj, id key, id data) {
			release(obj);
			release(key);
			counter++;
			return NO;
		}

		$(dictionary, enumerateObjectsAndKeys, enumerator, NULL);

		ck_assert_int_eq(1024, counter);

		$(dictionary, removeAllObjects);

		ck_assert_int_eq(dictionary->count, 0);

		delete(dictionary);

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
