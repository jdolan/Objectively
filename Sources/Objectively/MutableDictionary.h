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

#include <Objectively/Dictionary.h>

/**
 * @file
 * @brief Mutable key-value stores.
 */

typedef struct MutableDictionaryInterface MutableDictionaryInterface;

/**
 * @brief Mutable key-value stores.
 * @extends Dictionary
 * @ingroup Collections
 */
struct MutableDictionary {

  /**
   * @brief The superclass.
   */
  Dictionary dictionary;

  /**
   * @brief The interface.
   * @protected
   */
  MutableDictionaryInterface *interface;
};

/**
 * @brief The MutableDictionary interface.
 */
struct MutableDictionaryInterface {

  /**
   * @brief The superclass.
   */
  DictionaryInterface dictionaryInterface;

  /**
   * @fn void MutableDictionary::addEntriesFromDictionary(MutableDictionary *self, const Dictionary *dictionary)
   * @brief Adds the key-value entries from `dictionary` to this MutableDictionary.
   * @param self The MutableDictionary.
   * @param dictionary A Dictionary.
   * @memberof MutableDictionary
   */
  void (*addEntriesFromDictionary)(MutableDictionary *self, const Dictionary *dictionary);

  /**
   * @static
   * @fn MutableDictionary *MutableDictionary::dictionary(void)
   * @brief Returns a new MutableDictionary.
   * @return The new MutableDictionary, or `NULL` on error.
   * @memberof MutableDictionary
   */
  MutableDictionary *(*dictionary)(void);

  /**
   * @static
   * @fn MutableDictionary *MutableDictionary::dictionaryWithCapacity(size_t capacity)
   * @brief Returns a new MutableDictionary with the given `capacity`.
   * @param capacity The desired initial capacity.
   * @return The new MutableDictionary, or `NULL` on error.
   * @memberof MutableDictionary
   */
  MutableDictionary *(*dictionaryWithCapacity)(size_t capacity);

  /**
   * @fn MutableDictionary *MutableDictionary::init(MutableDictionary *self)
   * @brief Initializes this MutableDictionary.
   * @param self The MutableDictionary.
   * @return The initialized MutableDictionary, or `NULL` on error.
   * @memberof MutableDictionary
   */
  MutableDictionary *(*init)(MutableDictionary *self);

  /**
   * @fn MutableDictionary *MutableDictionary::initWithCapacity(MutableDictionary *self, size_t capacity)
   * @brief Initializes this MutableDictionary with the specified capacity.
   * @param self The MutableDictionary.
   * @param capacity The initial capacity.
   * @return The initialized MutableDictionary, or `NULL` on error.
   * @memberof MutableDictionary
   */
  MutableDictionary *(*initWithCapacity)(MutableDictionary *self, size_t capacity);

  /**
   * @fn void MutableDictionary::removeAllObjects(MutableDictionary *self)
   * @brief Removes all Objects from this MutableDictionary.
   * @param self The MutableDictionary.
   * @memberof MutableDictionary
   */
  void (*removeAllObjects)(MutableDictionary *self);

  /**
   * @fn void MutableDictionary::removeAllObjectsWithEnumerator(MutableDictionary *self, DictionaryEnumerator enumerator, ident data)
   * @brief Removes all Objects from this MutableDictionary, invoking `enumerator` for each Object and key pair.
   * @param self The MutableDictionary.
   * @param enumerator The enumerator.
   * @param data The data.
   * @memberof MutableArray
   */
  void (*removeAllObjectsWithEnumerator)(MutableDictionary *self, DictionaryEnumerator enumerator, ident data);

  /**
   * @fn void MutableDictionary::removeObjectForKey(MutableDictionary *self, const ident key)
   * @brief Removes the Object with the specified key from this MutableDictionary.
   * @param self The MutableDictionary.
   * @param key The key of the Object to remove.
   * @memberof MutableDictionary
   */
  void (*removeObjectForKey)(MutableDictionary *self, const ident key);

  /**
   * @fn void MutableDictionary::removeObjectForKeyPath(MutableDictionary *self, const char *path)
   * @brief Removes the Object with the specified key path from this MutableDictionary.
   * @param self The MutableDictionary.
   * @param path The key path of the Object to remove.
   * @memberof MutableDictionary
   */
  void (*removeObjectForKeyPath)(MutableDictionary *self, const char *path);

  /**
   * @fn void MutableDictionary ::setObjectForKey(MutableDictionary *self, const ident obj, const ident key)
   * @brief Sets a pair in this MutableDictionary.
   * @param self The MutableDictionary.
   * @param obj The Object to set.
   * @param key The key of the Object to set.
   * @memberof MutableDictionary
   */
  void (*setObjectForKey)(MutableDictionary *self, const ident obj, const ident key);

  /**
   * @fn void MutableDictionary::setObjectForKeyPath(MutableDictionary *self, const ident obj, const char *path)
   * @brief Sets a pair in this MutableDictionary.
   * @param self The MutableDictionary.
   * @param obj The Object to set.
   * @param path The key path of the Object to set.
   * @memberof MutableDictionary
   */
  void (*setObjectForKeyPath)(MutableDictionary *self, const ident obj, const char *path);

  /**
   * @fn void MutableDictionary::setObjectsForKeyPaths(MutableDictionary *self, ...)
   * @brief Sets pairs in this MutableDictionary from the NULL-terminated list.
   * @param self The MutableDictionary.
   * @memberof MutableDictionary
   */
  void (*setObjectsForKeyPaths)(MutableDictionary *self, ...);

  /**
   * @fn void MutableDictionary::setObjectsForKeys(MutableDictionary *self, ...)
   * @brief Sets pairs in this MutableDictionary from the NULL-terminated list.
   * @param self The MutableDictionary.
   * @memberof MutableDictionary
   */
  void (*setObjectsForKeys)(MutableDictionary *self, ...);
};

/**
 * @fn Class *MutableDictionary::_MutableDictionary(void)
 * @brief The MutableDictionary archetype.
 * @return The MutableDictionary Class.
 * @memberof MutableDictionary
 */
OBJECTIVELY_EXPORT Class *_MutableDictionary(void);
