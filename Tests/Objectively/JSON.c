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

/**
 * @brief Verifies that writeString escapes special characters and that
 * readString correctly round-trips them back.
 */
START_TEST(json_escaping) {

  static const struct escape_case {
    const char *key;
    const char *value;
  } cases[] = {
    { "quote",      "say \"hello\""          },  // embedded double-quote
    { "backslash",  "C:\\Users\\player"       },  // backslashes
    { "newline",    "line1\nline2"            },  // \n
    { "tab",        "col1\tcol2"              },  // \t
    { "cr",         "carriage\rreturn"        },  // \r
    { "backspace",  "back\bspace"             },  // \b
    { "formfeed",   "form\ffeed"              },  // \f
    { "control",    "esc\033char"             },  // raw control char (ESC, \u001b)
    { "utf8",       "\xc3\xa9l\xc3\xa8ve"    },  // UTF-8 passthrough (éléve)
    { "combined",   "\"\\n\t\r\b\f\001"      },  // several specials in one value
  };

  // Build a Dictionary from the test cases and serialize it.
  MutableDictionary *dict0 = $(alloc(MutableDictionary), init);
  for (size_t i = 0; i < lengthof(cases); i++) {
    String *key = $$(String, stringWithCharacters, cases[i].key);
    String *val = $$(String, stringWithCharacters, cases[i].value);
    $(dict0, setObjectForKey, val, key);
    release(key);
    release(val);
  }

  Data *data = $$(JSONSerialization, dataFromObject, dict0, 0);
  ck_assert_ptr_ne(NULL, data);

  // The serialized bytes must not contain any raw unescaped control characters.
  for (size_t i = 0; i < data->length; i++) {
    unsigned char b = ((unsigned char *) data->bytes)[i];
    ck_assert_msg(b >= 0x20 || b == '\0', "raw control char 0x%02x at offset %zu", b, i);
  }

  // Round-trip: deserialize and verify every value survives intact.
  Dictionary *dict1 = $$(JSONSerialization, objectFromData, data, 0);
  ck_assert_ptr_ne(NULL, dict1);

  for (size_t i = 0; i < lengthof(cases); i++) {
    String *key = $$(String, stringWithCharacters, cases[i].key);
    String *val = $(dict1, objectForKey, key);
    ck_assert_ptr_ne(NULL, val);
    ck_assert_str_eq(cases[i].value, val->chars);
    release(key);
  }

  release(data);
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
  tcase_add_test(tcase, json_escaping);

  Suite *suite = suite_create("Json");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
