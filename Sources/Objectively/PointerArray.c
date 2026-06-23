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

#include "PointerArray.h"

#define _Class _PointerArray

#define POINTER_ARRAY_CHUNK_SIZE 64

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  PointerArray *this = (PointerArray *) self;

  $(this, removeAllPointers);

  free(this->elements);

  super(Object, self, dealloc);
}

#pragma mark - PointerArray

/**
 * @fn void PointerArray::addPointer(PointerArray *self, void *pointer)
 * @memberof PointerArray
 */
static void addPointer(PointerArray *self, void *pointer) {

  if (self->count == self->capacity) {

    self->capacity += POINTER_ARRAY_CHUNK_SIZE;

    self->elements = realloc(self->elements, self->capacity * sizeof(void *));
    assert(self->elements);
  }

  self->elements[self->count++] = pointer;
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
 * @fn void *PointerArray::pointerAtIndex(const PointerArray *self, size_t index)
 * @memberof PointerArray
 */
static void *pointerAtIndex(const PointerArray *self, size_t index) {

  assert(index < self->count);

  return self->elements[index];
}

/**
 * @fn void PointerArray::removeAllPointers(PointerArray *self)
 * @memberof PointerArray
 */
static void removeAllPointers(PointerArray *self) {

  if (self->destroy) {
    for (size_t i = 0; i < self->count; i++) {
      self->destroy(self->elements[i]);
    }
  }

  self->count = 0;
}

/**
 * @fn void PointerArray::removePointer(PointerArray *self, void *pointer)
 * @memberof PointerArray
 */
static void removePointer(PointerArray *self, void *pointer) {

  for (size_t i = 0; i < self->count; i++) {
    if (self->elements[i] == pointer) {
      $(self, removePointerAtIndex, i);
      return;
    }
  }
}

/**
 * @fn void PointerArray::removePointerAtIndex(PointerArray *self, size_t index)
 * @memberof PointerArray
 */
static void removePointerAtIndex(PointerArray *self, size_t index) {

  assert(index < self->count);

  if (self->destroy) {
    self->destroy(self->elements[index]);
  }

  const size_t tail = self->count - index - 1;
  memmove(self->elements + index, self->elements + index + 1, tail * sizeof(void *));

  self->count--;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((PointerArrayInterface *) clazz->interface)->addPointer = addPointer;
  ((PointerArrayInterface *) clazz->interface)->init = init;
  ((PointerArrayInterface *) clazz->interface)->initWithDestroy = initWithDestroy;
  ((PointerArrayInterface *) clazz->interface)->pointerAtIndex = pointerAtIndex;
  ((PointerArrayInterface *) clazz->interface)->removeAllPointers = removeAllPointers;
  ((PointerArrayInterface *) clazz->interface)->removePointer = removePointer;
  ((PointerArrayInterface *) clazz->interface)->removePointerAtIndex = removePointerAtIndex;
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
