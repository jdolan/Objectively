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

/**
 * @brief Verifies that dataFromObject handles an Array at the top level.
 */
START_TEST(json_array_toplevel) {

  MutableArray *arr = $(alloc(MutableArray), init);

  String *s1 = $$(String, stringWithCharacters, "hello");
  String *s2 = $$(String, stringWithCharacters, "world");
  $(arr, addObject, s1);
  $(arr, addObject, s2);
  release(s1);
  release(s2);

  Data *data = $$(JSONSerialization, dataFromObject, arr, 0);
  ck_assert_ptr_ne(NULL, data);

  // Should start with '[' and end with ']'
  ck_assert_int_eq('[', ((const char *) data->bytes)[0]);
  ck_assert_int_eq(']', ((const char *) data->bytes)[data->length - 1]);

  // Round-trip: parse back
  Array *parsed = $$(JSONSerialization, objectFromData, data, 0);
  ck_assert_ptr_ne(NULL, parsed);
  ck_assert_int_eq(2, (int) parsed->count);

  String *r0 = $(parsed, objectAtIndex, 0);
  String *r1 = $(parsed, objectAtIndex, 1);
  ck_assert_str_eq("hello", r0->chars);
  ck_assert_str_eq("world", r1->chars);

  release(data);
  release(arr);
  release(parsed);

} END_TEST

/**
 * @brief Tests dictionaryFromStruct and dataFromStructs with a representative C struct.
 */
START_TEST(json_struct_properties) {

  typedef struct {
    char     name[64];
    char     *tag;
    int32_t  score;
    double   ratio;
    bool     active;
  } TestStruct;

  static const JsonProperty properties[] = MakeJsonProperties(
    MakeJsonProperty("name",   JsonPropertyTypeCharacters, offsetof(TestStruct, name),   sizeof(((TestStruct *)0)->name)),
    MakeJsonProperty("tag",    JsonPropertyTypeCharacters, offsetof(TestStruct, tag),    0),
    MakeJsonProperty("score",  JsonPropertyTypeInteger,    offsetof(TestStruct, score),  sizeof(((TestStruct *)0)->score)),
    MakeJsonProperty("ratio",  JsonPropertyTypeDouble,     offsetof(TestStruct, ratio),  sizeof(((TestStruct *)0)->ratio)),
    MakeJsonProperty("active", JsonPropertyTypeBool,       offsetof(TestStruct, active), sizeof(((TestStruct *)0)->active))
  );

  TestStruct instances[2] = {
    { .name = "Alice", .tag = "hero",   .score = 42,  .ratio = 0.75, .active = true  },
    { .name = "Bob",   .tag = "villain",.score = -7,  .ratio = 0.25, .active = false },
  };

  // dictionaryFromStruct — single instance
  Dictionary *dict = $$(JSONSerialization, dictionaryFromStruct, properties, &instances[0]);
  ck_assert_ptr_ne(NULL, dict);

  String *kName   = $$(String, stringWithCharacters, "name");
  String *kTag    = $$(String, stringWithCharacters, "tag");
  String *kScore  = $$(String, stringWithCharacters, "score");
  String *kRatio  = $$(String, stringWithCharacters, "ratio");
  String *kActive = $$(String, stringWithCharacters, "active");

  String *vName = $(dict, objectForKey, kName);
  ck_assert_str_eq("Alice", vName->chars);

  String *vTag = $(dict, objectForKey, kTag);
  ck_assert_str_eq("hero", vTag->chars);

  Number *vScore = $(dict, objectForKey, kScore);
  ck_assert_int_eq(42, (int) vScore->value);

  Number *vRatio = $(dict, objectForKey, kRatio);
  ck_assert(vRatio->value > 0.74 && vRatio->value < 0.76);

  Boole *vActive = $(dict, objectForKey, kActive);
  ck_assert(vActive->value);

  release(kName); release(kTag); release(kScore); release(kRatio); release(kActive);
  release(dict);

  // dataFromStructs — two instances, round-trip
  Data *data = $$(JSONSerialization, dataFromStructs, properties, instances, 2, sizeof(TestStruct));
  ck_assert_ptr_ne(NULL, data);
  ck_assert_int_eq('[', ((const char *) data->bytes)[0]);

  Array *parsed = $$(JSONSerialization, objectFromData, data, 0);
  ck_assert_ptr_ne(NULL, parsed);
  ck_assert_int_eq(2, (int) parsed->count);

  Dictionary *d0 = $(parsed, objectAtIndex, 0);
  Dictionary *d1 = $(parsed, objectAtIndex, 1);

  String *k = $$(String, stringWithCharacters, "name");
  ck_assert_str_eq("Alice", ((String *) $(d0, objectForKey, k))->chars);
  ck_assert_str_eq("Bob",   ((String *) $(d1, objectForKey, k))->chars);
  release(k);

  k = $$(String, stringWithCharacters, "active");
  ck_assert(((Boole *) $(d0, objectForKey, k))->value == true);
  ck_assert(((Boole *) $(d1, objectForKey, k))->value == false);
  release(k);

  release(data);
  release(parsed);

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
  tcase_add_test(tcase, json_array_toplevel);
  tcase_add_test(tcase, json_struct_properties);

  Suite *suite = suite_create("Json");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
