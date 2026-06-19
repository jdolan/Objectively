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

#pragma once

#include <Objectively/Object.h>

/**
 * @file
 * @brief Hash tables with user-supplied hash and equality functions for raw C types.
 */

typedef struct HashTable HashTable;
typedef struct HashTableInterface HashTableInterface;

/**
 * @brief A function that computes a hash for the given key.
 */
typedef size_t (*HashTableHashFunc)(const ident key);

/**
 * @brief A function that tests equality of two keys.
 */
typedef bool (*HashTableEqualFunc)(const ident a, const ident b);

/**
 * @brief A function called to destroy a key or value on removal.
 */
typedef void (*HashTableDestroyFunc)(ident obj);

/**
 * @brief The HashTableEnumerator function type.
 * @param table The HashTable.
 * @param key The key.
 * @param value The value.
 * @param data User data.
 */
typedef void (*HashTableEnumerator)(const HashTable *table, ident key, ident value, ident data);

/**
 * @brief Common hash functions for use with HashTable.
 */
OBJECTIVELY_EXPORT size_t HashTableHashStr(const ident key);
OBJECTIVELY_EXPORT bool   HashTableEqualStr(const ident a, const ident b);
OBJECTIVELY_EXPORT size_t HashTableHashDirect(const ident key);
OBJECTIVELY_EXPORT bool   HashTableEqualDirect(const ident a, const ident b);

/**
 * @brief Internal bucket entry.
 * @private
 */
typedef struct HashTableEntry {
  ident key;
  ident value;
  struct HashTableEntry *next;
} HashTableEntry;

/**
 * @brief Hash tables with user-supplied hash and equality functions.
 * @extends Object
 * @ingroup Collections
 */
struct HashTable {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  HashTableInterface *interface;

  /**
   * @brief The number of entries.
   */
  size_t count;

  /**
   * @brief The number of buckets.
   * @protected
   */
  size_t capacity;

  /**
   * @brief The buckets.
   * @protected
   */
  HashTableEntry **buckets;

  /**
   * @brief The hash function.
   */
  HashTableHashFunc hash;

  /**
   * @brief The equality function.
   */
  HashTableEqualFunc equal;

  /**
   * @brief Optional destructor called when a key is removed or replaced.
   */
  HashTableDestroyFunc destroyKey;

  /**
   * @brief Optional destructor called when a value is removed or replaced.
   */
  HashTableDestroyFunc destroyValue;
};

/**
 * @brief The HashTable interface.
 */
struct HashTableInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn bool HashTable::containsKey(const HashTable *self, const ident key)
   * @param self The HashTable.
   * @param key The key.
   * @return True if this HashTable contains the given key.
   * @memberof HashTable
   */
  bool (*containsKey)(const HashTable *self, const ident key);

  /**
   * @fn void HashTable::enumerateEntries(const HashTable *self, HashTableEnumerator enumerator, ident data)
   * @brief Enumerates the entries of this HashTable with the given function.
   * @param self The HashTable.
   * @param enumerator The enumerator function.
   * @param data User data.
   * @memberof HashTable
   */
  void (*enumerateEntries)(const HashTable *self, HashTableEnumerator enumerator, ident data);

  /**
   * @fn ident HashTable::get(const HashTable *self, const ident key)
   * @param self The HashTable.
   * @param key The key.
   * @return The value for the given key, or NULL if not found.
   * @memberof HashTable
   */
  ident (*get)(const HashTable *self, const ident key);

  /**
   * @fn HashTable *HashTable::init(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal)
   * @brief Initializes this HashTable with the given hash and equality functions.
   * @param self The HashTable.
   * @param hash The hash function.
   * @param equal The equality function.
   * @return The initialized HashTable, or NULL on error.
   * @memberof HashTable
   */
  HashTable *(*init)(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal);

  /**
   * @fn HashTable *HashTable::initWithCapacity(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal, size_t capacity)
   * @brief Initializes this HashTable with the given capacity.
   * @param self The HashTable.
   * @param hash The hash function.
   * @param equal The equality function.
   * @param capacity The initial bucket count.
   * @return The initialized HashTable, or NULL on error.
   * @memberof HashTable
   */
  HashTable *(*initWithCapacity)(HashTable *self, HashTableHashFunc hash, HashTableEqualFunc equal, size_t capacity);

  /**
   * @fn void HashTable::remove(HashTable *self, const ident key)
   * @brief Removes the entry for the given key.
   * @param self The HashTable.
   * @param key The key to remove.
   * @memberof HashTable
   */
  void (*remove)(HashTable *self, const ident key);

  /**
   * @fn void HashTable::removeAll(HashTable *self)
   * @brief Removes all entries from this HashTable.
   * @param self The HashTable.
   * @memberof HashTable
   */
  void (*removeAll)(HashTable *self);

  /**
   * @fn void HashTable::set(HashTable *self, const ident key, const ident value)
   * @brief Sets the value for the given key, replacing any existing entry.
   * @param self The HashTable.
   * @param key The key.
   * @param value The value.
   * @memberof HashTable
   */
  void (*set)(HashTable *self, const ident key, const ident value);
};

/**
 * @fn Class *HashTable::_HashTable(void)
 * @brief The HashTable archetype.
 * @return The HashTable Class.
 * @memberof HashTable
 */
OBJECTIVELY_EXPORT Class *_HashTable(void);
