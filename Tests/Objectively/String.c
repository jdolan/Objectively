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

  String *string = str("hello %s!", "world");

  ck_assert(string != NULL);
  ck_assert_ptr_eq(_String(), classof(string));

  String *copy = (String *) $((Object * ) string, copy);
  ck_assert_str_eq("hello world!", copy->chars);

  ck_assert($((Object *) string, isEqual, (Object *) copy));
  ck_assert_int_eq($((Object *) string, hash), $((Object *) copy, hash));

  String *prefix = str("hello");
  ck_assert($(string, hasPrefix, prefix));

  String *suffix = str("world!");
  ck_assert($(string, hasSuffix, suffix));

  Range range = { 6, 5 };
  String *substring = $(string, substring, range);
  ck_assert_str_eq("world", substring->chars);

  Range match = $(string, rangeOfString, substring, range);
  ck_assert_int_eq(range.location, match.location);
  ck_assert_int_eq(range.length, match.length);

  String *sep = str(" ");
  Array *components = $(string, componentsSeparatedByString, sep);
  ck_assert_int_eq(2, components->count);

  for (int i = 0; i < components->count; i++) {
    String *component = $(components, objectAtIndex, i);

    switch (i) {
      case 0:
        ck_assert_str_eq("hello", component->chars);
        break;
      case 1:
        ck_assert_str_eq("world!", component->chars);
        break;
      default:
        break;
    }
  }

  String *upper = $(string, uppercaseString);
  ck_assert_str_eq("HELLO WORLD!", upper->chars);

  String *lower = $(upper, lowercaseString);
  ck_assert_str_eq("hello world!", lower->chars);

  String *whitespace = str(" hello ");
  String *trimmed = $(whitespace, trimmedString);
  ck_assert_str_eq("hello", trimmed->chars);
  release(trimmed);

  String *newlines = str("\nhello\n");
  String *trimmedNewlines = $(newlines, trimmedString);
  ck_assert_str_eq("hello", trimmedNewlines->chars);
  release(newlines);
  release(trimmedNewlines);

  const char *path = "/tmp/Objectively_String.test";
  ck_assert($(string, writeToFile, path, STRING_ENCODING_UTF8));

  String *fromFile = $$(String, stringWithContentsOfFile, path, STRING_ENCODING_UTF8);
  ck_assert_str_eq("hello world!", fromFile->chars);

  unlink(path);

  release(fromFile);
  release(upper);
  release(lower);
  release(sep);
  release(components);
  release(substring);
  release(prefix);
  release(suffix);
  release(copy);
  release(string);

} END_TEST

START_TEST(stringMutable) {

  String *string = $$(String, string);

  ck_assert(string != NULL);
  ck_assert_ptr_eq(_String(), classof(string));

  String *hello = str("hello");

  $(string, appendString, hello);
  ck_assert_str_eq("hello", string->chars);

  $(string, appendFormat, " %s", "world!");
  ck_assert_str_eq("hello world!", string->chars);

  String *goodbye = str("goodbye cruel");

  $(string, replaceStringInRange, (Range) { 0, 5 }, goodbye);
  ck_assert_str_eq("goodbye cruel world!", string->chars);

  $(string, replaceOccurrencesOfCharactersInRange, " ", (Range) { 0, 20 }, "  ");
  ck_assert_str_eq("goodbye  cruel  world!", string->chars);

  $(string, replaceOccurrencesOfCharacters, " ", "");
  ck_assert_str_eq("goodbyecruelworld!", string->chars);

  $(string, setString, NULL);
  ck_assert_str_eq("", string->chars);

  $(string, setString, hello);
  ck_assert_str_eq("hello", string->chars);

  String *copy = (String *) $((Object * ) string, copy);
  ck_assert_ptr_eq(_String(), classof(copy));
  ck_assert($((Object *) string, isEqual, (Object *) copy));

  release(hello);
  release(goodbye);
  release(string);
  release(copy);

} END_TEST

START_TEST(appendBytes) {

  String *string = $$(String, string);

  const uint8_t hello[] = "hello";
  $(string, appendBytes, hello, sizeof(hello) - 1, STRING_ENCODING_ASCII);
  ck_assert_str_eq("hello", string->chars);

  const uint8_t world[] = " world";
  $(string, appendBytes, world, sizeof(world) - 1, STRING_ENCODING_ASCII);
  ck_assert_str_eq("hello world", string->chars);

  release(string);

} END_TEST

START_TEST(initWithBytes) {

  const uint8_t bytes[] = "hello world";
  String *string = $(alloc(String), initWithBytes, bytes, sizeof(bytes) - 1, STRING_ENCODING_ASCII);

  ck_assert(string != NULL);
  ck_assert_str_eq("hello world", string->chars);

  release(string);

} END_TEST

START_TEST(initWithData) {

  const uint8_t bytes[] = "hello world";
  Data *data = $$(Data, dataWithBytes, bytes, sizeof(bytes) - 1);

  String *string = $(alloc(String), initWithData, data, STRING_ENCODING_ASCII);

  ck_assert(string != NULL);
  ck_assert_str_eq("hello world", string->chars);

  release(data);
  release(string);

} END_TEST

START_TEST(initWithContentsOfFile) {

  const char *path = "/tmp/Objectively_String.test";

  String *written = $(alloc(String), initWithCharacters, "hello world");
  ck_assert($(written, writeToFile, path, STRING_ENCODING_UTF8));
  release(written);

  String *string = $(alloc(String), initWithContentsOfFile, path, STRING_ENCODING_UTF8);

  ck_assert(string != NULL);
  ck_assert_str_eq("hello world", string->chars);

  release(string);
  unlink(path);

} END_TEST

START_TEST(_strtrim) {

  char *trimmed = strtrim("hello world");
  ck_assert_str_eq("hello world", trimmed);

  free(trimmed);

  trimmed = strtrim("\t hello world");
  ck_assert_str_eq("hello world", trimmed);

  free(trimmed);

  trimmed = strtrim("hello world\t ");
  ck_assert_str_eq("hello world", trimmed);

  free(trimmed);

  trimmed = strtrim("\n hello world \n\t");
  ck_assert_str_eq("hello world", trimmed);

  free(trimmed);
} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("String");
  tcase_add_test(tcase, string);
  tcase_add_test(tcase, _strtrim);
  tcase_add_test(tcase, stringMutable);
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
