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
#include "MutableSet.h"
#include "Set.h"
#include "String.h"

#define _Class _Set

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const Set *this = (Set *) self;

  Set *that = $(alloc(Set), initWithSet, this);

  return (Object *) that;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Set *this = (Set *) self;

  for (size_t i = 0; i < this->capacity; i++) {
    release(this->elements[i]);
  }

  free(this->elements);

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  const Array *array = $((Set *) self, allObjects);

  return $((Object *) array, description);
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  const Set *this = (Set *) self;

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

  if (other && (self->clazz == other->clazz)) {

    const Set *this = (Set *) self;
    const Set *that = (Set *) other;

    if (this->count == that->count) {

      Array *objects = $(this, allObjects);

      for (size_t i = 0; i < objects->count; i++) {
        const ident obj = $(objects, objectAtIndex, i);

        if ($(that, containsObject, obj) == false) {
          release(objects);
          return false;
        }
      }

      release(objects);
      return true;
    }
  }

  return false;
}

#pragma mark - Set

/**
 * @brief SetEnumerator for allObjects.
 */
static void allObjects_enumerator(const Set *set, ident obj, ident data) {
  $((MutableArray *) data, addObject, obj);
}

/**
 * @fn Array *Set::allObjects(const Set *self)
 * @memberof Set
 */
static Array *allObjects(const Set *self) {

  MutableArray *objects = $(alloc(MutableArray), initWithCapacity, self->count);

  $(self, enumerateObjects, allObjects_enumerator, objects);

  return (Array *) objects;
}

/**
 * @fn bool Set::containsObject(const Set *self, const ident obj)
 * @memberof Set
 */
static bool containsObject(const Set *self, const ident obj) {

  if (self->capacity) {
    const size_t bin = HashForObject(HASH_SEED, obj) % self->capacity;

    const Array *array = self->elements[bin];
    if (array) {
      return $(array, containsObject, obj);
    }
  }

  return false;
}

/**
 * @fn bool Set::containsObjectMatching(const Set *self, Predicate predicate, ident data)
 * @memberof Set
 */
static bool containsObjectMatching(const Set *self, Predicate predicate, ident data) {

  assert(predicate);

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j++) {
        if (predicate($(array, objectAtIndex, j), data)) {
          return true;
        }
      }
    }
  }

  return false;
}

/**
 * @fn void Set::enumerateObjects(const Set *self, SetEnumerator enumerator, ident data)
 * @memberof Set
 */
static void enumerateObjects(const Set *self, SetEnumerator enumerator, ident data) {

  assert(enumerator);

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j++) {
        enumerator(self, $(array, objectAtIndex, j), data);
      }
    }
  }
}

/**
 * @fn Set *Set::filteredSet(const Set *self, Predicate predicate, ident data)
 * @memberof Set
 */
static Set *filteredSet(const Set *self, Predicate predicate, ident data) {

  assert(predicate);

  MutableSet *set = $(alloc(MutableSet), init);

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j++) {
        ident obj = $(array, objectAtIndex, j);

        if (predicate(obj, data)) {
          $(set, addObject, obj);
        }
      }
    }
  }

  return (Set *) set;
}

/**
 * @brief ArrayEnumerator for initWithArray.
 */
static void initWithArray_enumerator(const Array *array, ident obj, ident data) {
  $$(MutableSet, addObject, (MutableSet *) data, obj);
}

/**
 * @fn Set *Set::initWithArray(Set *self, const Array *array)
 * @memberof Set
 */
static Set *initWithArray(Set *self, const Array *array) {

  self = (Set *) super(Object, self, init);
  if (self) {
    if (array) {
      $(array, enumerateObjects, initWithArray_enumerator, self);
    }
  }

  return self;
}

/**
 * @fn Set *Set::initWithObjects(Set *self, ...)
 * @memberof Set
 */
static Set *initWithObjects(Set *self, ...) {

  self = (Set *) super(Object, self, init);
  if (self) {

    va_list args;
    va_start(args, self);

    while (true) {

      ident obj = va_arg(args, ident);
      if (obj) {
        $$(MutableSet, addObject, (MutableSet *) self, obj);
      } else {
        break;
      }
    }

    va_end(args);
  }

  return self;
}

