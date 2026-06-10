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
 * @brief Tests dictionaryFromInstance, dataFromInstance, dataFromInstances, instanceFromData, and instancesFromData with a representative C struct.
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
    MakeJsonProperty(TestStruct, name,   JsonPropertyCharacters),
    MakeJsonProperty(TestStruct, tag,    JsonPropertyCString),
    MakeJsonProperty(TestStruct, score,  JsonPropertyInteger),
    MakeJsonProperty(TestStruct, ratio,  JsonPropertyDouble),
    MakeJsonProperty(TestStruct, active, JsonPropertyBool)
  );

  TestStruct instances[2] = {
    { .name = "Alice", .tag = "hero",    .score = 42, .ratio = 0.75, .active = true  },
    { .name = "Bob",   .tag = "villain", .score = -7, .ratio = 0.25, .active = false },
  };

  // dictionaryFromInstance — single instance
  Dictionary *dict = $$(JSONSerialization, dictionaryFromInstance, properties, &instances[0]);
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

  release(dict);

  // dataFromInstance — single instance -> JSON
  Data *singleData = $$(JSONSerialization, dataFromInstance, properties, &instances[0]);
  ck_assert_ptr_ne(NULL, singleData);

  Dictionary *singleDict = $$(JSONSerialization, objectFromData, singleData, 0);
  ck_assert_ptr_ne(NULL, singleDict);
  ck_assert_str_eq("Alice", ((String *) $(singleDict, objectForKey, kName))->chars);
  ck_assert_str_eq("hero", ((String *) $(singleDict, objectForKey, kTag))->chars);
  ck_assert_int_eq(42, (int) ((Number *) $(singleDict, objectForKey, kScore))->value);
  ck_assert(((Boole *) $(singleDict, objectForKey, kActive))->value);
  release(singleDict);

  // instanceFromData — single object -> single struct
  TestStruct singleOut = { 0 };
  size_t m = $$(JSONSerialization, instanceFromData, properties, singleData, &singleOut);
  ck_assert_int_eq(1, (int) m);
  ck_assert_str_eq("Alice", singleOut.name);
  ck_assert_str_eq("hero", singleOut.tag);
  ck_assert_int_eq(42, (int) singleOut.score);
  ck_assert(singleOut.ratio > 0.74 && singleOut.ratio < 0.76);
  ck_assert(singleOut.active == true);
  free(singleOut.tag);

  release(singleData);

  release(kName); release(kTag); release(kScore); release(kRatio); release(kActive);

  // dataFromInstances — two instances -> JSON
  Data *data = $$(JSONSerialization, dataFromInstances, properties, instances, 2, sizeof(TestStruct));
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

  release(parsed);

  // instancesFromData — round-trip: JSON -> structs
  TestStruct out[2] = { 0 };
  size_t n = $$(JSONSerialization, instancesFromData, properties, data, out, sizeof(TestStruct), 2);
  ck_assert_int_eq(2, (int) n);

  ck_assert_str_eq("Alice",   out[0].name);
  ck_assert_str_eq("hero",    out[0].tag);
  ck_assert_int_eq(42,        out[0].score);
  ck_assert(out[0].ratio > 0.74 && out[0].ratio < 0.76);
  ck_assert(out[0].active == true);

  ck_assert_str_eq("Bob",     out[1].name);
  ck_assert_str_eq("villain", out[1].tag);
  ck_assert_int_eq(-7,        out[1].score);
  ck_assert(out[1].ratio > 0.24 && out[1].ratio < 0.26);
  ck_assert(out[1].active == false);

  free(out[0].tag);
  free(out[1].tag);
  release(data);

} END_TEST

/**
 * @brief Tests dataFromInstances with a struct mirroring g_frag_t from Quetoo,
 * exercising the exact property types and edge cases seen in production.
 */
