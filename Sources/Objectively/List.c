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
 * @fn void List::append(List *self, const ident element)
 * @memberof List
 */
static void append(List *self, const ident element) {

  ListNode *node = calloc(1, sizeof(ListNode));
  assert(node);

  node->element = element;
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
 * @fn bool List::containsElement(const List *self, const ident element)
 * @memberof List
 */
static bool containsElement(const List *self, const ident element) {
  return $(self, nodeForElement, element) != NULL;
}

/**
 * @fn void List::enumerate(const List *self, ListEnumerator enumerator, ident element)
 * @memberof List
 */
static void enumerate(const List *self, ListEnumerator enumerator, ident element) {

  assert(enumerator);

  for (ListNode *node = self->head; node; ) {
    ListNode *next = node->next;
    if (enumerator(self, node, element)) {
      break;
    }
    node = next;
  }
}

/**
 * @fn void List::filter(List *self, Predicate predicate, ident data)
 * @memberof List
 */
static void filter(List *self, Predicate predicate, ident data) {

  assert(predicate);

  for (ListNode *node = self->head; node; ) {
    ListNode *next = node->next;
    if (!predicate(node->element, data)) {
      $(self, removeNode, node);
    }
    node = next;
  }
}

/**
 * @fn List *List::filteredList(const List *self, Predicate predicate, ident data)
 * @memberof List
 */
static List *filteredList(const List *self, Predicate predicate, ident data) {

  assert(predicate);

  List *list = $(alloc(List), init);
  for (ListNode *node = self->head; node; node = node->next) {
    if (predicate(node->element, data)) {
      $(list, appendElement, node->element);
    }
  }

  return list;
}

/**
 * @fn ident List::find(const List *self, Predicate predicate, ident data)
 * @memberof List
 */
static ident find(const List *self, Predicate predicate, ident data) {

  assert(predicate);

  for (ListNode *node = self->head; node; node = node->next) {
    if (predicate(node->element, data)) {
      return node->element;
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
 * @fn void List::insertElementAfter(List *self, ListNode *node, const ident element)
 * @memberof List
 */
static void insertElementAfter(List *self, ListNode *node, const ident element) {

  if (node == NULL || node == self->tail) {
    $(self, appendElement, element);
    return;
  }

  ListNode *newNode = calloc(1, sizeof(ListNode));
  assert(newNode);

  newNode->element = element;
  newNode->prev = node;
  newNode->next = node->next;

  if (node->next) {
    node->next->prev = newNode;
  }
  node->next = newNode;

  self->count++;
}

/**
 * @fn void List::map(List *self, Functor functor, ident data)
 * @memberof List
 */
static void map(List *self, Functor functor, ident data) {

  assert(functor);

  for (ListNode *node = self->head; node; node = node->next) {
    node->element = functor(node->element, data);
  }
}

/**
 * @fn List *List::mappedList(const List *self, Functor functor, ident data)
 * @memberof List
 */
static List *mappedList(const List *self, Functor functor, ident data) {

  assert(functor);

  List *list = $(alloc(List), init);
  for (ListNode *node = self->head; node; node = node->next) {
    $(list, appendElement, functor(node->element, data));
  }

  return list;
}

/**
 * @fn ListNode *List::nodeForElement(const List *self, const ident element)
 * @memberof List
 */
static ListNode *nodeForElement(const List *self, const ident element) {

  for (ListNode *node = self->head; node; node = node->next) {
    if (node->element == element) {
      return node;
    }
  }

  return NULL;
}

/**
 * @fn void List::prependElement(List *self, const ident element)
 * @memberof List
 */
static void prependElement(List *self, const ident element) {

  ListNode *node = calloc(1, sizeof(ListNode));
  assert(node);

  node->element = element;
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
 * @fn ident List::reduce(const List *self, Reducer reducer, ident accumulator, ident data)
 * @memberof List
 */
static ident reduce(const List *self, Reducer reducer, ident accumulator, ident data) {

  assert(reducer);

  for (ListNode *node = self->head; node; node = node->next) {
    accumulator = reducer(node->element, accumulator, data);
  }

  return accumulator;
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
      self->destroy(node->element);
    }
    free(node);
    node = next;
  }

  self->head = self->tail = NULL;
  self->count = 0;
}

/**
 * @fn void List::removeElement(List *self, const ident element)
 * @memberof List
 */
static void removeElement(List *self, const ident element) {

  ListNode *node = $(self, nodeForElement, element);
  if (node) {
    $(self, removeNode, node);
  }
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
    self->destroy(node->element);
  }

  free(node);
  self->count--;
}

/**
 * @fn void List::sort(List *self, ListSortFunc sort)
 * @memberof List
 */
static void _sort(List *self, Comparator comparator) {

  assert(comparator);

  if (self->count < 2) {
    return;
  }

  for (ListNode *node = self->head->next; node; ) {
    ListNode *next = node->next;
    ident key = node->element;

    ListNode *j = node->prev;
    while (j && comparator(j->element, key) > OrderSame) {
      j->next->element = j->element;
      j = j->prev;
    }

    (j ? j->next : self->head)->element = key;
    node = next;
  }
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

  ((ObjectInterface *) clazz->interface)->dealloc = dealloc;

  ((ListInterface *) clazz->interface)->appendElement = append;
  ((ListInterface *) clazz->interface)->containsElement = containsElement;
  ((ListInterface *) clazz->interface)->enumerate = enumerate;
  ((ListInterface *) clazz->interface)->filter = filter;
  ((ListInterface *) clazz->interface)->filteredList = filteredList;
  ((ListInterface *) clazz->interface)->find = find;
  ((ListInterface *) clazz->interface)->init = init;
  ((ListInterface *) clazz->interface)->insertElementAfter = insertElementAfter;
  ((ListInterface *) clazz->interface)->map = map;
  ((ListInterface *) clazz->interface)->mappedList = mappedList;
  ((ListInterface *) clazz->interface)->nodeForElement = nodeForElement;
  ((ListInterface *) clazz->interface)->prependElement = prependElement;
  ((ListInterface *) clazz->interface)->reduce = reduce;
  ((ListInterface *) clazz->interface)->removeAll = removeAll;
  ((ListInterface *) clazz->interface)->removeElement = removeElement;
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
