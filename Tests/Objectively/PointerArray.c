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

#include "Objectively.h"

START_TEST(add) {

  int one = 1, two = 2, three = 3;

  PointerArray *array = $(alloc(PointerArray), init);

  $(array, add, &one);
  $(array, add, &two);
  $(array, add, &three);

  ck_assert_int_eq(3, array->count);
  ck_assert_ptr_eq(&one, array->elements[0]);
  ck_assert_ptr_eq(&two, array->elements[1]);
  ck_assert_ptr_eq(&three, array->elements[2]);

  release(array);

} END_TEST

static int destroyCount;
static void *lastDestroyed;

static void destroyFunc(ident element) {
  destroyCount++;
  lastDestroyed = element;
}

START_TEST(destroy) {

  destroyCount = 0;
  lastDestroyed = NULL;

  int one = 1, two = 2, three = 3;

  PointerArray *array = $(alloc(PointerArray), initWithDestroy, destroyFunc);

  $(array, add, &one);
  $(array, add, &two);
  $(array, add, &three);

  $(array, removeElementAtIndex, 1);

  ck_assert_int_eq(1, destroyCount);
  ck_assert_ptr_eq(&two, lastDestroyed);

  // Leave two elements; release exercises the dealloc path.
  release(array);
  ck_assert_int_eq(3, destroyCount);

} END_TEST

START_TEST(init) {

  PointerArray *array = $(alloc(PointerArray), init);

  ck_assert_ptr_ne(NULL, array);
  ck_assert_ptr_eq(_PointerArray(), classof(array));
  ck_assert_int_eq(0, array->count);
  ck_assert_int_eq(0, array->capacity);
  ck_assert_ptr_eq(NULL, array->elements);
  ck_assert_ptr_eq(NULL, array->destroy);

  release(array);

} END_TEST

START_TEST(initWithDestroy) {

  PointerArray *array = $(alloc(PointerArray), initWithDestroy, destroyFunc);

  ck_assert_ptr_ne(NULL, array);
  ck_assert_ptr_eq(destroyFunc, array->destroy);

  release(array);

} END_TEST

START_TEST(elementAtIndex) {

  int one = 1, two = 2, three = 3;

  PointerArray *array = $(alloc(PointerArray), init);

  $(array, add, &one);
  $(array, add, &two);
  $(array, add, &three);

  ck_assert_ptr_eq(&one,   $(array, get, 0));
  ck_assert_ptr_eq(&two,   $(array, get, 1));
  ck_assert_ptr_eq(&three, $(array, get, 2));

  release(array);

} END_TEST

START_TEST(removeAll) {

  int one = 1, two = 2, three = 3;

  PointerArray *array = $(alloc(PointerArray), init);

  $(array, add, &one);
  $(array, add, &two);
  $(array, add, &three);

  $(array, removeAll);

  ck_assert_int_eq(0, array->count);

  release(array);

} END_TEST

START_TEST(_remove) {

  int one = 1, two = 2, three = 3;

  PointerArray *array = $(alloc(PointerArray), init);

  $(array, add, &one);
  $(array, add, &two);
  $(array, add, &three);

  $(array, remove, &two);

  ck_assert_int_eq(2, array->count);
  ck_assert_ptr_eq(&one,   array->elements[0]);
  ck_assert_ptr_eq(&three, array->elements[1]);

  release(array);

} END_TEST

START_TEST(removeElementAtIndex) {

  int one = 1, two = 2, three = 3;

  PointerArray *array = $(alloc(PointerArray), init);

  $(array, add, &one);
  $(array, add, &two);
  $(array, add, &three);

  $(array, removeElementAtIndex, 0);

  ck_assert_int_eq(2, array->count);
  ck_assert_ptr_eq(&two,   array->elements[0]);
  ck_assert_ptr_eq(&three, array->elements[1]);

  release(array);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("PointerArray");
  tcase_add_test(tcase, add);
  tcase_add_test(tcase, destroy);
  tcase_add_test(tcase, init);
  tcase_add_test(tcase, initWithDestroy);
  tcase_add_test(tcase, elementAtIndex);
  tcase_add_test(tcase, removeAll);
  tcase_add_test(tcase, _remove);
  tcase_add_test(tcase, removeElementAtIndex);

  Suite *suite = suite_create("PointerArray");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
