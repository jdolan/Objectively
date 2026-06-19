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

START_TEST(indexSet) {
  
  size_t indexes[] = { 1, 2, 3, 3, 2 };
  IndexSet *indexSet = $(alloc(IndexSet), initWithIndexes, indexes, lengthof(indexes));

  ck_assert(indexSet != NULL);
  ck_assert_int_eq(3, indexSet->count);
  ck_assert($(indexSet, containsIndex, 1));
  ck_assert($(indexSet, containsIndex, 2));
  ck_assert($(indexSet, containsIndex, 3));

  String *description = $((Object *) indexSet, description);
  ck_assert_str_eq("[1, 2, 3]", description->chars);

  Object *object = (Object *) indexSet;
  Object *copy = $(object, copy);

  ck_assert(copy != NULL);
  ck_assert_int_eq($(object, hash), $(copy, hash));
  ck_assert($(object, isEqual, copy));

  release(description);
  release(copy);
  release(indexSet);

} END_TEST


START_TEST(indexSet_addIndex) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  $(indexSet, addIndex, 2);
  $(indexSet, addIndex, 1);
  $(indexSet, addIndex, 0);
  $(indexSet, addIndex, 2);
  $(indexSet, addIndex, 1);
  $(indexSet, addIndex, 0);

  ck_assert_int_eq(3, indexSet->count);

  const size_t *indexes = indexSet->indexes;

  ck_assert_int_eq(0, indexes[0]);
  ck_assert_int_eq(1, indexes[1]);
  ck_assert_int_eq(2, indexes[2]);

  release(indexSet);

} END_TEST

START_TEST(indexSet_addIndexes) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  size_t in[] = { 3, 5, 4, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  const size_t *indexes = indexSet->indexes;

  ck_assert_int_eq(1, indexes[0]);
  ck_assert_int_eq(2, indexes[1]);
  ck_assert_int_eq(3, indexes[2]);
  ck_assert_int_eq(4, indexes[3]);
  ck_assert_int_eq(5, indexes[4]);

  release(indexSet);

} END_TEST

START_TEST(indexSet_addIndexesInRange) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  const Range range = { 1, 10 };
  $(indexSet, addIndexesInRange, range);

  const size_t *indexes = indexSet->indexes;
  for (size_t r = range.location, i = 0; r < range.length; r++, i++) {
    ck_assert_int_eq(r, indexes[i]);
  }

  release(indexSet);

} END_TEST

START_TEST(indexSet_init) {

  IndexSet *indexSet = $(alloc(IndexSet), init);
  ck_assert_ptr_ne(NULL, indexSet);
  ck_assert_ptr_eq(_IndexSet(), classof(indexSet));
  release(indexSet);

} END_TEST

START_TEST(indexSet_initWithCapacity) {

  IndexSet *indexSet = $(alloc(IndexSet), initWithCapacity, 1);
  ck_assert_ptr_ne(NULL, indexSet);
  ck_assert_ptr_eq(_IndexSet(), classof(indexSet));

  ck_assert_int_eq(1, indexSet->capacity);

  release(indexSet);

} END_TEST

START_TEST(indexSet_removeAllIndexes) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  $(indexSet, removeAllIndexes);

  ck_assert_int_eq(0, indexSet->count);

  release(indexSet);

} END_TEST

START_TEST(indexSet_removeIndex) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  $(indexSet, removeIndex, 3);

  const size_t *indexes = indexSet->indexes;

  ck_assert_int_eq(1, indexes[0]);
  ck_assert_int_eq(2, indexes[1]);
  ck_assert_int_eq(4, indexes[2]);
  ck_assert_int_eq(5, indexes[3]);

  release(indexSet);

} END_TEST

START_TEST(indexSet_removeIndexes) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  size_t out[] = { 0, 2, 3 };

  $(indexSet, removeIndexes, out, lengthof(out));

  ck_assert_int_eq(3, indexSet->count);
  
  const size_t *indexes = indexSet->indexes;

  ck_assert_int_eq(1, indexes[0]);
  ck_assert_int_eq(4, indexes[1]);
  ck_assert_int_eq(5, indexes[2]);

  release(indexSet);

} END_TEST

START_TEST(indexSet_removeIndexesInRange) {

  IndexSet *indexSet = $(alloc(IndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  const Range range = { 2, 10 };
  $(indexSet, removeIndexesInRange, range);

  ck_assert_int_eq(1, indexSet->count);

  const size_t *indexes = indexSet->indexes;

  ck_assert_int_eq(1, indexes[0]);

  release(indexSet);

} END_TEST


int main(int argc, char **argv) {

  TCase *tcase = tcase_create("IndexSet");
  tcase_add_test(tcase, indexSet);

  tcase_add_test(tcase, indexSet_addIndex);
  tcase_add_test(tcase, indexSet_addIndexes);
  tcase_add_test(tcase, indexSet_addIndexesInRange);
  tcase_add_test(tcase, indexSet_init);
  tcase_add_test(tcase, indexSet_initWithCapacity);
  tcase_add_test(tcase, indexSet_removeAllIndexes);
  tcase_add_test(tcase, indexSet_removeIndex);
  tcase_add_test(tcase, indexSet_removeIndexes);
  tcase_add_test(tcase, indexSet_removeIndexesInRange);

  Suite *suite = suite_create("IndexSet");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
