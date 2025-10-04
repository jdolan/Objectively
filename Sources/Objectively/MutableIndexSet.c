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
#include <stdlib.h>

#include "MutableIndexSet.h"

#define _Class _MutableIndexSet

#define INDEX_SET_CHUNK_SIZE 8

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  //..

  super(Object, self, dealloc);
}

#pragma mark - MutableIndexSet

/**
 * @fn void MutableIndexSet::addIndex(MutableIndexSet *self, size_t index)
 * @memberof MutableIndexSet
 */
static void addIndex(MutableIndexSet *self, size_t index) {

  IndexSet *this = &self->indexSet;

  size_t i;
  for (i = 0; i < this->count; i++) {
    if (this->indexes[i] == index) {
      return;
    }
    if (this->indexes[i] > index) {
      break;
    }
  }

  if (this->count == self->capacity) {
    self->capacity += INDEX_SET_CHUNK_SIZE;

    this->indexes = realloc(this->indexes, self->capacity * sizeof(size_t));
    assert(this->indexes);
  }

  for (size_t j = this->count; j > i; j--) {
    this->indexes[j] = this->indexes[j - 1];
  }

  this->indexes[i] = index;
  this->count++;
}

/**
 * @fn void MutableIndexSet::addIndexes(MutableIndexSet *self, size_t *indexes, size_t count)
 * @memberof MutableIndexSet
 */
static void addIndexes(MutableIndexSet *self, size_t *indexes, size_t count) {

  for (size_t i = 0; i < count; i++) {
    $(self, addIndex, indexes[i]);
  }
}

/**
 * @fn void MutableIndexSet::addIndexesInRange(MutableIndexSet *self, const Range range)
 * @brief Adds indexes in the specified Range to this MutableIndexSet.
 * @param self The MutableIndexSet.
 * @param range The Range of indexes to add.
 * @memberof MutableIndexSet
 */
static void addIndexesInRange(MutableIndexSet *self, const Range range) {

  for (size_t i = range.location; i < range.length; i++) {
    $(self, addIndex, i);
  }
}

/**
 * @fn MutableIndexSet *MutableIndexSet::init(MutableIndexSet *self)
 * @memberof MutableIndexSet
 */
static MutableIndexSet *init(MutableIndexSet *self) {
  return $(self, initWithCapacity, INDEX_SET_CHUNK_SIZE);
}

/**
 * @fn MutableSet *MutableSet::initWithCapacity(MutableSet *self, size_t capacity)
 * @memberof MutableSet
 */
static MutableIndexSet *initWithCapacity(MutableIndexSet *self, size_t capacity) {

  self = (MutableIndexSet *) super(Object, self, init);
  if (self) {
    self->capacity = capacity;

    IndexSet *this = & self->indexSet;

    this->indexes = malloc(self->capacity * sizeof(size_t));
    assert(this->indexes);
  }

  return self;
}

/**
 * @fn void MutableIndexSet::removeAllIndexes(MutableIndexSet *self)
 * @memberof MutableIndexSet
 */
static void removeAllIndexes(MutableIndexSet *self) {

  IndexSet *this = &self->indexSet;

  free(this->indexes);
  this->indexes = NULL;

  this->count = 0;
  self->capacity = 0;
}

/**
 * @fn void MutableIndexSet::removeIndex(MutableIndexSet *self, size_t index)
 * @memberof MutableIndexSet
 */
static void removeIndex(MutableIndexSet *self, size_t index) {

  IndexSet *this = &self->indexSet;
  for (size_t i = 0; i < this->count; i++) {
    if (this->indexes[i] == index) {
      this->count--;
      for (size_t j = i; j < this->count; j++) {
        this->indexes[j] = this->indexes[j + 1];
      }
      return;
    }
  }
}

/**
 * @fn void MutableIndexSet::removeIndexes(MutableIndexSet *self, size_t *indexes, size_t count)
 * @memberof MutableIndexSet
 */
static void removeIndexes(MutableIndexSet *self, size_t *indexes, size_t count) {

  for (size_t i = 0; i < count; i++) {
    $(self, removeIndex, indexes[i]);
  }
}

/**
* @fn void MutableIndexSet::removeIndexesInRange(MutableIndexSet *self, const Range range)
* @memberof MutableIndexSet
*/
static void removeIndexesInRange(MutableIndexSet *self, const Range range) {

  for (size_t i = range.location; i < range.length; i++) {
    $(self, removeIndex, i);
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((MutableIndexSetInterface *) clazz->interface)->addIndex = addIndex;
  ((MutableIndexSetInterface *) clazz->interface)->addIndexes = addIndexes;
  ((MutableIndexSetInterface *) clazz->interface)->addIndexesInRange = addIndexesInRange;
  ((MutableIndexSetInterface *) clazz->interface)->init = init;
  ((MutableIndexSetInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((MutableIndexSetInterface *) clazz->interface)->removeAllIndexes = removeAllIndexes;
  ((MutableIndexSetInterface *) clazz->interface)->removeIndex = removeIndex;
  ((MutableIndexSetInterface *) clazz->interface)->removeIndexes = removeIndexes;
  ((MutableIndexSetInterface *) clazz->interface)->removeIndexesInRange = removeIndexesInRange;
}

/**
 * @fn Class *MutableIndexSet::_MutableIndexSet(void)
 * @memberof MutableIndexSet
 */
Class *_MutableIndexSet(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "MutableIndexSet",
      .superclass = _IndexSet(),
      .instanceSize = sizeof(MutableIndexSet),
      .interfaceOffset = offsetof(MutableIndexSet, interface),
      .interfaceSize = sizeof(MutableIndexSetInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
