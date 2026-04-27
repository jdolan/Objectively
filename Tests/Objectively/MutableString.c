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

#include <stdlib.h>
#include <unistd.h>
#include <check.h>

#include "Objectively.h"

START_TEST(string) {

  MutableString *string = $$(MutableString, string);

  ck_assert(string != NULL);
  ck_assert_ptr_eq(_MutableString(), classof(string));

  String *hello = str("hello");

  $(string, appendString, hello);
  ck_assert_str_eq("hello", string->string.chars);

  $(string, appendFormat, " %s", "world!");
  ck_assert_str_eq("hello world!", string->string.chars);

  String *goodbye = str("goodbye cruel");

  $(string, replaceStringInRange, (Range) { 0, 5 }, goodbye);
  ck_assert_str_eq("goodbye cruel world!", string->string.chars);

  $(string, replaceOccurrencesOfCharactersInRange, " ", (Range) { 0, 20 }, "  ");
  ck_assert_str_eq("goodbye  cruel  world!", string->string.chars);

  $(string, replaceOccurrencesOfCharacters, " ", "");
  ck_assert_str_eq("goodbyecruelworld!", string->string.chars);

  $(string, setString, NULL);
  ck_assert_str_eq("", string->string.chars);

  $(string, setString, hello);
  ck_assert_str_eq("hello", string->string.chars);

  String *copy = (String *) $((Object * ) string, copy);
  ck_assert_ptr_eq(_MutableString(), classof(copy));
  ck_assert($((Object *) string, isEqual, (Object *) copy));

  release(hello);
  release(goodbye);
  release(string);
  release(copy);

} END_TEST

START_TEST(appendBytes) {

  MutableString *string = $$(MutableString, string);

  const uint8_t hello[] = "hello";
  $(string, appendBytes, hello, sizeof(hello) - 1, STRING_ENCODING_ASCII);
  ck_assert_str_eq("hello", string->string.chars);

  const uint8_t world[] = " world";
  $(string, appendBytes, world, sizeof(world) - 1, STRING_ENCODING_ASCII);
  ck_assert_str_eq("hello world", string->string.chars);

  release(string);

} END_TEST

START_TEST(initWithBytes) {

  const uint8_t bytes[] = "hello world";
  MutableString *string = $(alloc(MutableString), initWithBytes, bytes, sizeof(bytes) - 1, STRING_ENCODING_ASCII);

  ck_assert(string != NULL);
  ck_assert_str_eq("hello world", string->string.chars);

  release(string);

} END_TEST

START_TEST(initWithData) {

  const uint8_t bytes[] = "hello world";
  Data *data = $$(Data, dataWithBytes, bytes, sizeof(bytes) - 1);

  MutableString *string = $(alloc(MutableString), initWithData, data, STRING_ENCODING_ASCII);

  ck_assert(string != NULL);
  ck_assert_str_eq("hello world", string->string.chars);

  release(data);
  release(string);

} END_TEST

START_TEST(initWithContentsOfFile) {

  const char *path = "/tmp/Objectively_MutableString.test";

  MutableString *written = $(alloc(MutableString), initWithCharacters, "hello world");
  ck_assert($((String *) written, writeToFile, path, STRING_ENCODING_UTF8));
  release(written);

  MutableString *string = $(alloc(MutableString), initWithContentsOfFile, path, STRING_ENCODING_UTF8);

  ck_assert(string != NULL);
  ck_assert_str_eq("hello world", string->string.chars);

  release(string);
  unlink(path);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("String");
  tcase_add_test(tcase, string);
  tcase_add_test(tcase, appendBytes);
  tcase_add_test(tcase, initWithBytes);
  tcase_add_test(tcase, initWithData);
  tcase_add_test(tcase, initWithContentsOfFile);

  Suite *suite = suite_create("String");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
