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
#include <stdarg.h>
#include <stdlib.h>

#include "Array.h"
#include "Hash.h"
#include "String.h"

#if defined(__APPLE__)

/**
 * @brief BSD qsort_r.
 */
static int _quicksort(void *data, const void *a, const void *b) {
  return ((Comparator) data)(*((const ident *) a), *((const ident *) b));
}

void quicksort(ident base, size_t count, size_t size, Comparator comparator, ident data) {
  qsort_r(base, count, size, comparator, _quicksort);
}

#elif defined(_WIN32)

/**
 * @brief WIN32 qsort_s.
 */
static int _quicksort(void *data, const void *a, const void *b) {
  return ((Comparator) data)(*((const ident *) a), *((const ident *) b));
}

void quicksort(ident base, size_t count, size_t size, Comparator comparator, ident data) {
  qsort_s(base, count, size, _quicksort, comparator);
}

#else

/**
 * @brief GNU qsort_r.
 */
static int _quicksort(const void *a, const void *b, void *data) {
  return ((Comparator) data)(*((const ident *) a), *((const ident *) b));
}

void quicksort(ident base, size_t count, size_t size, Comparator comparator, ident data) {
  qsort_r(base, count, size, _quicksort, comparator);
}

#endif

#define _Class _Array

#define ARRAY_CHUNK_SIZE 64

#pragma mark - Object

/**
 * @see Object::copy(const Object *)
 */
static Object *copy(const Object *self) {

  const Array *this = (Array *) self;

  Array *copy = $(alloc(Array), initWithCapacity, this->count);
  assert(copy);

  $(copy, addObjectsFromArray, this);

  return (Object *) copy;
}

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  Array *this = (Array *) self;

  for (size_t i = 0; i < this->count; i++) {
    release(this->elements[i]);
  }

  free(this->elements);

  super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

  String *components = $((Array *) self, componentsJoinedByCharacters, ", ");

  String *desc = $(alloc(String), initWithFormat, "[%s]", components->chars ?: "");

  release(components);

  return desc;
}

/**
 * @see Object::hash(const Object *)
 */