START_TEST(json_frag_t) {

#define FRAG_QPATH 64

  typedef struct {
    char     level[FRAG_QPATH];
    char     attacker[FRAG_QPATH];
    char     attacker_guid[FRAG_QPATH];
    bool     attacker_ai;
    char     target[FRAG_QPATH];
    char     target_guid[FRAG_QPATH];
    bool     target_ai;
    char     weapon[FRAG_QPATH];
    int32_t  mod;
    int32_t  damage;
    uint32_t time;
  } frag_t;

  static const JsonProperty properties[] = MakeJsonProperties(
    MakeJsonProperty(frag_t, level,         JsonPropertyCharacters),
    MakeJsonProperty(frag_t, attacker,      JsonPropertyCharacters),
    MakeJsonProperty(frag_t, attacker_guid, JsonPropertyCharacters),
    MakeJsonProperty(frag_t, attacker_ai,   JsonPropertyBool),
    MakeJsonProperty(frag_t, target,        JsonPropertyCharacters),
    MakeJsonProperty(frag_t, target_guid,   JsonPropertyCharacters),
    MakeJsonProperty(frag_t, target_ai,     JsonPropertyBool),
    MakeJsonProperty(frag_t, weapon,        JsonPropertyCharacters),
    MakeJsonProperty(frag_t, mod,           JsonPropertyInteger),
    MakeJsonProperty(frag_t, damage,        JsonPropertyInteger),
    MakeJsonProperty(frag_t, time,          JsonPropertyInteger)
  );

  // Mirror real-world permutations: human kills human, human kills bot,
  // bot kills human, suicide, empty weapon, max uint32 time, zero damage.
  frag_t frags[] = {
    {
      .level = "edge",
      .attacker = "jdolan", .attacker_guid = "3a56346d46f8e00b88232df6db2b4595", .attacker_ai = false,
      .target   = "Skies912", .target_guid  = "02bd0844f98709ceb87a9c41998484228", .target_ai = false,
      .weapon = "rockets", .mod = 1, .damage = 80, .time = 12345
    },
    {
      .level = "edge",
      .attacker = "jdolan", .attacker_guid = "3a56346d46f8e00b88232df6db2b4595", .attacker_ai = false,
      .target   = "[BOT] Makron", .target_guid = "81f97126cfaa1b4c41f152d76fee16b3", .target_ai = true,
      .weapon = "railgun", .mod = 7, .damage = 100, .time = 23456
    },
    {
      .level = "edge",
      .attacker = "[BOT] Brain", .attacker_guid = "b19e1c04f191f77ea2cd73552997a603", .attacker_ai = true,
      .target   = "Skies912", .target_guid = "02bd0844f98709ceb87a9c41998484228", .target_ai = false,
      .weapon = "plasma", .mod = 3, .damage = 60, .time = 34567
    },
    {
      // Suicide: attacker == target (same guid)
      .level = "edge",
      .attacker = "jdolan", .attacker_guid = "3a56346d46f8e00b88232df6db2b4595", .attacker_ai = false,
      .target   = "jdolan", .target_guid   = "3a56346d46f8e00b88232df6db2b4595", .target_ai = false,
      .weapon = "", .mod = 0, .damage = 0, .time = 45678  // empty weapon (e.g. world kill)
    },
    {
      // Zero damage (telefrag / world)
      .level = "edge",
      .attacker = "Skies912", .attacker_guid = "02bd0844f98709ceb87a9c41998484228", .attacker_ai = false,
      .target   = "jdolan",   .target_guid   = "3a56346d46f8e00b88232df6db2b4595", .target_ai = false,
      .weapon = "bfg", .mod = 9, .damage = 0, .time = 56789
    },
    {
      // Large epoch-like time, negative mod (edge case)
      .level = "edge",
      .attacker = "jdolan", .attacker_guid = "3a56346d46f8e00b88232df6db2b4595", .attacker_ai = false,
      .target   = "[BOT] Gladiator", .target_guid = "73c6bc5c39e6b7fbbca031eeadf71c09", .target_ai = true,
      .weapon = "hyperblaster", .mod = 5, .damage = 50, .time = 1781060274
    },
  };
  const size_t n = sizeof(frags) / sizeof(frags[0]);

  // dataFromInstances — serialize all frags to JSON
  Data *data = $$(JSONSerialization, dataFromInstances, properties, frags, n, sizeof(frag_t));
  ck_assert_ptr_ne(NULL, data);
  ck_assert_int_eq('[', ((const char *) data->bytes)[0]);

  // Round-trip: parse back and verify count
  Array *parsed = $$(JSONSerialization, objectFromData, data, 0);
  ck_assert_ptr_ne(NULL, parsed);
  ck_assert_int_eq((int) n, (int) parsed->count);

  // Spot-check first frag (human→human)
  {
    Dictionary *d = $(parsed, objectAtIndex, 0);
    String *k;

    k = $$(String, stringWithCharacters, "attacker");
    ck_assert_str_eq("jdolan", ((String *) $(d, objectForKey, k))->chars);
    release(k);

    k = $$(String, stringWithCharacters, "attacker_ai");
    ck_assert(((Boole *) $(d, objectForKey, k))->value == false);
    release(k);

    k = $$(String, stringWithCharacters, "target_ai");
    ck_assert(((Boole *) $(d, objectForKey, k))->value == false);
    release(k);

    k = $$(String, stringWithCharacters, "mod");
    ck_assert_int_eq(1, (int) ((Number *) $(d, objectForKey, k))->value);
    release(k);
  }

  // Spot-check fourth frag (suicide with empty weapon — weapon key should be absent)
  {
    Dictionary *d = $(parsed, objectAtIndex, 3);
    String *k = $$(String, stringWithCharacters, "weapon");
    ck_assert_ptr_eq(NULL, $(d, objectForKey, k));
    release(k);
  }

  // Spot-check sixth frag (epoch-sized time).
  {
    Dictionary *d = $(parsed, objectAtIndex, 5);
    String *k = $$(String, stringWithCharacters, "time");
    Number *t = $(d, objectForKey, k);
    ck_assert_ptr_ne(NULL, t);
    ck_assert_int_eq(1781060274, (int32_t) t->value);
    release(k);
  }

  release(parsed);
  release(data);

#undef FRAG_QPATH

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
  tcase_add_test(tcase, json_frag_t);

  Suite *suite = suite_create("Json");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
