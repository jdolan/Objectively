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
#include "Set.h"
#include "String.h"

#define _Class _Set

#define SET_DEFAULT_CAPACITY 64
#define SET_GROW_FACTOR 2.0
#define SET_MAX_LOAD 0.75f

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const Set *this = (Set *) self;

  Set *that = $(alloc(Set), initWithCapacity, this->capacity);

  $(that, addObjectsFromSet, this);

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
 * @brief A helper for resizing Sets as Objects are added to them.
 * @remarks Static method invocations are used for all operations.
 */
static void addObject_resize(Set *set) {

  if (set->capacity) {

    const float load = set->count / (float) set->capacity;
    if (load >= SET_MAX_LOAD) {

      size_t capacity = set->capacity;
      ident *elements = set->elements;

      set->capacity = set->capacity * SET_GROW_FACTOR;
      set->count = 0;

      set->elements = calloc(set->capacity, sizeof(ident));
      assert(set->elements);

      for (size_t i = 0; i < capacity; i++) {

        Array *array = elements[i];
        if (array) {
          $(set, addObjectsFromArray, array);
          release(array);
        }
      }

      free(elements);
    }
  } else {
    $(set, initWithCapacity, SET_DEFAULT_CAPACITY);
  }
}

/**
 * @fn void Set::addObject(Set *self, const ident obj)
 * @memberof Set
 */
static void addObject(Set *self, const ident obj) {

  addObject_resize(self);

  const size_t bin = HashForObject(HASH_SEED, obj) % self->capacity;

  Array *array = self->elements[bin];
  if (array == NULL) {
    array = self->elements[bin] = $(alloc(Array), init);
  }

  if ($((Array *) array, containsObject, obj) == false) {
    $(array, addObject, obj);
    self->count++;
  }
}

/**
 * @brief ArrayEnumerator for addObjectsFromArray.
 */
static void addObjectsFromArray_enumerator(const Array *array, ident obj, ident data) {
  $((Set *) data, addObject, obj);
}

/**
 * @fn void Set::addObjectsFromArray(Set *self, const Array *array)
 * @memberof Set
 */
static void addObjectsFromArray(Set *self, const Array *array) {

  if (array) {
    $(array, enumerateObjects, addObjectsFromArray_enumerator, self);
  }
}

/**
 * @brief SetEnumerator for addObjectsFromSet.
 */
static void addObjectsFromSet_enumerator(const Set *set, ident obj, ident data) {
  $((Set *) data, addObject, obj);
}

/**
 * @fn void Set::addObjectsFromSet(Set *self, const Set *set)
 * @memberof Set
 */
static void addObjectsFromSet(Set *self, const Set *set) {

  if (set) {
    $(set, enumerateObjects, addObjectsFromSet_enumerator, self);
  }
}

/**
 * @brief SetEnumerator for allObjects.
 */
static void allObjects_enumerator(const Set *set, ident obj, ident data) {
  $((Array *) data, addObject, obj);
}

/**
 * @fn Array *Set::allObjects(const Set *self)
 * @memberof Set
 */
static Array *allObjects(const Set *self) {

  Array *objects = $(alloc(Array), initWithCapacity, self->count);

  $(self, enumerateObjects, allObjects_enumerator, objects);

  return objects;
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
 * @fn void Set::filter(Set *self, Predicate predicate, ident data)
 * @memberof Set
 */
static void filter(Set *self, Predicate predicate, ident data) {

  assert(predicate);

  self->count = 0;

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {

      $(array, filter, predicate, data);

      if (array->count == 0) {
        release(array);
        self->elements[i] = NULL;
      } else {
        self->count += array->count;
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

  Set *set = $(alloc(Set), init);

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

  return set;
}

/**
 * @fn Set *Set::init(Set *self)
 * @memberof Set
 */
static Set *init(Set *self) {

  return $(self, initWithCapacity, SET_DEFAULT_CAPACITY);
}

/**
 * @brief ArrayEnumerator for initWithArray.
 */
static void initWithArray_enumerator(const Array *array, ident obj, ident data) {
  $((Set *) data, addObject, obj);
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
 * @fn Set *Set::initWithCapacity(Set *self, size_t capacity)
 * @memberof Set
 */
static Set *initWithCapacity(Set *self, size_t capacity) {

  self = (Set *) super(Object, self, init);
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
        $(self, addObject, obj);
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
  $((Set *) data, addObject, obj);
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

  Set *set = $(alloc(Set), initWithCapacity, self->count);
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

  return set;
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
 * @fn void Set::removeAllObjects(Set *self)
 * @memberof Set
 */
static void removeAllObjects(Set *self) {

  for (size_t i = 0; i < self->capacity; i++) {

    Array *array = self->elements[i];
    if (array) {
      release(array);
      self->elements[i] = NULL;
    }
  }

  self->count = 0;
}

/**
 * @fn void Set::removeObject(Set *self, const ident obj)
 * @memberof Set
 */
static void removeObject(Set *self, const ident obj) {

  if (self->capacity == 0) {
    return;
  }

  const size_t bin = HashForObject(HASH_SEED, obj) % self->capacity;

  Array *array = self->elements[bin];
  if (array) {

    const ssize_t index = $(array, indexOfObject, obj);
    if (index > -1) {

      $(array, removeObjectAtIndex, index);

      if (((Array *) array)->count == 0) {
        release(array);
        self->elements[bin] = NULL;
      }

      self->count--;
    }
  }
}

/**
 * @fn Set *Set::set(void)
 * @memberof Set
 */
static Set *set(void) {

  return $(alloc(Set), init);
}

/**
 * @fn Set *Set::setWithArray(const Array *array)
 * @memberof Set
 */
static Set *setWithArray(const Array *array) {

  return $(alloc(Set), initWithArray, array);
}

/**
 * @fn Set *Set::setWithCapacity(size_t capacity)
 * @memberof Set
 */
static Set *setWithCapacity(size_t capacity) {

  return $(alloc(Set), initWithCapacity, capacity);
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
      $(set, addObject, obj);
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

  ((SetInterface *) clazz->interface)->addObject = addObject;
  ((SetInterface *) clazz->interface)->addObjectsFromArray = addObjectsFromArray;
  ((SetInterface *) clazz->interface)->addObjectsFromSet = addObjectsFromSet;
  ((SetInterface *) clazz->interface)->allObjects = allObjects;
  ((SetInterface *) clazz->interface)->containsObject = containsObject;
  ((SetInterface *) clazz->interface)->containsObjectMatching = containsObjectMatching;
  ((SetInterface *) clazz->interface)->enumerateObjects = enumerateObjects;
  ((SetInterface *) clazz->interface)->filter = filter;
  ((SetInterface *) clazz->interface)->filteredSet = filteredSet;
  ((SetInterface *) clazz->interface)->init = init;
  ((SetInterface *) clazz->interface)->initWithArray = initWithArray;
  ((SetInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((SetInterface *) clazz->interface)->initWithObjects = initWithObjects;
  ((SetInterface *) clazz->interface)->initWithSet = initWithSet;
  ((SetInterface *) clazz->interface)->mappedSet = mappedSet;
  ((SetInterface *) clazz->interface)->reduce = reduce;
  ((SetInterface *) clazz->interface)->removeAllObjects = removeAllObjects;
  ((SetInterface *) clazz->interface)->removeObject = removeObject;
  ((SetInterface *) clazz->interface)->set = set;
  ((SetInterface *) clazz->interface)->setWithArray = setWithArray;
  ((SetInterface *) clazz->interface)->setWithCapacity = setWithCapacity;
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
