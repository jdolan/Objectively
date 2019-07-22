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
#include <locale.h>

#include <Objectively.h>

static void setup(void) {
	setlocale(LC_CTYPE, "en_US.UTF-8");
}

static void teardown(void) {
	setlocale(LC_CTYPE, NULL);
}

START_TEST(initWithCharacters) {

	StringReader *reader = $(alloc(StringReader), initWithCharacters, "Hello World!");
	ck_assert(reader != NULL);
	ck_assert_ptr_eq(_StringReader(), classof(reader));

	release(reader);
} END_TEST

START_TEST(read_english) {

	StringReader *reader = $(alloc(StringReader), initWithCharacters, "Hello World!");
	ck_assert(reader != NULL);

	ck_assert_int_eq(L'H', $(reader, read));
	ck_assert_int_eq(L'e', $(reader, read));
	ck_assert_int_eq(L'l', $(reader, read));
	ck_assert_int_eq(L'l', $(reader, read));
	ck_assert_int_eq(L'o', $(reader, read));
	ck_assert_int_eq(L' ', $(reader, read));
	ck_assert_int_eq(L'W', $(reader, read));
	ck_assert_int_eq(L'o', $(reader, read));
	ck_assert_int_eq(L'r', $(reader, read));
	ck_assert_int_eq(L'l', $(reader, read));
	ck_assert_int_eq(L'd', $(reader, read));
	ck_assert_int_eq(L'!', $(reader, read));
	ck_assert_int_eq(READER_EOF, $(reader, read));

	release(reader);
} END_TEST

START_TEST(read_japanese) {

	StringReader *reader = $(alloc(StringReader), initWithCharacters, "こんにちは世界");
	ck_assert(reader != NULL);

	ck_assert_int_eq(L'こ', $(reader, read));
	ck_assert_int_eq(L'ん', $(reader, read));
	ck_assert_int_eq(L'に', $(reader, read));
	ck_assert_int_eq(L'ち', $(reader, read));
	ck_assert_int_eq(L'は', $(reader, read));
	ck_assert_int_eq(L'世', $(reader, read));
	ck_assert_int_eq(L'界', $(reader, read));
	ck_assert_int_eq(READER_EOF, $(reader, read));

	release(reader);
} END_TEST

START_TEST(readToken) {

	StringReader *reader = $(alloc(StringReader), initWithCharacters, "ṮḧḭṠ  ḭṠ  ṏṆḶẏ  Ḁ  ṮḕṠṮ");
	ck_assert(reader != NULL);

	String *token;
	const Unicode *charset = L" ";
	Unicode stop;

	token = $(reader, readToken, charset, &stop);
	ck_assert_ptr_ne(NULL, token);
	ck_assert_str_eq("ṮḧḭṠ", token->chars);
	ck_assert_int_eq(' ', stop);
	release(token);

	token = $(reader, readToken, charset, &stop);
	ck_assert_ptr_ne(NULL, token);
	ck_assert_str_eq("ḭṠ", token->chars);
	ck_assert_int_eq(' ', stop);
	release(token);

	token = $(reader, readToken, charset, &stop);
	ck_assert_ptr_ne(NULL, token);
	ck_assert_str_eq("ṏṆḶẏ", token->chars);
	ck_assert_int_eq(' ', stop);
	release(token);

	token = $(reader, readToken, charset, &stop);
	ck_assert_ptr_ne(NULL, token);
	ck_assert_str_eq("Ḁ", token->chars);
	ck_assert_int_eq(' ', stop);
	release(token);

	token = $(reader, readToken, charset, &stop);
	ck_assert_ptr_ne(NULL, token);
	ck_assert_str_eq("ṮḕṠṮ", token->chars);
	ck_assert_int_eq(-1, stop);
	release(token);

	token = $(reader, readToken, charset, &stop);
	ck_assert_ptr_eq(NULL, token);
	ck_assert_int_eq(READER_EOF, stop);

	release(reader);
} END_TEST


int main(int argc, char **argv) {

	TCase *tcase = tcase_create("StringReader");
	tcase_add_unchecked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, initWithCharacters);
	tcase_add_test(tcase, read_english);
	tcase_add_test(tcase, read_japanese);
	tcase_add_test(tcase, readToken);

	Suite *suite = suite_create("StringReader");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_NORMAL);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
