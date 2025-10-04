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

#include "Hash.h"
#include "MutableArray.h"
#include "MutableDictionary.h"
#include "String.h"

#define _Class _MutableDictionary

#define MUTABLEDICTIONARY_DEFAULT_CAPACITY 64
#define MUTABLEDICTIONARY_GROW_FACTOR 2.0
#define MUTABLEDICTIONARY_MAX_LOAD 0.75f

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const Dictionary *this = (const Dictionary *) self;

  MutableDictionary *copy = $(alloc(MutableDictionary), initWithCapacity, this->capacity);

  $(copy, addEntriesFromDictionary, this);

  return (Object *) copy;
}

#pragma mark - MutableDictionary

/**
 * @brief DictionaryEnumerator for addEntriesFromDictionary.
 */
static void addEntriesFromDictionary_enumerator(const Dictionary *dict, ident obj, ident key, ident data) {
  $((MutableDictionary *) data, setObjectForKey, obj, key);
}

/**
 * @fn void MutableDictionary::addEntriesFromDictionary(MutableDictionary *self, const Dictionary *dictionary)
 * @memberof MutableDictionary
 */
static void addEntriesFromDictionary(MutableDictionary *self, const Dictionary *dictionary) {

  assert(dictionary);

  $(dictionary, enumerateObjectsAndKeys, addEntriesFromDictionary_enumerator, self);
}

/**
 * @fn MutableDictionary *MutableDictionary::dictionary(void)
 * @memberof MutableDictionary
 */
static MutableDictionary *dictionary(void) {

  return $(alloc(MutableDictionary), init);
}

/**
 * @fn MutableDictionary *MutableDictionary::dictionaryWithCapacity(size_t capacity)
 * @memberof MutableDictionary
 */
static MutableDictionary *dictionaryWithCapacity(size_t capacity) {

  return $(alloc(MutableDictionary), initWithCapacity, capacity);
}

/**
 * @fn MutableDictionary *MutableDictionary::init(MutableDictionary *self)
 * @memberof MutableDictionary
 */
static MutableDictionary *init(MutableDictionary *self) {

  return $(self, initWithCapacity, MUTABLEDICTIONARY_DEFAULT_CAPACITY);
}

/**
 * @fn MutableDictionary *MutableDictionary::initWithCapacity(MutableDictionary *self, size_t capacity)
 * @memberof MutableDictionary
 */
static MutableDictionary *initWithCapacity(MutableDictionary *self, size_t capacity) {

  self = (MutableDictionary *) super(Object, self, init);
  if (self) {

    self->dictionary.capacity = capacity;
    if (self->dictionary.capacity) {

      self->dictionary.elements = calloc(self->dictionary.capacity, sizeof(ident));
      assert(self->dictionary.elements);
    }
  }

  return self;
}

/**
 * @fn void MutableDictionary::removeAllObjects(MutableDictionary *self)
 * @memberof MutableDictionary
 */
static void removeAllObjects(MutableDictionary *self) {

  for (size_t i = 0; i < self->dictionary.capacity; i++) {

    Array *array = self->dictionary.elements[i];
    if (array) {
      self->dictionary.elements[i] = release(array);
    }
  }

  self->dictionary.count = 0;
}

/**
 * @fn void MutableDictionary::removeAllObjectsWithEnumerator(MutableDictionary *self, DictionaryEnumerator enumerator, ident data)
 * @memberof MutableDictionary
 */
static void removeAllObjectsWithEnumerator(MutableDictionary *self, DictionaryEnumerator enumerator, ident data) {

  assert(enumerator);

  for (size_t i = 0; i < self->dictionary.capacity; i++) {

    Array *array = self->dictionary.elements[i];
    if (array) {
      for (size_t j = array->count; j > 0; j -= 2) {

        ident obj = array->elements[j - 1];
        ident key = array->elements[j - 2];

        enumerator((Dictionary *) self, obj, key, data);

        $((MutableArray *) array, removeObjectAtIndex, j - 1);
        $((MutableArray *) array, removeObjectAtIndex, j - 2);
      }

      self->dictionary.elements[i] = release(array);
    }
  }

  self->dictionary.count = 0;
}

/**
 * @fn void MutableDictionary::removeObjectForKey(MutableDictionary *self, const ident key)
 * @memberof MutableDictionary
 */
static void removeObjectForKey(MutableDictionary *self, const ident key) {

  if (self->dictionary.capacity == 0) {
    return;
  }
  
  const size_t bin = HashForObject(HASH_SEED, key) % self->dictionary.capacity;

  MutableArray *array = self->dictionary.elements[bin];
  if (array) {

    const ssize_t index = $((Array *) array, indexOfObject, key);
    if (index > -1) {

      $(array, removeObjectAtIndex, index);
      $(array, removeObjectAtIndex, index);

      if (((Array *) array)->count == 0) {
        self->dictionary.elements[bin] = release(array);
      }

      self->dictionary.count--;
    }
  }
}

