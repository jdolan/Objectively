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

#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>

#include "Dictionary.h"
#include "Hash.h"
#include "String.h"
#include "Null.h"

#define _Class _Dictionary

#define DICTIONARY_DEFAULT_CAPACITY 64
#define DICTIONARY_GROW_FACTOR 2.0
#define DICTIONARY_MAX_LOAD 0.75f

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const Dictionary *this = (const Dictionary *) self;

  Dictionary *that = $(alloc(Dictionary), initWithCapacity, this->capacity);
  if (that) {
    $(that, addEntriesFromDictionary, this);
  }

  return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Dictionary *this = (Dictionary *) self;

  for (size_t i = 0; i < this->capacity; i++) {
    release(this->elements[i]);
  }

  free(this->elements);

  super(Object, self, dealloc);
}

/**
 * @brief A DictionaryEnumerator for description.
 */
static void description_enumerator(const Dictionary *dict, ident obj, ident key, ident data) {

  String *desc = (String *) data;

  String *objDesc = $((Object *) obj, description);
  String *keyDesc = $((Object *) key, description);

  $(desc, appendFormat, "%s: %s, ", keyDesc->chars, objDesc->chars);

  release(objDesc);
  release(keyDesc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  const Dictionary *this = (Dictionary *) self;

  String *desc = $(alloc(String), init);

  $(desc, appendCharacters, "{");

  $(this, enumerateObjectsAndKeys, description_enumerator, desc);

  $(desc, appendCharacters, "}");

  return (String *) desc;
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  const Dictionary *this = (Dictionary *) self;

  int hash = HashForInteger(HASH_SEED, this->count);

  for (size_t i = 0; i < this->capacity; i++) {
    if (this->elements[i]) {
      hash = HashForObject(hash, this->elements[i]);
    }
  }

  return hash;
}

/**
 * @see Object::isEqual(const Object *, const Object *)
 */
static bool isEqual(const Object *self, const Object *other) {

  if (super(Object, self, isEqual, other)) {
    return true;
  }

  if (other && $(other, isKindOfClass, _Dictionary())) {

    const Dictionary *this = (Dictionary *) self;
    const Dictionary *that = (Dictionary *) other;

    if (this->count == that->count) {

      Array *keys = $(this, allKeys);

      for (size_t i = 0; i < keys->count; i++) {
        const ident key = $(keys, objectAtIndex, i);

        const Object *thisObject = $(this, objectForKey, key);
        const Object *thatObject = $(that, objectForKey, key);

        if ($(thisObject, isEqual, thatObject) == false) {
          release(keys);
          return false;
        }
      }

      release(keys);
      return true;
    }
  }

  return false;
}

#pragma mark - Dictionary

/**
 * @brief DictionaryEnumerator for addEntriesFromDictionary.
 */
static void addEntriesFromDictionary_enumerator(const Dictionary *dict, ident obj, ident key, ident data) {
  $((Dictionary *) data, setObjectForKey, obj, key);
}

/**
 * @fn void Dictionary::addEntriesFromDictionary(Dictionary *self, const Dictionary *dictionary)
 * @memberof Dictionary
 */
static void addEntriesFromDictionary(Dictionary *self, const Dictionary *dictionary) {

  assert(dictionary);

  $(dictionary, enumerateObjectsAndKeys, addEntriesFromDictionary_enumerator, self);
}

/**
 * @brief DictionaryEnumerator for allKeys.
 */
static void allKeys_enumerator(const Dictionary *dict, ident obj, ident key, ident data) {
  $((Array *) data, addObject, key);
}

/**
 * @fn Array *Dictionary::allKeys(const Dictionary *self)
 * @memberof Dictionary
 */
static Array *allKeys(const Dictionary *self) {

  Array *keys = $(alloc(Array), initWithCapacity, self->count);

  $(self, enumerateObjectsAndKeys, allKeys_enumerator, keys);

  return (Array *) keys;
}

/**
 * @brief DictionaryEnumerator for allObjects.
 */
static void allObjects_enumerator(const Dictionary *dict, ident obj, ident key, ident data) {
  $((Array *) data, addObject, obj);
}

/**
 * @fn Array *Dictionary::allObjects(const Dictionary *self)
 * @memberof Dictionary
 */
static Array *allObjects(const Dictionary *self) {

  Array *objects = $(alloc(Array), initWithCapacity, self->count);

  $(self, enumerateObjectsAndKeys, allObjects_enumerator, objects);

  return (Array *) objects;
}

/**
 * @fn bool Dictionary::containsKey(const Dictionary *self, const ident key)
 * @memberof Dictionary
 */
static bool containsKey(const Dictionary *self, const ident key) {
  return $(self, objectForKey, key) != NULL;
}

/**
 * @fn bool Dictionary::containsKeyPath(const Dictionary *self, const char *path)
 * @memberof Dictionary
 */
static bool containsKeyPath(const Dictionary *self, const char *path) {
  return $(self, objectForKeyPath, path) != NULL;
}

/**
 * @fn Dictionary *Dictionary::dictionary(void)
 * @memberof Dictionary
 */
static Dictionary *dictionary(void) {

  return $(alloc(Dictionary), init);
}

/**
 * @fn Dictionary *Dictionary::dictionaryWithCapacity(size_t capacity)
 * @memberof Dictionary
 */
static Dictionary *dictionaryWithCapacity(size_t capacity) {

  return $(alloc(Dictionary), initWithCapacity, capacity);
}

/**
 * @fn Dictionary *Dictionary::dictionaryWithDictionary(const Dictionary *dictionary)
 * @memberof Dictionary
 */
static Dictionary *dictionaryWithDictionary(const Dictionary *dictionary) {

  return $(alloc(Dictionary), initWithDictionary, dictionary);
}

/**
 * @fn Dictionary *Dictionary::dictionaryWithObjectsAndKeys(ident obj, ...)
 * @memberof Dictionary
 */
static Dictionary *dictionaryWithObjectsAndKeys(ident obj, ...) {

  Dictionary *dict = (Dictionary *) $((Object *) alloc(Dictionary), init);
  if (dict) {

    va_list args;
    va_start(args, obj);

    while (obj) {
      ident key = va_arg(args, ident);

      $(dict, setObjectForKey, obj, key);

      obj = va_arg(args, ident);
    }

    va_end(args);
  }

  return dict;
}

/**
 * @fn void Dictionary::enumerateObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator, ident data)
 * @memberof Dictionary
 */
static void enumerateObjectsAndKeys(const Dictionary *self, DictionaryEnumerator enumerator,
    ident data) {

  assert(enumerator);

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j += 2) {

        ident key = $(array, objectAtIndex, j);
        ident obj = $(array, objectAtIndex, j + 1);

        enumerator(self, obj, key, data);
      }
    }
  }
}

