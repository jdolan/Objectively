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

START_TEST(appendElement) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

START_TEST(containsElement) {

  int one = 1, two = 2, three = 3, other = 4;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  ck_assert($(list, contains, &one));
  ck_assert($(list, contains, &two));
  ck_assert($(list, contains, &three));
  ck_assert(!$(list, contains, &other));

  release(list);

} END_TEST

static bool enumerator(const List *list, ListNode *node, ident data) {
  *(int *) data += *(int *) node->element;
  return false;
}

START_TEST(enumerate) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  int sum = 0;
  $(list, enumerate, enumerator, &sum);

  ck_assert_int_eq(6, sum);

  release(list);

} END_TEST

static bool predicate(const ident element, ident data) {
  return *(int *) element > (intptr_t) data;
}

START_TEST(filter) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  $(list, filter, predicate, (ident) 1);

  ck_assert_int_eq(2, list->count);
  ck_assert(!$(list, contains, &one));
  ck_assert($(list, contains, &two));
  ck_assert($(list, contains, &three));

  release(list);

} END_TEST

START_TEST(filteredList) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  List *filtered = $(list, filteredList, predicate, (ident) 1);

  ck_assert_int_eq(3, list->count);
  ck_assert_int_eq(2, filtered->count);
  ck_assert(!$(filtered, contains, &one));
  ck_assert($(filtered, contains, &two));
  ck_assert($(filtered, contains, &three));

  release(filtered);
  release(list);

} END_TEST

START_TEST(find) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  ck_assert_ptr_eq(&two, $(list, find, predicate, (ident) 1));
  ck_assert_ptr_eq(&three, $(list, find, predicate, (ident) 2));
  ck_assert_ptr_eq(NULL, $(list, find, predicate, (ident) 3));

  release(list);

} END_TEST

START_TEST(init) {

  List *list = $(alloc(List), init);

  ck_assert_ptr_ne(NULL, list);
  ck_assert_ptr_eq(_List(), classof(list));
  ck_assert_int_eq(0, list->count);
  ck_assert_ptr_eq(NULL, list->head);
  ck_assert_ptr_eq(NULL, list->tail);

  release(list);

} END_TEST

START_TEST(insertAfter) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &three);

  ListNode *node = $(list, nodeForElement, &one);
  $(list, insertAfter, node, &two);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&two, list->head->next->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

static ident functor(const ident element, ident data) {
  int *n = (int *) element;
  return (ident)(intptr_t)(*n * 2);
}

START_TEST(map) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  $(list, map, functor, NULL);

  ck_assert_int_eq(3, list->count);
  ck_assert_int_eq(2, (intptr_t) list->head->element);
  ck_assert_int_eq(4, (intptr_t) list->head->next->element);
  ck_assert_int_eq(6, (intptr_t) list->tail->element);

  release(list);

} END_TEST

START_TEST(mappedList) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  List *mapped = $(list, mappedList, functor, NULL);

  ck_assert_int_eq(3, list->count);
  ck_assert_int_eq(3, mapped->count);
  ck_assert_int_eq(2, (intptr_t) mapped->head->element);
  ck_assert_int_eq(4, (intptr_t) mapped->head->next->element);
  ck_assert_int_eq(6, (intptr_t) mapped->tail->element);

  release(mapped);
  release(list);

} END_TEST

START_TEST(nodeForElement) {

  int one = 1, two = 2, three = 3, other = 4;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  ck_assert_ptr_eq(&one, $(list, nodeForElement, &one)->element);
  ck_assert_ptr_eq(&two, $(list, nodeForElement, &two)->element);
  ck_assert_ptr_eq(&three, $(list, nodeForElement, &three)->element);
  ck_assert_ptr_eq(NULL, $(list, nodeForElement, &other));

  release(list);

} END_TEST

START_TEST(prepend) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, prepend, &three);
  $(list, prepend, &two);
  $(list, prepend, &one);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

static ident reducer(const ident element, ident accumulator, ident data) {
  return (ident)((intptr_t) accumulator + *(int *) element);
}

START_TEST(reduce) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  ck_assert_int_eq(6, (intptr_t) $(list, reduce, reducer, (ident) 0, NULL));

  release(list);

} END_TEST

START_TEST(removeAll) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  $(list, removeAll);

  ck_assert_int_eq(0, list->count);
  ck_assert_ptr_eq(NULL, list->head);
  ck_assert_ptr_eq(NULL, list->tail);

  release(list);

} END_TEST

START_TEST(_remove) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  $(list, remove, &two);

  ck_assert_int_eq(2, list->count);
  ck_assert(!$(list, contains, &two));
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

START_TEST(removeNode) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &one);
  $(list, append, &two);
  $(list, append, &three);

  ListNode *node = $(list, nodeForElement, &one);
  $(list, removeNode, node);

  ck_assert_int_eq(2, list->count);
  ck_assert_ptr_eq(&two, list->head->element);

  release(list);

} END_TEST

static Order comparator(const ident a, const ident b) {
  return *(int *) a - *(int *) b;
}

START_TEST(sort) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, append, &three);
  $(list, append, &one);
  $(list, append, &two);

  $(list, sort, comparator);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&two, list->head->next->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("List");
  tcase_add_test(tcase, appendElement);
  tcase_add_test(tcase, containsElement);
  tcase_add_test(tcase, enumerate);
  tcase_add_test(tcase, filter);
  tcase_add_test(tcase, filteredList);
  tcase_add_test(tcase, find);
  tcase_add_test(tcase, init);
  tcase_add_test(tcase, insertAfter);
  tcase_add_test(tcase, map);
  tcase_add_test(tcase, mappedList);
  tcase_add_test(tcase, nodeForElement);
  tcase_add_test(tcase, prepend);
  tcase_add_test(tcase, reduce);
  tcase_add_test(tcase, removeAll);
  tcase_add_test(tcase, _remove);
  tcase_add_test(tcase, removeNode);
  tcase_add_test(tcase, sort);

  Suite *suite = suite_create("List");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
