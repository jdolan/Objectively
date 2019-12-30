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

#include "Objectively.h"

static void enumerator(const Dictionary *dictionary, ident obj, ident key, ident data) {
	(* (int *) data)++;
}

static _Bool predicate(ident obj, ident key, ident data) {
	return strcmp("two", ((String *) key)->chars) == 0;
}

START_TEST(dictionary) {

	Object *objectOne = $(alloc(Object), init);
	Object *objectTwo = $(alloc(Object), init);
	Object *objectThree = $(alloc(Object), init);

	String *keyOne = $(alloc(String), initWithFormat, "one");
	String *keyTwo = $(alloc(String), initWithFormat, "two");
	String *keyThree = $(alloc(String), initWithFormat, "three");

	Dictionary *dict = $$(Dictionary, dictionaryWithObjectsAndKeys,
			objectOne, keyOne, objectTwo, keyTwo, objectThree, keyThree, NULL);

	ck_assert(dict != NULL);
	ck_assert_ptr_eq(_Dictionary(), classof(dict));

	ck_assert_int_eq(3, dict->count);

	ck_assert_ptr_eq(objectOne, $(dict, objectForKey, keyOne));
	ck_assert_ptr_eq(objectTwo, $(dict, objectForKey, keyTwo));
	ck_assert_ptr_eq(objectThree, $(dict, objectForKey, keyThree));

	ck_assert_int_eq(2, objectOne->referenceCount);
	ck_assert_int_eq(2, objectTwo->referenceCount);
	ck_assert_int_eq(2, objectThree->referenceCount);

	release(objectOne);
	release(objectTwo);
	release(objectThree);

	release(keyOne);
	release(keyTwo);
	release(keyThree);

	int counter = 0;

	$(dict, enumerateObjectsAndKeys, enumerator, &counter);

	ck_assert_int_eq(dict->count, counter);

	Dictionary *filtered = $(dict, filterObjectsAndKeys, predicate, NULL);

	ck_assert_int_eq(1, filtered->count);
	ck_assert($(filtered, objectForKey, keyTwo) == objectTwo);

	release(dict);
	release(filtered);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Dictionary");
	tcase_add_test(tcase, dictionary);

	Suite *suite = suite_create("Dictionary");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