/**
 * @fn Dicionary *Dictionary::filterObjectsAndKeys(const Dictionary *self, DictionaryPredicate predicate, ident data)
 * @memberof Dictionary
 */
static Dictionary *filterObjectsAndKeys(const Dictionary *self, DictionaryPredicate predicate, ident data) {

  assert(predicate);

  Dictionary *dictionary = $(alloc(Dictionary), init);

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j += 2) {

        ident key = $(array, objectAtIndex, j);
        ident obj = $(array, objectAtIndex, j + 1);

        if (predicate(obj, key, data)) {
          $(dictionary, setObjectForKey, obj, key);
        }
      }
    }
  }

  return dictionary;
}

/**
 * @fn Dictionary *Dictionary::init(Dictionary *self)
 * @memberof Dictionary
 */
static Dictionary *init(Dictionary *self) {

  return $(self, initWithCapacity, DICTIONARY_DEFAULT_CAPACITY);
}

/**
 * @fn Dictionary *Dictionary::initWithCapacity(Dictionary *self, size_t capacity)
 * @memberof Dictionary
 */
static Dictionary *initWithCapacity(Dictionary *self, size_t capacity) {

  self = (Dictionary *) super(Object, self, init);
  if (self) {

    self->capacity = capacity;
    if (self->capacity) {

      self->elements = calloc(self->capacity, sizeof(ident));
      assert(self->elements);
    }
  }

  return self;
}