/**
 * @fn void MutableDictionary::removeObjectForKeyPath(MutableDictionary *self, const char *path)
 * @memberof MutableDictionary
 */
static void removeObjectForKeyPath(MutableDictionary *self, const char *path) {

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
    if (load >= MUTABLEDICTIONARY_MAX_LOAD) {

      size_t capacity = dict->capacity;
      ident *elements = dict->elements;

      dict->capacity = dict->capacity * MUTABLEDICTIONARY_GROW_FACTOR;
      dict->count = 0;

      dict->elements = calloc(dict->capacity, sizeof(ident));
      assert(dict->elements);

      for (size_t i = 0; i < capacity; i++) {

        Array *array = elements[i];
        if (array) {

          for (size_t j = 0; j < array->count; j += 2) {

            ident key = $(array, objectAtIndex, j);
            ident obj = $(array, objectAtIndex, j + 1);

            $$(MutableDictionary, setObjectForKey, (MutableDictionary *) dict, obj, key);
          }

          release(array);
        }
      }

      free(elements);
    }
  } else {
    $$(MutableDictionary, initWithCapacity, (MutableDictionary *) dict, MUTABLEDICTIONARY_DEFAULT_CAPACITY);
  }
}

/**
 * @fn void MutableDictionary::setObjectForKey(MutableDictionary *self, const ident obj, const ident key)
 * @memberof MutableDictionary
 */
static void setObjectForKey(MutableDictionary *self, const ident obj, const ident key) {

  Dictionary *dict = (Dictionary *) self;

  setObjectForKey_resize(dict);

  const size_t bin = HashForObject(HASH_SEED, key) % dict->capacity;

  MutableArray *array = dict->elements[bin];
  if (array == NULL) {
    array = dict->elements[bin] = $$(MutableArray, arrayWithCapacity, (dict->capacity >> 2) + 1);
  }

  const ssize_t index = $((Array *) array, indexOfObject, key);
  if (index > -1) {
    $(array, setObjectAtIndex, obj, index + 1);
  } else {
    $(array, addObject, key);
    $(array, addObject, obj);

    dict->count++;
  }
}

/**
 * @fn void MutableDictionary::setObjectForKeyPath(MutableDictionary *self, const ident obj, const char *path)
 * @memberof MutableDictionary
 */
static void setObjectForKeyPath(MutableDictionary *self, const ident obj, const char *path) {

  String *key = $$(String, stringWithCharacters, path);

  $(self, setObjectForKey, obj, key);

  release(key);
}

/**
 * @fn void MutableDictionary::setObjectsForKeyPaths(MutableDictionary *self, ...)
 * @memberof MutableDictionary
 */
static void setObjectsForKeyPaths(MutableDictionary *self, ...) {

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
 * @fn void MutableDictionary::setObjectsForKeys(MutableDictionary *self, ...)
 * @memberof MutableDictionary
 */
static void setObjectsForKeys(MutableDictionary *self, ...) {

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

  ((MutableDictionaryInterface *) clazz->interface)->addEntriesFromDictionary = addEntriesFromDictionary;
  ((MutableDictionaryInterface *) clazz->interface)->dictionary = dictionary;
  ((MutableDictionaryInterface *) clazz->interface)->dictionaryWithCapacity = dictionaryWithCapacity;
  ((MutableDictionaryInterface *) clazz->interface)->init = init;
  ((MutableDictionaryInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((MutableDictionaryInterface *) clazz->interface)->removeAllObjects = removeAllObjects;
  ((MutableDictionaryInterface *) clazz->interface)->removeAllObjectsWithEnumerator = removeAllObjectsWithEnumerator;
  ((MutableDictionaryInterface *) clazz->interface)->removeObjectForKey = removeObjectForKey;
  ((MutableDictionaryInterface *) clazz->interface)->removeObjectForKeyPath = removeObjectForKeyPath;
  ((MutableDictionaryInterface *) clazz->interface)->setObjectForKey = setObjectForKey;
  ((MutableDictionaryInterface *) clazz->interface)->setObjectForKeyPath = setObjectForKeyPath;
  ((MutableDictionaryInterface *) clazz->interface)->setObjectsForKeyPaths = setObjectsForKeyPaths;
  ((MutableDictionaryInterface *) clazz->interface)->setObjectsForKeys = setObjectsForKeys;
}

/**
 * @fn Class *MutableDictionary::_MutableDictionary(void)
 * @memberof MutableDictionary
 */
Class *_MutableDictionary(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "MutableDictionary",
      .superclass = _Dictionary(),
      .instanceSize = sizeof(MutableDictionary),
      .interfaceOffset = offsetof(MutableDictionary, interface),
      .interfaceSize = sizeof(MutableDictionaryInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
