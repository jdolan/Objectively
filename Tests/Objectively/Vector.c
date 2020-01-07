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

typedef struct {
	int bar;
} Foo;

START_TEST(addElement) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	ck_assert_int_eq(3, vector->count);

	const Foo *elements = vector->elements;

	ck_assert_int_eq(1, elements[0].bar);
	ck_assert_int_eq(2, elements[1].bar);
	ck_assert_int_eq(3, elements[2].bar);

	release(vector);

} END_TEST

static void enumerator(const Vector *vector, ident element, ident data) {
	*(int *) data += *(int *) element;
}

START_TEST(enumerateElements) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	int sum = 0;
	$(vector, enumerateElements, enumerator, &sum);

	ck_assert_int_eq(6, sum);

	release(vector);

} END_TEST

static _Bool predicate(const ident obj, ident data) {
	return ((Foo *) obj)->bar == (int) data;
}

START_TEST(filterElements) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	$(vector, filterElements, predicate, (ident) 2);

	ck_assert_int_eq(1, vector->count);
	ck_assert_int_eq(2, VectorElement(vector, Foo, 0)->bar);

	release(vector);

} END_TEST

START_TEST(findElement) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 }, *foo;

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	foo = $(vector, findElement, predicate, (ident) 1);
	ck_assert_int_eq(1, foo->bar);

	foo = $(vector, findElement, predicate, (ident) 2);
	ck_assert_int_eq(2, foo->bar);;

	foo = $(vector, findElement, predicate, (ident) 3);
	ck_assert_int_eq(3, foo->bar);;

	release(vector);

} END_TEST

START_TEST(initWithElements) {

	Foo *foo = calloc(3, sizeof(Foo));
	ck_assert_ptr_ne(NULL, foo);

	Vector *vector = $(alloc(Vector), initWithElements, sizeof(Foo), 3, foo);
	ck_assert_ptr_ne(NULL, vector);
	ck_assert_ptr_eq(_Vector(), classof(vector));

	ck_assert_int_eq(3, vector->count);
	ck_assert_ptr_eq(foo, vector->elements);

	release(vector);

} END_TEST

START_TEST(initWithSize) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));
	ck_assert_ptr_ne(NULL, vector);
	ck_assert_ptr_eq(_Vector(), classof(vector));

	ck_assert_int_eq(0, vector->count);
	ck_assert_int_eq(0, vector->capacity);
	ck_assert_ptr_eq(NULL, vector->elements);

	release(vector);

} END_TEST

START_TEST(insertElementAtIndex) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, insertElementAtIndex, &one, 0);
	$(vector, insertElementAtIndex, &two, 0);
	$(vector, insertElementAtIndex, &three, 0);

	ck_assert_int_eq(3, vector->count);

	const Foo *elements = vector->elements;

	ck_assert_int_eq(3, elements[0].bar);
	ck_assert_int_eq(2, elements[1].bar);
	ck_assert_int_eq(1, elements[2].bar);

	release(vector);

} END_TEST

static ident reducer(const ident obj, ident accumulator, ident data) {
	return accumulator + ((Foo *) obj)->bar;
}

START_TEST(reduce) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	ck_assert_int_eq(6, (int) $(vector, reduce, reducer, (ident) 0, NULL));

	release(vector);

} END_TEST

START_TEST(removeAllElements) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	$(vector, removeAllElements);

	ck_assert_int_eq(0, vector->count);

	release(vector);

} END_TEST

START_TEST(removeElementAtIndex) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	$(vector, removeElementAtIndex, 0);

	ck_assert_int_eq(2, vector->count);

	const Foo *elements = vector->elements;

	ck_assert_int_eq(2, elements[0].bar);
	ck_assert_int_eq(3, elements[1].bar);

	release(vector);

} END_TEST

START_TEST(resize) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	ck_assert_int_eq(3, vector->count);

	$(vector, resize, 2);

	ck_assert_int_eq(2, vector->count);

	const Foo *elements = vector->elements;

	ck_assert_int_eq(1, elements[0].bar);
	ck_assert_int_eq(2, elements[1].bar);

	release(vector);

} END_TEST

static Order comparator(const ident a, const ident b) {
	return ((Foo *) b)->bar - ((Foo *) a)->bar;
}

START_TEST(sort) {

	Vector *vector = $(alloc(Vector), initWithSize, sizeof(Foo));

	Foo one = { 1 }, two = { 2 }, three = { 3 };

	$(vector, addElement, &one);
	$(vector, addElement, &two);
	$(vector, addElement, &three);

	ck_assert_int_eq(3, vector->count);

	$(vector, sort, comparator);

	const Foo *elements = vector->elements;

	ck_assert_int_eq(3, elements[0].bar);
	ck_assert_int_eq(2, elements[1].bar);
	ck_assert_int_eq(1, elements[2].bar);

	release(vector);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Vector");
	tcase_add_test(tcase, addElement);
	tcase_add_test(tcase, enumerateElements);
	tcase_add_test(tcase, filterElements);
	tcase_add_test(tcase, findElement);
	tcase_add_test(tcase, initWithElements);
	tcase_add_test(tcase, initWithSize);
	tcase_add_test(tcase, insertElementAtIndex);
	tcase_add_test(tcase, reduce);
	tcase_add_test(tcase, removeAllElements);
	tcase_add_test(tcase, removeElementAtIndex);
	tcase_add_test(tcase, resize);
	tcase_add_test(tcase, sort);

	Suite *suite = suite_create("Vector");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