/**
 * @brief SetEnumerator for initWithSet.
 */
static void initWithSet_enumerator(const Set *set, ident obj, ident data) {
  $$(MutableSet, addObject, (MutableSet *) data, obj);
}

/**
 * @fn Set *Set::initWithSet(Set *self, const Set *set)
 * @memberof Set
 */
static Set *initWithSet(Set *self, const Set *set) {

  self = (Set *) super(Object, self, init);
  if (self) {
    if (set) {
      $(set, enumerateObjects, initWithSet_enumerator, self);
    }
  }

  return self;
}

/**
 * @fn Set *Set::mappedSet(const Set *self, Functor functor, ident data)
 * @memberof Set
 */
static Set *mappedSet(const Set *self, Functor functor, ident data) {

  assert(functor);

  MutableSet *set = $(alloc(MutableSet), initWithCapacity, self->count);
  assert(set);

  for (size_t i = 0; i < self->capacity; i++) {

    const Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j++) {

        ident obj = functor(array->elements[j], data);

        $(set, addObject, obj);

        release(obj);
      }
    }
  }

  return (Set *) set;
}

/**
 * @fn MutableSet *Set::mutableCopy(const Set *self)
 * @memberof Set
 */
static MutableSet *mutableCopy(const Set *self) {

  MutableSet *copy = $(alloc(MutableSet), initWithCapacity, self->count);
  assert(copy);

  $(copy, addObjectsFromSet, self);
  return copy;
}

/**
 * @fn ident Set::reduce(const Set *self, Reducer reducer, ident accumulator, ident data)
 * @memberof Set
 */
static ident reduce(const Set *self, Reducer reducer, ident accumulator, ident data) {

  assert(reducer);

  for (size_t i = 0; i < self->capacity; i++) {

    const Array *array = self->elements[i];
    if (array) {

      for (size_t j = 0; j < array->count; j++) {
        accumulator = reducer(array->elements[j], accumulator, data);
      }
    }
  }

  return accumulator;
}

/**
 * @fn Set *Set::setWithArray(const Array *array)
 * @memberof Set
 */
static Set *setWithArray(const Array *array) {

  return $(alloc(Set), initWithArray, array);
}

/**
 * @fn Set *Set::setWithObjects(ident obj, ...)
 * @memberof Set
 */
static Set *setWithObjects(ident obj, ...) {

  Set *set = (Set *) $((Object *) alloc(Set), init);
  if (set) {

    va_list args;
    va_start(args, obj);

    while (obj) {
      $$(MutableSet, addObject, (MutableSet * ) set, obj);
      obj = va_arg(args, ident);
    }

    va_end(args);
  }

  return set;
}

/**
 * @fn Set *Set::setWithSet(const Set *set)
 * @memberof Set
 */
static Set *setWithSet(const Set *set) {

  return $(alloc(Set), initWithSet, set);
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

  ((SetInterface *) clazz->interface)->allObjects = allObjects;
  ((SetInterface *) clazz->interface)->containsObject = containsObject;
  ((SetInterface *) clazz->interface)->containsObjectMatching = containsObjectMatching;
  ((SetInterface *) clazz->interface)->enumerateObjects = enumerateObjects;
  ((SetInterface *) clazz->interface)->filteredSet = filteredSet;
  ((SetInterface *) clazz->interface)->initWithArray = initWithArray;
  ((SetInterface *) clazz->interface)->initWithSet = initWithSet;
  ((SetInterface *) clazz->interface)->initWithObjects = initWithObjects;
  ((SetInterface *) clazz->interface)->mappedSet = mappedSet;
  ((SetInterface *) clazz->interface)->mutableCopy = mutableCopy;
  ((SetInterface *) clazz->interface)->reduce = reduce;
  ((SetInterface *) clazz->interface)->setWithArray = setWithArray;
  ((SetInterface *) clazz->interface)->setWithObjects = setWithObjects;
  ((SetInterface *) clazz->interface)->setWithSet = setWithSet;
}

/**
 * @fn Class *Set::_Set(void)
 * @memberof Set
 */
Class *_Set(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Set",
      .superclass = _Object(),
      .instanceSize = sizeof(Set),
      .interfaceOffset = offsetof(Set, interface),
      .interfaceSize = sizeof(SetInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class

