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

static char *path;

START_TEST(json) {
  
  Data *data0 = $(alloc(Data), initWithContentsOfFile, path);
  ck_assert_ptr_ne(NULL, data0);

  Dictionary *dict0 = $$(JSONSerialization, objectFromData, data0, 0);
  ck_assert(dict0->count);

  Data *data1 = $$(JSONSerialization, dataFromObject, dict0, JSON_WRITE_PRETTY | JSON_WRITE_SORTED);
  ck_assert_ptr_ne(NULL, data1);
  ck_assert($((Object *) data0, isEqual, (Object *) data1));

  Dictionary *dict1 = $$(JSONSerialization, objectFromData, data1, 0);
  ck_assert_int_eq(dict0->count, dict1->count);
  ck_assert($((Object *) dict0, isEqual, (Object *) dict1));

  Object *notFound = $$(JSONPath, objectForKeyPath, dict0, "$.notFound");
  ck_assert_ptr_eq(NULL, notFound);

  Object *root = $$(JSONPath, objectForKeyPath, dict0, "$");
  ck_assert_ptr_eq(dict0, root);

  Number *dataStoreInitConns = $$(JSONPath, objectForKeyPath, dict0, "$.web-app.servlet[0].init-param.dataStoreInitConns");
  ck_assert(dataStoreInitConns != NULL);
  ck_assert_int_eq(10, (int) dataStoreInitConns->value);

  release(data0);
  release(data1);
  release(dict0);
  release(dict1);

} END_TEST

int main(int argc, char **argv) {

  if (argc == 2) {
    path = argv[1];
  } else {
    path = "Fixtures/test.json";
  }

  TCase *tcase = tcase_create("Json");
  tcase_add_test(tcase, json);

  Suite *suite = suite_create("Json");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
