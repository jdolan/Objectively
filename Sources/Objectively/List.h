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
 * @brief Doubly-linked lists of raw C pointers.
 */

typedef struct List List;
typedef struct ListInterface ListInterface;
typedef struct ListNode ListNode;

/**
 * @brief A function called to destroy a list element on removal.
 */
typedef void (*ListDestroyFunc)(ident obj);

/**
 * @brief The ListEnumerator function type.
 * @param list The List.
 * @param node The current node.
 * @param data User data.
 * @return True to stop enumeration, false to continue.
 */
typedef bool (*ListEnumerator)(const List *list, ListNode *node, ident data);

/**
 * @brief Comparator function for sorting list elements.
 * @return Negative if a < b, zero if equal, positive if a > b.
 */
typedef int (*ListSortFunc)(const ident a, const ident b);

/**
 * @brief A node in a List.
 */
struct ListNode {
  ident data;
  ListNode *prev;
  ListNode *next;
};

/**
 * @brief Doubly-linked lists of raw C pointers.
 * @extends Object
 * @ingroup Collections
 */
struct List {

  /**
   * @brief The superclass.
   */
  Object object;

  /**
   * @brief The interface.
   * @protected
   */
  ListInterface *interface;

  /**
   * @brief The number of elements.
   */
  size_t count;

  /**
   * @brief The head node.
   */
  ListNode *head;

  /**
   * @brief The tail node.
   */
  ListNode *tail;

  /**
   * @brief Optional destructor called when an element is removed.
   */
  ListDestroyFunc destroy;
};

/**
 * @brief The List interface.
 */
struct ListInterface {

  /**
   * @brief The superclass interface.
   */
  ObjectInterface objectInterface;

  /**
   * @fn void List::append(List *self, const ident data)
   * @brief Appends the given element to the tail of this List.
   * @param self The List.
   * @param data The element to append.
   * @memberof List
   */
  void (*append)(List *self, const ident data);

  /**
   * @fn bool List::containsElement(const List *self, const ident data)
   * @param self The List.
   * @param data The element to search for (pointer equality).
   * @return True if this List contains the given element.
   * @memberof List
   */
  bool (*containsElement)(const List *self, const ident data);

  /**
   * @fn void List::enumerateElements(const List *self, ListEnumerator enumerator, ident data)
   * @brief Enumerates this List with the given function.
   * @param self The List.
   * @param enumerator The enumerator function.
   * @param data User data.
   * @memberof List
   */
  void (*enumerateElements)(const List *self, ListEnumerator enumerator, ident data);

  /**
   * @fn ListNode *List::findNode(const List *self, const ident data)
   * @param self The List.
   * @param data The element to find (pointer equality).
   * @return The first node whose data matches, or NULL.
   * @memberof List
   */
  ListNode *(*findNode)(const List *self, const ident data);

  /**
   * @fn List *List::init(List *self)
   * @brief Initializes this List.
   * @param self The List.
   * @return The initialized List, or NULL on error.
   * @memberof List
   */
  List *(*init)(List *self);

  /**
   * @fn void List::insertAfter(List *self, ListNode *node, const ident data)
   * @brief Inserts an element after the given node.
   * @param self The List.
   * @param node The node to insert after, or NULL to prepend.
   * @param data The element to insert.
   * @memberof List
   */
  void (*insertAfter)(List *self, ListNode *node, const ident data);

  /**
   * @fn void List::prepend(List *self, const ident data)
   * @brief Prepends the given element to the head of this List.
   * @param self The List.
   * @param data The element to prepend.
   * @memberof List
   */
  void (*prepend)(List *self, const ident data);

  /**
   * @fn void List::removeAll(List *self)
   * @brief Removes all elements from this List.
   * @param self The List.
   * @memberof List
   */
  void (*removeAll)(List *self);

  /**
   * @fn void List::removeNode(List *self, ListNode *node)
   * @brief Removes the given node from this List.
   * @param self The List.
   * @param node The node to remove.
   * @memberof List
   */
  void (*removeNode)(List *self, ListNode *node);

  /**
   * @fn void List::sort(List *self, ListSortFunc sort)
   * @brief Sorts this List in-place using the given comparator.
   * @param self The List.
   * @param sort The comparator function.
   * @memberof List
   */
  void (*sort)(List *self, ListSortFunc sort);
};

/**
 * @fn Class *List::_List(void)
 * @brief The List archetype.
 * @return The List Class.
 * @memberof List
 */
OBJECTIVELY_EXPORT Class *_List(void);
