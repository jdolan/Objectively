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

#include "List.h"

#define _Class _List

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

  List *this = (List *) self;
  $(this, removeAll);

  super(Object, self, dealloc);
}

#pragma mark - List

/**
 * @fn void List::append(List *self, const ident data)
 * @memberof List
 */
static void append(List *self, const ident data) {

  ListNode *node = calloc(1, sizeof(ListNode));
  assert(node);

  node->data = data;
  node->prev = self->tail;

  if (self->tail) {
    self->tail->next = node;
  } else {
    self->head = node;
  }

  self->tail = node;
  self->count++;
}

/**
 * @fn bool List::containsElement(const List *self, const ident data)
 * @memberof List
 */
static bool containsElement(const List *self, const ident data) {
  return $(self, findNode, data) != NULL;
}

/**
 * @fn void List::enumerateElements(const List *self, ListEnumerator enumerator, ident data)
 * @memberof List
 */
static void enumerateElements(const List *self, ListEnumerator enumerator, ident data) {

  assert(enumerator);

  for (ListNode *node = self->head; node; ) {
    ListNode *next = node->next;
    if (enumerator(self, node, data)) {
      break;
    }
    node = next;
  }
}

/**
 * @fn ListNode *List::findNode(const List *self, const ident data)
 * @memberof List
 */
static ListNode *findNode(const List *self, const ident data) {

  for (ListNode *node = self->head; node; node = node->next) {
    if (node->data == data) {
      return node;
    }
  }

  return NULL;
}

/**
 * @fn List *List::init(List *self)
 * @memberof List
 */
static List *init(List *self) {

  self = (List *) super(Object, self, init);
  return self;
}

/**
 * @fn void List::insertAfter(List *self, ListNode *node, const ident data)
 * @memberof List
 */
static void insertAfter(List *self, ListNode *node, const ident data) {

  if (node == NULL || node == self->tail) {
    $(self, append, data);
    return;
  }

  ListNode *newNode = calloc(1, sizeof(ListNode));
  assert(newNode);

  newNode->data = data;
  newNode->prev = node;
  newNode->next = node->next;

  if (node->next) {
    node->next->prev = newNode;
  }
  node->next = newNode;

  self->count++;
}

/**
 * @fn void List::prepend(List *self, const ident data)
 * @memberof List
 */
static void prepend(List *self, const ident data) {

  ListNode *node = calloc(1, sizeof(ListNode));
  assert(node);

  node->data = data;
  node->next = self->head;

  if (self->head) {
    self->head->prev = node;
  } else {
    self->tail = node;
  }

  self->head = node;
  self->count++;
}

/**
 * @fn void List::removeAll(List *self)
 * @memberof List
 */
static void removeAll(List *self) {

  ListNode *node = self->head;
  while (node) {
    ListNode *next = node->next;
    if (self->destroy) {
      self->destroy(node->data);
    }
    free(node);
    node = next;
  }

  self->head = self->tail = NULL;
  self->count = 0;
}

/**
 * @fn void List::removeNode(List *self, ListNode *node)
 * @memberof List
 */
static void removeNode(List *self, ListNode *node) {

  assert(node);

  if (node->prev) {
    node->prev->next = node->next;
  } else {
    self->head = node->next;
  }

  if (node->next) {
    node->next->prev = node->prev;
  } else {
    self->tail = node->prev;
  }

  if (self->destroy) {
    self->destroy(node->data);
  }

  free(node);
  self->count--;
}

/**
 * @fn void List::sort(List *self, ListSortFunc sort)
 * @memberof List
 * @brief Insertion sort — appropriate for the list sizes typical in Quetoo.
 */
static void _sort(List *self, ListSortFunc sortFn) {

  assert(sortFn);

  if (self->count < 2) {
    return;
  }

  for (ListNode *i = self->head->next; i; ) {
    ListNode *next = i->next;
    ident key = i->data;

    ListNode *j = i->prev;
    while (j && sortFn(j->data, key) > 0) {
      j->next->data = j->data;
      j = j->prev;
    }

    (j ? j->next : self->head)->data = key;
    i = next;
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ListInterface *) clazz->interface)->append = append;
  ((ListInterface *) clazz->interface)->containsElement = containsElement;
  ((ListInterface *) clazz->interface)->enumerateElements = enumerateElements;
  ((ListInterface *) clazz->interface)->findNode = findNode;
  ((ListInterface *) clazz->interface)->init = init;
  ((ListInterface *) clazz->interface)->insertAfter = insertAfter;
  ((ListInterface *) clazz->interface)->prepend = prepend;
  ((ListInterface *) clazz->interface)->removeAll = removeAll;
  ((ListInterface *) clazz->interface)->removeNode = removeNode;
  ((ListInterface *) clazz->interface)->sort = _sort;
}

/**
 * @fn Class *List::_List(void)
 * @memberof List
 */
Class *_List(void) {
  static Class *clazz;
  static Once once;

  do_once(&once, {
    clazz = _initialize(&(const ClassDef) {
      .name = "List",
      .superclass = _Object(),
      .instanceSize = sizeof(List),
      .interfaceOffset = offsetof(List, interface),
      .interfaceSize = sizeof(ListInterface),
      .initialize = initialize,
    });
  });

  return clazz;
}

#undef _Class