/**
 * @fn Dictionary *Dictionary::initWithDictionary(Dictionary *self, const Dictionary *dictionary)
 * @memberof Dictionary
 */
static Dictionary *initWithDictionary(Dictionary *self, const Dictionary *dictionary) {

  self = (Dictionary *) super(Object, self, init);
  if (self) {
    if (dictionary) {

      self->capacity = dictionary->capacity;

      self->elements = calloc(self->capacity, sizeof(ident));
      assert(self->elements);

      for (size_t i = 0; i < dictionary->capacity; i++) {

        Array *array = dictionary->elements[i];
        if (array) {
          self->elements[i] = $((Object *) array, copy);
        }
      }

      self->count = dictionary->count;
    }
  }

  return self;
}

/**
 * @fn Dictionary *Dictionary::initWithObjectsAndKeys(Dictionary *self, ...)
 * @memberof Dictionary
 */
static Dictionary *initWithObjectsAndKeys(Dictionary *self, ...) {

  self = (Dictionary *) super(Object, self, init);
  if (self) {

    va_list args;
    va_start(args, self);

    while (true) {

      ident obj = va_arg(args, ident);
      if (obj) {

        ident key = va_arg(args, ident);
        $(self, setObjectForKey, obj, key);
      } else {
        break;
      }
    }

    va_end(args);
  }

  return self;
}

/**
 * @fn ident Dictionary::objectForKey(const Dictionary *self, const ident key)
 * @memberof Dictionary
 */
static ident objectForKey(const Dictionary *self, const ident key) {

  if (self->capacity == 0) {
    return NULL;
  }
  
  const size_t bin = HashForObject(HASH_SEED, key) % self->capacity;

  Array *array = self->elements[bin];
  if (array) {

    const ssize_t index = $(array, indexOfObject, key);
    if (index > -1 && (index & 1) == 0) {
      return $(array, objectAtIndex, index + 1);
    }
  }

  return NULL;
}

/**
 * @fn ident Dictionary::objectForKeyPath(const Dictionary *self, const char *path)
 * @memberof Dictionary
 */
static ident objectForKeyPath(const Dictionary *self, const char *path) {

  assert(path);

  String *key = str(path);

  ident obj = $(self, objectForKey, key);

  release(key);

  return obj;
}

/**
 * @fn ident Dictionary::objectForKeyPathWithClass(const Dictionary *self, const char *path, const Class *clazz)
 * @memberof Dictionary
 */
static ident objectForKeyPathWithClass(const Dictionary *self, const char *path, const Class *clazz) {

  assert(path);

  const ident value = $(self, objectForKeyPath, path);
  if (value) {
    if (instanceof(Null, value)) {
      return NULL;
    } else {
      return _cast(clazz, value);
    }
  }

  return NULL;
}

/**
 * @fn void Dictionary::removeAllObjects(Dictionary *self)
 * @memberof Dictionary
 */
static void removeAllObjects(Dictionary *self) {

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {
      self->elements[i] = release(array);
    }
  }

  self->count = 0;
}

/**
 * @fn void Dictionary::removeAllObjectsWithEnumerator(Dictionary *self, DictionaryEnumerator enumerator, ident data)
 * @memberof Dictionary
 */
static void removeAllObjectsWithEnumerator(Dictionary *self, DictionaryEnumerator enumerator, ident data) {

  assert(enumerator);

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {
      for (size_t j = array->count; j > 0; j -= 2) {

        ident obj = array->elements[j - 1];
        ident key = array->elements[j - 2];

        enumerator(self, obj, key, data);

        $((Array *) array, removeObjectAtIndex, j - 1);
        $((Array *) array, removeObjectAtIndex, j - 2);
      }

      self->elements[i] = release(array);
    }
  }

  self->count = 0;
}