static int hash(const Object *self) {

  Array *this = (Array *) self;

  int hash = HashForInteger(HASH_SEED, this->count);

  for (size_t i = 0; i < this->count; i++) {
    hash = HashForObject(hash, this->elements[i]);
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

  if (other && $(other, isKindOfClass, _Array())) {

    const Array *this = (Array *) self;
    const Array *that = (Array *) other;

    if (this->count == that->count) {

      for (size_t i = 0; i < this->count; i++) {

        const Object *thisObject = this->elements[i];
        const Object *thatObject = that->elements[i];

        if ($(thisObject, isEqual, thatObject) == false) {
          return false;
        }
      }

      return true;
    }
  }

  return false;
}

#pragma mark - Array

/**
 * @fn void Array::addObject(Array *self, const ident obj)
 * @memberof Array
 */
static void addObject(Array *self, const ident obj) {

  if (self->count == self->capacity) {

    self->capacity += ARRAY_CHUNK_SIZE;

    if (self->elements) {
      self->elements = realloc(self->elements, self->capacity * sizeof(ident));
    } else {
      self->elements = calloc(self->capacity, sizeof(ident));
    }

    assert(self->elements);
  }

  self->elements[self->count++] = retain(obj);
}

/**
 * @fn void Array::addObjects(Array *self, const ident obj, ...)
 * @memberof Array
 */
static void addObjects(Array *self, const ident obj, ...) {

  va_list args;
  va_start(args, obj);

  ident object = obj;
  while (object) {
    $(self, addObject, object);
    object = va_arg(args, ident);
  }

  va_end(args);
}

/**
 * @fn void Array::addObjectsFromArray(Array *self, const Array *array)
 * @memberof Array
 */
static void addObjectsFromArray(Array *self, const Array *array) {

  if (array) {
    for (size_t i = 0; i < array->count; i++) {
      $(self, addObject, array->elements[i]);
    }
  }
}

/**
 * @fn Array *Array::array(void)
 * @memberof Array
 */
static Array *array(void) {

  return $(alloc(Array), init);
}

/**
 * @fn Array *Array::arrayWithArray(const Array *array)
 * @memberof Array
 */
static Array *arrayWithArray(const Array *array) {

  return $(alloc(Array), initWithArray, array);
}

/**
 * @fn Array *Array::arrayWithCapacity(size_t capacity)
 * @memberof Array
 */
static Array *arrayWithCapacity(size_t capacity) {

  return $(alloc(Array), initWithCapacity, capacity);
}

/**
 * @fn Array *Array::arrayWithObjects(ident obj, ...)
 * @memberof Array
 */
static Array *arrayWithObjects(ident obj, ...) {

  Array *array = (Array *) super(Object, alloc(Array), init);
  if (array) {
    va_list args;
    va_start(args, obj);

    while (obj) {
      array->elements = realloc(array->elements, ++array->count * sizeof(ident));
      assert(array->elements);

      array->elements[array->count - 1] = retain(obj);
      obj = va_arg(args, ident);
    }

    va_end(args);
  }

  return array;
}

/**
 * @fn Array *Array::arrayWithVaList(va_list args)
 * @memberof Array
 */
static Array *arrayWithVaList(va_list args) {

  return $(alloc(Array), initWithVaList, args);
}

/**
 * @fn String *Array::componentsJoinedByCharacters(const Array *self, const char *chars)
 * @memberof Array
 */
static String *componentsJoinedByCharacters(const Array *self, const char *chars) {

  String *string = $(alloc(String), init);

  for (size_t i = 0; i < self->count; i++) {

    String *desc = $((Object *) self->elements[i], description);
    $(string, appendString, desc);

    release(desc);

    if (i < self->count - 1) {
      $(string, appendCharacters, chars);
    }
  }

  return (String *) string;
}

/**
 * @fn String *Array::componentsJoinedByString(const Array *self, const String *string)
 * @memberof Array
 */
static String *componentsJoinedByString(const Array *self, const String *string) {
  return $(self, componentsJoinedByCharacters, string->chars);
}

/**
 * @fn bool Array::containsObject(const Array *self, const ident obj)
 * @memberof Array
 */
static bool containsObject(const Array *self, const ident obj) {
  return $(self, indexOfObject, obj) != -1;
}

/**
 * @fn void Array::enumerate(const Array *self, ArrayEnumerator enumerator, ident data)
 * @memberof Array
 */
static void enumerate(const Array *self, ArrayEnumerator enumerator, ident data) {

  assert(enumerator);

  for (size_t i = 0; i < self->count; i++) {
    enumerator(self, self->elements[i], data);
  }
}

/**
 * @fn void Array::filter(Array *self, Predicate predicate, ident data)
 * @memberof Array
 */
static void filter(Array *self, Predicate predicate, ident data) {

  assert(predicate);

  for (size_t i = 0; i < self->count; i++) {
    if (predicate(self->elements[i], data) == false) {
      $(self, removeObjectAtIndex, i--);
    }
  }
}

/**
 * @fn Array *Array::filteredArray(const Array *self, Predicate predicate, ident data)
 * @memberof Array
 */
static Array *filteredArray(const Array *self, Predicate predicate, ident data) {

  assert(predicate);

  Array *copy = (Array *) $((Object *) self, copy);

  $(copy, filter, predicate, data);

  return copy;
}

/**
 * @fn ident Array::find(const Array *self, Predicate predicate, ident data)
 * @memberof Array
 */
static ident find(const Array *self, Predicate predicate, ident data) {

  assert(predicate);

  for (size_t i = 0; i < self->count; i++) {
    if (predicate(self->elements[i], data)) {
      return self->elements[i];
    }
  }

  return NULL;
}

/**
 * @fn ident Array::firstObject(const Array *self)
 * @memberof Array
 */
static ident firstObject(const Array *self) {

  return self->count ? $(self, objectAtIndex, 0) : NULL;
}

/**
 * @fn ssize_t Array::indexOfObject(const Array *self, const ident obj)
 * @memberof Array
 */
static ssize_t indexOfObject(const Array *self, const ident obj) {

  assert(obj);

  for (size_t i = 0; i < self->count; i++) {
    if ($((Object * ) self->elements[i], isEqual, obj)) {
      return i;
    }
  }

  return -1;
}

/**
 * @fn Array *Array::init(Array *self)
 * @memberof Array
 */
static Array *init(Array *self) {

  return $(self, initWithCapacity, 0);
}

/**
 * @fn Array *Array::initWithArray(Array *self, const Array *array)
 * @memberof Array
 */
static Array *initWithArray(Array *self, const Array *array) {

  self = (Array *) super(Object, self, init);
  if (self) {

    self->count = array->count;
    if (self->count) {

      self->elements = calloc(self->count, sizeof(ident));
      assert(self->elements);

      for (size_t i = 0; i < self->count; i++) {
        self->elements[i] = retain(array->elements[i]);
      }
    }
  }

  return self;
}

/**
 * @fn Array *Array::initWithCapacity(Array *self, size_t capacity)
 * @memberof Array
 */
static Array *initWithCapacity(Array *self, size_t capacity) {

  self = (Array *) super(Object, self, init);
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
 * @fn Array *Array::initWithObjects(Array *self, ...)
 * @memberof Array
 */
static Array *initWithObjects(Array *self, ...) {

  va_list args;
  va_start(args, self);

  self = $(self, initWithVaList, args);

  va_end(args);
  return self;
}

/**
 * @fn Array *Array::initWithVaList(Array *self, va_list args)
 * @memberof Array
 */
static Array *initWithVaList(Array *self, va_list args) {

  self = (Array *) super(Object, self, init);
  if (self) {

    ident element = va_arg(args, ident);
    while (element) {
      self->elements = realloc(self->elements, ++self->count * sizeof(ident));
      assert(self->elements);

      self->elements[self->count - 1] = retain(element);
      element = va_arg(args, ident);
    }
  }

  return self;
}

/**
 * @fn void Array::insertObjectAtIndex(Array *self, ident obj, size_t index)
 * @memberof Array
 */
static void insertObjectAtIndex(Array *self, ident obj, size_t index) {

  assert(index <= self->count);

  $(self, addObject, obj);

  for (size_t i = self->count - 1; i > index; i--) {
    self->elements[i] = self->elements[i - 1];
  }

  self->elements[index] = obj;
}

/**
 * @fn ident Array::lastObject(const Array *self)
 * @memberof Array
 */
static ident lastObject(const Array *self) {

  return self->count ? $(self, objectAtIndex, self->count - 1) : NULL;
}

/**
 * @fn void Array::map(Array *self, Functor functor, ident data)
 * @memberof Array
 */
static void map(Array *self, Functor functor, ident data) {

  assert(functor);

  for (size_t i = 0; i < self->count; i++) {
    ident obj = functor(self->elements[i], data);

    retain(obj);

    release(self->elements[i]);

    self->elements[i] = obj;
  }
}

/**
 * @fn Array *Array::mappedArray(const Array *self, Functor functor, ident data)
 * @memberof Array
 */
static Array *mappedArray(const Array *self, Functor functor, ident data) {

  assert(functor);

  Array *array = $(alloc(Array), initWithCapacity, self->count);
  assert(array);
  
  for (size_t i = 0; i < self->count; i++) {

    ident obj = functor(self->elements[i], data);

    $(array, addObject, obj);

    release(obj);
  }

  return array;
}

/**
 * @fn ident Array::objectAtIndex(const Array *self, size_t index)
 * @memberof Array
 */
static ident objectAtIndex(const Array *self, size_t index) {

  assert(index < self->count);

  return self->elements[index];
}

/**
 * @fn ident Array::reduce(const Array *self, Reducer reducer, ident accumulator, ident data)
 * @memberof Array
 */
static ident reduce(const Array *self, Reducer reducer, ident accumulator, ident data) {

  assert(reducer);

  for (size_t i = 0; i < self->count; i++) {
    accumulator = reducer(self->elements[i], accumulator, data);
  }

  return accumulator;
}

/**
 * @fn void Array::removeAllObjects(Array *self)
 * @memberof Array
 */
static void removeAllObjects(Array *self) {

  while (self->count) {
    $(self, removeLastObject);
  }
}

/**
 * @fn void Array::removeAllObjectsWithEnumerator(Array *self, ArrayEnumerator enumerator, ident data)
 * @memberof Array
 */
static void removeAllObjectsWithEnumerator(Array *self, ArrayEnumerator enumerator, ident data) {

  assert(enumerator);

  while (self->count) {

    enumerator(self, $(self, lastObject), data);

    $(self, removeLastObject);
  }
}

/**
 * @fn void Array::removeLastObject(Array *self)
 * @memberof Array
 */
static void removeLastObject(Array *self) {

  if (self->count) {
    $(self, removeObjectAtIndex, self->count - 1);
  }
}

/**
 * @fn void Array::removeObject(Array *self, const ident obj)
 * @memberof Array
 */
static void removeObject(Array *self, const ident obj) {

  const ssize_t index = $(self, indexOfObject, obj);
  if (index > -1) {
    $(self, removeObjectAtIndex, index);
  }
}

/**
 * @fn void Array::removeObjectAtIndex(Array *self, size_t index)
 * @memberof Array
 */
static void removeObjectAtIndex(Array *self, size_t index) {

  assert(index < self->count);

  release(self->elements[index]);

  for (size_t i = index; i < self->count - 1; i++) {
    self->elements[i] = self->elements[i + 1];
  }

  self->count--;
}

/**
 * @fn void Array::setObjectAtIndex(Array *self, const ident obj, size_t index)
 * @memberof Array
 */
static void setObjectAtIndex(Array *self, const ident obj, size_t index) {

  assert(index < self->count);

  retain(obj);

  release(self->elements[index]);

  self->elements[index] = obj;
}

/**
 * @fn void Array::sort(Array *self, Comparator comparator)
 * @memberof Array
 */
static void sort(Array *self, Comparator comparator) {
  quicksort(self->elements, self->count, sizeof(ident), comparator, NULL);
}

/**
 * @fn Array *Array::sortedArray(const Array *self, Comparator comparator)
 * @memberof Array
 */
static Array *sortedArray(const Array *self, Comparator comparator) {

  assert(comparator);

  Array *array = (Array *) $((Object *) self, copy);

  $(array, sort, comparator);

  return array;
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

  ((ArrayInterface *) clazz->interface)->addObject = addObject;
  ((ArrayInterface *) clazz->interface)->addObjects = addObjects;
  ((ArrayInterface *) clazz->interface)->addObjectsFromArray = addObjectsFromArray;
  ((ArrayInterface *) clazz->interface)->array = array;
  ((ArrayInterface *) clazz->interface)->arrayWithArray = arrayWithArray;
  ((ArrayInterface *) clazz->interface)->arrayWithCapacity = arrayWithCapacity;
  ((ArrayInterface *) clazz->interface)->arrayWithObjects = arrayWithObjects;
  ((ArrayInterface *) clazz->interface)->arrayWithVaList = arrayWithVaList;
  ((ArrayInterface *) clazz->interface)->componentsJoinedByCharacters = componentsJoinedByCharacters;
  ((ArrayInterface *) clazz->interface)->componentsJoinedByString = componentsJoinedByString;
  ((ArrayInterface *) clazz->interface)->containsObject = containsObject;
  ((ArrayInterface *) clazz->interface)->enumerate = enumerate;
  ((ArrayInterface *) clazz->interface)->filter = filter;
  ((ArrayInterface *) clazz->interface)->filteredArray = filteredArray;
  ((ArrayInterface *) clazz->interface)->find = find;
  ((ArrayInterface *) clazz->interface)->firstObject = firstObject;
  ((ArrayInterface *) clazz->interface)->indexOfObject = indexOfObject;
  ((ArrayInterface *) clazz->interface)->init = init;
  ((ArrayInterface *) clazz->interface)->initWithArray = initWithArray;
  ((ArrayInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
  ((ArrayInterface *) clazz->interface)->initWithObjects = initWithObjects;
  ((ArrayInterface *) clazz->interface)->initWithVaList = initWithVaList;
  ((ArrayInterface *) clazz->interface)->insertObjectAtIndex = insertObjectAtIndex;
  ((ArrayInterface *) clazz->interface)->lastObject = lastObject;
  ((ArrayInterface *) clazz->interface)->map = map;
  ((ArrayInterface *) clazz->interface)->mappedArray = mappedArray;
  ((ArrayInterface *) clazz->interface)->objectAtIndex = objectAtIndex;
  ((ArrayInterface *) clazz->interface)->reduce = reduce;
  ((ArrayInterface *) clazz->interface)->removeAllObjects = removeAllObjects;
  ((ArrayInterface *) clazz->interface)->removeAllObjectsWithEnumerator = removeAllObjectsWithEnumerator;
  ((ArrayInterface *) clazz->interface)->removeLastObject = removeLastObject;
  ((ArrayInterface *) clazz->interface)->removeObject = removeObject;
  ((ArrayInterface *) clazz->interface)->removeObjectAtIndex = removeObjectAtIndex;
  ((ArrayInterface *) clazz->interface)->setObjectAtIndex = setObjectAtIndex;
  ((ArrayInterface *) clazz->interface)->sort = sort;
  ((ArrayInterface *) clazz->interface)->sortedArray = sortedArray;
}

/**
 * @fn Class *Array::_Array(void)
 * @memberof Array
 */
Class *_Array(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "Array",
      .superclass = _Object(),
      .instanceSize = sizeof(Array),
      .interfaceOffset = offsetof(Array, interface),
      .interfaceSize = sizeof(ArrayInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
