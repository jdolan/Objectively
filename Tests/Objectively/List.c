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

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

START_TEST(containsElement) {

  int one = 1, two = 2, three = 3, other = 4;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  ck_assert($(list, containsElement, &one));
  ck_assert($(list, containsElement, &two));
  ck_assert($(list, containsElement, &three));
  ck_assert(!$(list, containsElement, &other));

  release(list);

} END_TEST

static bool enumerator(const List *list, ListNode *node, ident data) {
  *(int *) data += *(int *) node->element;
  return false;
}

START_TEST(enumerateElements) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  int sum = 0;
  $(list, enumerateElements, enumerator, &sum);

  ck_assert_int_eq(6, sum);

  release(list);

} END_TEST

START_TEST(findElement) {

  int one = 1, two = 2, three = 3, other = 4;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  ck_assert_ptr_eq(&one, $(list, findElement, &one)->element);
  ck_assert_ptr_eq(&two, $(list, findElement, &two)->element);
  ck_assert_ptr_eq(&three, $(list, findElement, &three)->element);
  ck_assert_ptr_eq(NULL, $(list, findElement, &other));

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

START_TEST(insertElementAfter) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &three);

  ListNode *node = $(list, findElement, &one);
  $(list, insertElementAfter, node, &two);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&two, list->head->next->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

START_TEST(prependElement) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, prependElement, &three);
  $(list, prependElement, &two);
  $(list, prependElement, &one);

  ck_assert_int_eq(3, list->count);
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

START_TEST(removeAll) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  $(list, removeAll);

  ck_assert_int_eq(0, list->count);
  ck_assert_ptr_eq(NULL, list->head);
  ck_assert_ptr_eq(NULL, list->tail);

  release(list);

} END_TEST

START_TEST(removeElement) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  $(list, removeElement, &two);

  ck_assert_int_eq(2, list->count);
  ck_assert(!$(list, containsElement, &two));
  ck_assert_ptr_eq(&one, list->head->element);
  ck_assert_ptr_eq(&three, list->tail->element);

  release(list);

} END_TEST

START_TEST(removeNode) {

  int one = 1, two = 2, three = 3;

  List *list = $(alloc(List), init);

  $(list, appendElement, &one);
  $(list, appendElement, &two);
  $(list, appendElement, &three);

  ListNode *node = $(list, findElement, &one);
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

  $(list, appendElement, &three);
  $(list, appendElement, &one);
  $(list, appendElement, &two);

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
  tcase_add_test(tcase, enumerateElements);
  tcase_add_test(tcase, findElement);
  tcase_add_test(tcase, init);
  tcase_add_test(tcase, insertElementAfter);
  tcase_add_test(tcase, prependElement);
  tcase_add_test(tcase, removeAll);
  tcase_add_test(tcase, removeElement);
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
