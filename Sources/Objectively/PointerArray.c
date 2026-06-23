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
#include <string.h>

#include "Array.h"
#include "PointerArray.h"

#define _Class _PointerArray

#define POINTER_ARRAY_CHUNK_SIZE 64

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  PointerArray *this = (PointerArray *) self;

  $(this, removeAll);

  free(this->elements);

  super(Object, self, dealloc);
}

#pragma mark - PointerArray

/**
 * @fn void PointerArray::add(PointerArray *self, ident pointer)
 * @memberof PointerArray
 */
static void add(PointerArray *self, ident pointer) {

  if (self->count == self->capacity) {

    self->capacity += POINTER_ARRAY_CHUNK_SIZE;

    self->elements = realloc(self->elements, self->capacity * sizeof(ident ));
    assert(self->elements);
  }

  self->elements[self->count++] = pointer;
}

/**
 * @fn ident PointerArray::get(const PointerArray *self, size_t index)
 * @memberof PointerArray
 */
static ident get(const PointerArray *self, size_t index) {

  assert(index < self->count);

  return self->elements[index];
}

/**
 * @fn PointerArray *PointerArray::init(PointerArray *self)
 * @memberof PointerArray
 */
static PointerArray *init(PointerArray *self) {
  return $(self, initWithDestroy, NULL);
}

/**
 * @fn PointerArray *PointerArray::initWithDestroy(PointerArray *self, Consumer destroy)
 * @memberof PointerArray
 */
static PointerArray *initWithDestroy(PointerArray *self, Consumer destroy) {

  self = (PointerArray *) super(Object, self, init);
  if (self) {
    self->destroy = destroy;
  }

  return self;
}

/**
 * @fn void PointerArray::remove(PointerArray *self, ident pointer)
 * @memberof PointerArray
 */
static void _remove(PointerArray *self, ident pointer) {

  for (size_t i = 0; i < self->count; i++) {
    if (self->elements[i] == pointer) {
      $(self, removeAt, i);
      return;
    }
  }
}

/**
 * @fn void PointerArray::removeAll(PointerArray *self)
 * @memberof PointerArray
 */
static void removeAll(PointerArray *self) {

  if (self->destroy) {
    for (size_t i = 0; i < self->count; i++) {
      self->destroy(self->elements[i]);
    }
  }

  self->count = 0;
}

/**
 * @fn void PointerArray::removeAt(PointerArray *self, size_t index)
 * @memberof PointerArray
 */
static void removeAt(PointerArray *self, size_t index) {

  assert(index < self->count);

  if (self->destroy) {
    self->destroy(self->elements[index]);
  }

  const size_t tail = self->count - index - 1;
  memmove(self->elements + index, self->elements + index + 1, tail * sizeof(ident ));

  self->count--;
}

/**
 * @fn void PointerArray::sort(PointerArray *self, Comparator comparator)
 * @memberof PointerArray
 */
static void sort(PointerArray *self, Comparator comparator) {
  quicksort(self->elements, self->count, sizeof(ident), comparator, NULL);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((PointerArrayInterface *) clazz->interface)->add = add;
  ((PointerArrayInterface *) clazz->interface)->get = get;
  ((PointerArrayInterface *) clazz->interface)->init = init;
  ((PointerArrayInterface *) clazz->interface)->initWithDestroy = initWithDestroy;
  ((PointerArrayInterface *) clazz->interface)->remove = _remove;
  ((PointerArrayInterface *) clazz->interface)->removeAll = removeAll;
  ((PointerArrayInterface *) clazz->interface)->removeAt = removeAt;
  ((PointerArrayInterface *) clazz->interface)->sort = sort;
}

/**
 * @fn Class *PointerArray::_PointerArray(void)
 * @memberof PointerArray
 */
Class *_PointerArray(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "PointerArray",
      .superclass = _Object(),
      .instanceSize = sizeof(PointerArray),
      .interfaceOffset = offsetof(PointerArray, interface),
      .interfaceSize = sizeof(PointerArrayInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