/**
 * @fn void Dictionary::removeObjectForKey(Dictionary *self, const ident key)
 * @memberof Dictionary
 */
static void removeObjectForKey(Dictionary *self, const ident key) {

  if (self->capacity == 0) {
    return;
  }
  
  const size_t bin = HashForObject(HASH_SEED, key) % self->capacity;

  Array *array = self->elements[bin];
  if (array) {

    const ssize_t index = $((Array *) array, indexOfObject, key);
    if (index > -1) {

      $(array, removeObjectAtIndex, index);
      $(array, removeObjectAtIndex, index);

      if (((Array *) array)->count == 0) {
        self->elements[bin] = release(array);
      }

      self->count--;
    }
  }
}

/**
 * @fn void Dictionary::removeObjectForKeyPath(Dictionary *self, const char *path)
 * @memberof Dictionary
 */
static void removeObjectForKeyPath(Dictionary *self, const char *path) {

  String *key = $$(String, stringWithCharacters, path);

  $(self, removeObjectForKey, key);

  release(key);
}

/**
 * @brief A helper for resizing Dictionaries as pairs are added to them.
 * @remarks Static method invocations are used for all operations.
 */
static void setObjectForKey_resize(Dictionary *dict) {

  if (dict->capacity) {

    const float load = dict->count / (float) dict->capacity;
    if (load >= DICTIONARY_MAX_LOAD) {

      size_t capacity = dict->capacity;
      ident *elements = dict->elements;

      dict->capacity = dict->capacity * DICTIONARY_GROW_FACTOR;
      dict->count = 0;

      dict->elements = calloc(dict->capacity, sizeof(ident));
      assert(dict->elements);

      for (size_t i = 0; i < capacity; i++) {

        Array *array = elements[i];
        if (array) {

          for (size_t j = 0; j < array->count; j += 2) {

            ident key = $(array, objectAtIndex, j);
            ident obj = $(array, objectAtIndex, j + 1);

            $$(Dictionary, setObjectForKey, dict, obj, key);
          }

          release(array);
        }
      }

      free(elements);
    }
  } else {
    $(dict, initWithCapacity, DICTIONARY_DEFAULT_CAPACITY);
  }
}

/**
 * @fn void Dictionary::setObjectForKey(Dictionary *self, const ident obj, const ident key)
 * @memberof Dictionary
 */
static void setObjectForKey(Dictionary *self, const ident obj, const ident key) {

  Dictionary *dict = self;

  setObjectForKey_resize(dict);

  const size_t bin = HashForObject(HASH_SEED, key) % dict->capacity;

  Array *array = dict->elements[bin];
  if (array == NULL) {
    array = dict->elements[bin] = $$(Array, arrayWithCapacity, (dict->capacity >> 2) + 1);
  }

  const ssize_t index = $((Array *) array, indexOfObject, key);
  if (index > -1 && (index & 1) == 0) {
    $(array, setObjectAtIndex, obj, index + 1);
  } else {
    $(array, addObject, key);
    $(array, addObject, obj);

    dict->count++;
  }
}

/**
 * @fn void Dictionary::setObjectForKeyPath(Dictionary *self, const ident obj, const char *path)
 * @memberof Dictionary
 */
static void setObjectForKeyPath(Dictionary *self, const ident obj, const char *path) {

  String *key = $$(String, stringWithCharacters, path);

  $(self, setObjectForKey, obj, key);

  release(key);
}

/**
 * @fn void Dictionary::setObjectsForKeyPaths(Dictionary *self, ...)
 * @memberof Dictionary
 */
