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

START_TEST(init) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  ck_assert_ptr_ne(NULL, table);
  ck_assert_ptr_eq(_HashTable(), classof(table));
  ck_assert_int_eq(0, table->count);

  release(table);

} END_TEST

START_TEST(initWithCapacity) {

  HashTable *table = $(alloc(HashTable), initWithCapacity, HashTableHashStr, HashTableEqualStr, 64);

  ck_assert_ptr_ne(NULL, table);
  ck_assert_int_eq(0, table->count);
  ck_assert_int_eq(64, table->capacity);

  release(table);

} END_TEST

START_TEST(set_get) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  $(table, set, "one", "1");
  $(table, set, "two", "2");
  $(table, set, "three", "3");

  ck_assert_int_eq(3, table->count);

  ck_assert_str_eq("1", $(table, get, "one"));
  ck_assert_str_eq("2", $(table, get, "two"));
  ck_assert_str_eq("3", $(table, get, "three"));
  ck_assert_ptr_eq(NULL, $(table, get, "four"));

  release(table);

} END_TEST

START_TEST(containsKey) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  $(table, set, "one", "1");
  $(table, set, "two", "2");

  ck_assert($(table, containsKey, "one"));
  ck_assert($(table, containsKey, "two"));
  ck_assert(!$(table, containsKey, "three"));

  release(table);

} END_TEST

static void enumerator(const HashTable *table, ident key, ident value, ident data) {
  (*(int *) data)++;
}

START_TEST(enumerateEntries) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  $(table, set, "one", "1");
  $(table, set, "two", "2");
  $(table, set, "three", "3");

  int count = 0;
  $(table, enumerateEntries, enumerator, &count);

  ck_assert_int_eq(3, count);

  release(table);

} END_TEST

START_TEST(removeEntry) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  $(table, set, "one", "1");
  $(table, set, "two", "2");
  $(table, set, "three", "3");

  $(table, remove, "two");

  ck_assert_int_eq(2, table->count);
  ck_assert(!$(table, containsKey, "two"));
  ck_assert_ptr_eq(NULL, $(table, get, "two"));

  release(table);

} END_TEST

START_TEST(removeAll) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  $(table, set, "one", "1");
  $(table, set, "two", "2");
  $(table, set, "three", "3");

  $(table, removeAll);

  ck_assert_int_eq(0, table->count);
  ck_assert_ptr_eq(NULL, $(table, get, "one"));

  release(table);

} END_TEST

START_TEST(set_replaces) {

  HashTable *table = $(alloc(HashTable), init, HashTableHashStr, HashTableEqualStr);

  $(table, set, "key", "original");
  ck_assert_str_eq("original", $(table, get, "key"));
  ck_assert_int_eq(1, table->count);

  $(table, set, "key", "replaced");
  ck_assert_str_eq("replaced", $(table, get, "key"));
  ck_assert_int_eq(1, table->count);

  release(table);

} END_TEST

START_TEST(hash_direct) {

  int a = 1, b = 2;

  HashTable *table = $(alloc(HashTable), init, HashTableHashDirect, HashTableEqualDirect);

  $(table, set, &a, "a");
  $(table, set, &b, "b");

  ck_assert_int_eq(2, table->count);
  ck_assert_str_eq("a", $(table, get, &a));
  ck_assert_str_eq("b", $(table, get, &b));

  release(table);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("HashTable");
  tcase_add_test(tcase, init);
  tcase_add_test(tcase, initWithCapacity);
  tcase_add_test(tcase, set_get);
  tcase_add_test(tcase, containsKey);
  tcase_add_test(tcase, enumerateEntries);
  tcase_add_test(tcase, removeEntry);
  tcase_add_test(tcase, removeAll);
  tcase_add_test(tcase, set_replaces);
  tcase_add_test(tcase, hash_direct);

  Suite *suite = suite_create("HashTable");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
