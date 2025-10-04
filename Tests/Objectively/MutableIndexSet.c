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

START_TEST(addIndex) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  $(indexSet, addIndex, 2);
  $(indexSet, addIndex, 1);
  $(indexSet, addIndex, 0);
  $(indexSet, addIndex, 2);
  $(indexSet, addIndex, 1);
  $(indexSet, addIndex, 0);

  ck_assert_int_eq(3, indexSet->indexSet.count);

  const size_t *indexes = indexSet->indexSet.indexes;

  ck_assert_int_eq(0, indexes[0]);
  ck_assert_int_eq(1, indexes[1]);
  ck_assert_int_eq(2, indexes[2]);

  release(indexSet);

} END_TEST

START_TEST(addIndexes) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  size_t in[] = { 3, 5, 4, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  const size_t *indexes = indexSet->indexSet.indexes;

  ck_assert_int_eq(1, indexes[0]);
  ck_assert_int_eq(2, indexes[1]);
  ck_assert_int_eq(3, indexes[2]);
  ck_assert_int_eq(4, indexes[3]);
  ck_assert_int_eq(5, indexes[4]);

  release(indexSet);

} END_TEST

START_TEST(addIndexesInRange) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  const Range range = { 1, 10 };
  $(indexSet, addIndexesInRange, range);

  const size_t *indexes = indexSet->indexSet.indexes;
  for (size_t r = range.location, i = 0; r < range.length; r++, i++) {
    ck_assert_int_eq(r, indexes[i]);
  }

  release(indexSet);

} END_TEST

START_TEST(init) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);
  ck_assert_ptr_ne(NULL, indexSet);
  ck_assert_ptr_eq(_MutableIndexSet(), classof(indexSet));
  release(indexSet);

} END_TEST

START_TEST(initWithCapacity) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), initWithCapacity, 1);
  ck_assert_ptr_ne(NULL, indexSet);
  ck_assert_ptr_eq(_MutableIndexSet(), classof(indexSet));

  ck_assert_int_eq(1, indexSet->capacity);

  release(indexSet);

} END_TEST

START_TEST(removeAllIndexes) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  $(indexSet, removeAllIndexes);

  ck_assert_int_eq(0, indexSet->indexSet.count);

  release(indexSet);

} END_TEST

START_TEST(removeIndex) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  $(indexSet, removeIndex, 3);

  const size_t *indexes = indexSet->indexSet.indexes;

  ck_assert_int_eq(1, indexes[0]);
  ck_assert_int_eq(2, indexes[1]);
  ck_assert_int_eq(4, indexes[2]);
  ck_assert_int_eq(5, indexes[3]);

  release(indexSet);

} END_TEST

START_TEST(removeIndexes) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  size_t out[] = { 0, 2, 3 };

  $(indexSet, removeIndexes, out, lengthof(out));

  ck_assert_int_eq(3, indexSet->indexSet.count);
  
  const size_t *indexes = indexSet->indexSet.indexes;

  ck_assert_int_eq(1, indexes[0]);
  ck_assert_int_eq(4, indexes[1]);
  ck_assert_int_eq(5, indexes[2]);

  release(indexSet);

} END_TEST

START_TEST(removeIndexesInRange) {

  MutableIndexSet *indexSet = $(alloc(MutableIndexSet), init);

  size_t in[] = { 5, 4, 3, 2, 1 };

  $(indexSet, addIndexes, in, lengthof(in));

  const Range range = { 2, 10 };
  $(indexSet, removeIndexesInRange, range);

  ck_assert_int_eq(1, indexSet->indexSet.count);

  const size_t *indexes = indexSet->indexSet.indexes;

  ck_assert_int_eq(1, indexes[0]);

  release(indexSet);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("MutableIndexSet");
  tcase_add_test(tcase, addIndex);
  tcase_add_test(tcase, addIndexes);
  tcase_add_test(tcase, addIndexesInRange);
  tcase_add_test(tcase, init);
  tcase_add_test(tcase, initWithCapacity);
  tcase_add_test(tcase, removeAllIndexes);
  tcase_add_test(tcase, removeIndex);
  tcase_add_test(tcase, removeIndexes);
  tcase_add_test(tcase, removeIndexesInRange);

  Suite *suite = suite_create("MutableIndexSet");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