static void setObjectsForKeyPaths(Dictionary *self, ...) {

  va_list args;
  va_start(args, self);

  while (true) {

    ident obj = va_arg(args, ident);
    if (obj) {
      const char *path = va_arg(args, const char *);
      $(self, setObjectForKeyPath, obj, path);
    } else {
      break;
    }
  }

  va_end(args);
}

/**
 * @fn void Dictionary::setObjectsForKeys(Dictionary *self, ...)
 * @memberof Dictionary
 */
static void setObjectsForKeys(Dictionary *self, ...) {

  va_list args;
  va_start(args, self);

  while (true) {

    ident obj = va_arg(args, ident);
    if (obj) {
      ident key = va_arg(args, ident);
      $(self, setObjectForKey, obj, key);
    } else {
      break;
    }
  }

  va_end(args);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->copy = copy;
  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;
  ((ObjectInterface *) clazz->interface)->description = description;
  ((ObjectInterface *) clazz->interface)->hash = hash;
  ((ObjectInterface *) clazz->interface)->isEqual = isEqual;

  ((DictionaryInterface *) clazz->interface)->addEntriesFromDictionary = addEntriesFromDictionary;
  ((DictionaryInterface *) clazz->interface)->allKeys = allKeys;
  ((DictionaryInterface *) clazz->interface)->allObjects = allObjects;
  ((DictionaryInterface *) clazz->interface)->containsKey = containsKey;
  ((DictionaryInterface *) clazz->interface)->containsKeyPath = containsKeyPath;
  ((DictionaryInterface *) clazz->interface)->dictionary = dictionary;
  ((DictionaryInterface *) clazz->interface)->dictionaryWithCapacity = dictionaryWithCapacity;
  ((DictionaryInterface *) clazz->interface)->dictionaryWithDictionary = dictionaryWithDictionary;
  ((DictionaryInterface *) clazz->interface)->dictionaryWithObjectsAndKeys = dictionaryWithObjectsAndKeys;
  ((DictionaryInterface *) clazz->interface)->enumerateObjectsAndKeys = enumerateObjectsAndKeys;
  ((DictionaryInterface *) clazz->interface)->filterObjectsAndKeys = filterObjectsAndKeys;
  ((DictionaryInterface *) clazz->interface)->init = init;
  ((DictionaryInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((DictionaryInterface *) clazz->interface)->initWithDictionary = initWithDictionary;
  ((DictionaryInterface *) clazz->interface)->initWithObjectsAndKeys = initWithObjectsAndKeys;
  ((DictionaryInterface *) clazz->interface)->objectForKey = objectForKey;
  ((DictionaryInterface *) clazz->interface)->objectForKeyPath = objectForKeyPath;
  ((DictionaryInterface *) clazz->interface)->objectForKeyPathWithClass = objectForKeyPathWithClass;
  ((DictionaryInterface *) clazz->interface)->removeAllObjects = removeAllObjects;
  ((DictionaryInterface *) clazz->interface)->removeAllObjectsWithEnumerator = removeAllObjectsWithEnumerator;
  ((DictionaryInterface *) clazz->interface)->removeObjectForKey = removeObjectForKey;
  ((DictionaryInterface *) clazz->interface)->removeObjectForKeyPath = removeObjectForKeyPath;
  ((DictionaryInterface *) clazz->interface)->setObjectForKey = setObjectForKey;
  ((DictionaryInterface *) clazz->interface)->setObjectForKeyPath = setObjectForKeyPath;
  ((DictionaryInterface *) clazz->interface)->setObjectsForKeyPaths = setObjectsForKeyPaths;
  ((DictionaryInterface *) clazz->interface)->setObjectsForKeys = setObjectsForKeys;
}

/**
 * @fn Class *Dictionary::_Dictionary(void)
 * @memberof Dictionary
 */
Class *_Dictionary(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Dictionary",
      .superclass = _Object(),
      .instanceSize = sizeof(Dictionary),
      .interfaceOffset = offsetof(Dictionary, interface),
      .interfaceSize = sizeof(DictionaryInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
