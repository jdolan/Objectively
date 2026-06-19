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

#include "Config.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "Hash.h"
#include "HashTable.h"

#define _Class _HashTable

#define HASHTABLE_DEFAULT_CAPACITY 16

#pragma mark - Built-in hash/equal functions

/**
 * @see HashTableHashStr
 */
size_t HashTableHashStr(const ident key) {
  const char *s = (const char *) key;
  size_t h = 5381;
  int c;
  while ((c = *s++)) {
    h = ((h << 5) + h) + (size_t) c;
  }
  return h;
}

/**
 * @see HashTableEqualStr
 */
bool HashTableEqualStr(const ident a, const ident b) {
  return strcmp((const char *) a, (const char *) b) == 0;
}

/**
 * @see HashTableHashDirect
 */
size_t HashTableHashDirect(const ident key) {
  return (size_t) (uintptr_t) key;
}

/**
 * @see HashTableEqualDirect
 */
bool HashTableEqualDirect(const ident a, const ident b) {
  return a == b;
}

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  HashTable *this = (HashTable *) self;

  for (size_t i = 0; i < this->capacity; i++) {
    HashTableEntry *e = this->buckets[i];
    while (e) {
      HashTableEntry *next = e->next;
      if (this->destroyKey) {
        this->destroyKey(e->key);
      }
      if (this->destroyValue) {
        this->destroyValue(e->value);
      }
      free(e);
      e = next;
    }
  }

  free(this->buckets);

  super(Object, self, dealloc);
}

#pragma mark - HashTable

/**
 * @fn bool HashTable::containsKey(const HashTable *self, const ident key)
 * @memberof HashTable
 */
static bool containsKey(const HashTable *self, const ident key) {
  return $(self, get, key) != NULL;
}

/**
 * @fn void HashTable::enumerateEntries(const HashTable *self, HashTableEnumerator enumerator, ident data)
 * @memberof HashTable
 */
static void enumerateEntries(const HashTable *self, HashTableEnumerator enumerator, ident data) {

  assert(enumerator);

  for (size_t i = 0; i < self->capacity; i++) {
    for (const HashTableEntry *e = self->buckets[i]; e; e = e->next) {
      enumerator(self, e->key, e->value, data);
    }
  }
}

/**
 * @fn ident HashTable::get(const HashTable *self, const ident key)
 * @memberof HashTable
 */
static ident get(const HashTable *self, const ident key) {

  const size_t bin = self->hash(key) % self->capacity;

  for (const HashTableEntry *e = self->buckets[bin]; e; e = e->next) {
    if (self->equal(e->key, key)) {
      return e->value;
    }
  }

  return NULL;
}

/**
 * @fn HashTable *HashTable::init(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal)
 * @memberof HashTable
 */
static HashTable *init(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal) {
  return $(self, initWithCapacity, hash, equal, HASHTABLE_DEFAULT_CAPACITY);
}

/**
 * @fn HashTable *HashTable::initWithCapacity(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal, size_t capacity)
 * @memberof HashTable
 */
static HashTable *initWithCapacity(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal, size_t capacity) {

  self = (HashTable *) super(Object, self, init);
  if (self) {
    assert(hash);
    assert(equal);
    assert(capacity);

    self->hash = hash;
    self->equal = equal;
    self->capacity = capacity;
    self->buckets = calloc(self->capacity, sizeof(HashTableEntry *));
    assert(self->buckets);
  }
  return self;
}

/**
 * @fn void HashTable::remove(HashTable *self, const ident key)
 * @memberof HashTable
 */
static void _remove(HashTable *self, const ident key) {

  const size_t bin = self->hash(key) % self->capacity;

  HashTableEntry **e = &self->buckets[bin];
  while (*e) {
    if (self->equal((*e)->key, key)) {
      HashTableEntry *found = *e;
      *e = found->next;
      if (self->destroyKey) {
        self->destroyKey(found->key);
      }
      if (self->destroyValue) {
        self->destroyValue(found->value);
      }
      free(found);
      self->count--;
      return;
    }
    e = &(*e)->next;
  }
}

/**
 * @fn void HashTable::removeAll(HashTable *self)
 * @memberof HashTable
 */
static void removeAll(HashTable *self) {

  for (size_t i = 0; i < self->capacity; i++) {
    HashTableEntry *e = self->buckets[i];
    while (e) {
      HashTableEntry *next = e->next;
      if (self->destroyKey) {
        self->destroyKey(e->key);
      }
      if (self->destroyValue) {
        self->destroyValue(e->value);
      }
      free(e);
      e = next;
    }
    self->buckets[i] = NULL;
  }

  self->count = 0;
}

/**
 * @fn void HashTable::set(HashTable *self, const ident key, const ident value)
 * @memberof HashTable
 */
static void set(HashTable *self, const ident key, const ident value) {

  const size_t bin = self->hash(key) % self->capacity;

  for (HashTableEntry *e = self->buckets[bin]; e; e = e->next) {
    if (self->equal(e->key, key)) {
      if (self->destroyKey) {
        self->destroyKey(e->key);
      }
      if (self->destroyValue) {
        self->destroyValue(e->value);
      }
      e->key = key;
      e->value = value;
      return;
    }
  }

  HashTableEntry *e = calloc(1, sizeof(HashTableEntry));
  assert(e);
  e->key = key;
  e->value = value;
  e->next = self->buckets[bin];
  self->buckets[bin] = e;
  self->count++;
}


/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((HashTableInterface *) clazz->interface)->containsKey = containsKey;
  ((HashTableInterface *) clazz->interface)->enumerateEntries = enumerateEntries;
  ((HashTableInterface *) clazz->interface)->get = get;
  ((HashTableInterface *) clazz->interface)->init = init;
  ((HashTableInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((HashTableInterface *) clazz->interface)->remove = _remove;
  ((HashTableInterface *) clazz->interface)->removeAll = removeAll;
  ((HashTableInterface *) clazz->interface)->set = set;
}

/**
 * @fn Class *HashTable::_HashTable(void)
 * @memberof HashTable
 */
Class *_HashTable(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "HashTable",
      .superclass = _Object(),
      .instanceSize = sizeof(HashTable),
      .interfaceOffset = offsetof(HashTable, interface),
      .interfaceSize = sizeof(HashTableInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
