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

#include <unistd.h>
#include <stdlib.h>
#include <check.h>

#include "Objectively.h"

START_TEST(data) {
  
  Data *data1 = $(alloc(Data), initWithBytes, (uint8_t *) "abcdef", 6);
  Data *data2 = $(alloc(Data), initWithBytes, (uint8_t *) "ghijkl", 6);

  ck_assert(data1 != NULL);
  ck_assert(data2 != NULL);

  ck_assert_int_eq(6, data1->length);

  ck_assert($((Object *) data1, isEqual, (Object *) data2) == false);
  release(data2);

  data2 = (Data *) $((Object * ) data1, copy);
  ck_assert(data2 != NULL);

  ck_assert($((Object *) data1, isEqual, (Object *) data2) == true);
  release(data2);

  const char *path = "Objectively_Data.test";
  ck_assert($(data1, writeToFile, path) == true);

  data2 = $(alloc(Data), initWithContentsOfFile, path);

  unlink(path);

  ck_assert($((Object *) data1, isEqual, (Object *) data2) == true);

  release(data1);
  release(data2);
} END_TEST


START_TEST(data_mutation) {

  Data *data = $(alloc(Data), init);
  $(data, appendBytes, (uint8_t *) "123", 3);

  ck_assert_int_eq(3, data->length);
  ck_assert(strncmp("123", (char *) data->bytes, 3) == 0);

  $(data, setLength, 128);
  ck_assert_int_eq(128, data->length);
  ck_assert_int_eq(0, data->bytes[data->length - 1]);

  ident mem = malloc(8192 * sizeof(uint8_t));
  ck_assert(mem != NULL);

  memset(mem, 1, 8192 * sizeof(uint8_t));

  Data *append = $(alloc(Data), initWithMemory, mem, 8192);
  ck_assert(append != NULL);

  $(data, appendData, append);
  release(append);

  ck_assert_int_eq(8192 + 128, data->length);
  ck_assert_int_eq(1, data->bytes[data->length - 1]);

  release(data);

} END_TEST


int main(int argc, char **argv) {

  TCase *tcase = tcase_create("Data");
  tcase_add_test(tcase, data);

  tcase_add_test(tcase, data_mutation);

  Suite *suite = suite_create("Data");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
