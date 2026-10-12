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

#include <check.h>
#include <pthread.h>
#include <unistd.h>

#include <Objectively/Condition.h>
#include <Objectively/ThreadPool.h>

static Condition *condition;
static int counter;
static bool gate;

static ident addOne(Thread *thread) {
  return (ident) ((intptr_t) thread->data + 1);
}

START_TEST(executeAndWait) {

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 2);
  ck_assert(pool != NULL);
  ck_assert_int_eq(2, pool->size);

  for (intptr_t i = 0; i < 8; i++) {
    ThreadPoolWork *work = $(pool, execute, addOne, (ident) i);
    ck_assert(work != NULL);
    ck_assert_int_eq(i + 1, (intptr_t) $(pool, wait, work));
  }

  ck_assert($(pool, wait, NULL) == NULL);

  release(pool);

} END_TEST

static ident incrementAndSignal(Thread *thread) {

  synchronized(condition, {
    counter++;
    $(condition, signal);
  });

  return NULL;
}

START_TEST(detach) {

  condition = $(alloc(Condition), init);
  counter = 0;

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 2);

  for (int i = 0; i < 16; i++) {
    $(pool, detach, incrementAndSignal, NULL);
  }

  synchronized(condition, {
    while (counter < 16) {
      $(condition, wait);
    }
  });

  ck_assert_int_eq(16, counter);

  release(pool);
  release(condition);

} END_TEST

static ident currentPthread(Thread *thread) {
  return (ident) pthread_self();
}

START_TEST(sizeZero) {

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 0);
  ck_assert_int_eq(0, pool->size);

  ThreadPoolWork *work = $(pool, execute, currentPthread, NULL);
  ck_assert(work != NULL);
  ck_assert(pthread_equal((pthread_t) $(pool, wait, work), pthread_self()));

  condition = $(alloc(Condition), init);
  counter = 0;

  $(pool, detach, incrementAndSignal, NULL);
  ck_assert_int_eq(1, counter);

  release(condition);
  release(pool);

} END_TEST

static ident waitForGate(Thread *thread) {

  synchronized(condition, {
    counter++;
    $(condition, broadcast);
    while (gate == false) {
      $(condition, wait);
    }
  });

  return (ident) pthread_self();
}

static pthread_t detachedThreads[8];

static ident recordThread(Thread *thread) {

  synchronized(condition, {
    detachedThreads[counter++] = pthread_self();
    $(condition, broadcast);
  });

  return NULL;
}

START_TEST(queued) {

  condition = $(alloc(Condition), init);
  counter = 0;
  gate = false;

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 1);

  ThreadPoolWork *blocked = $(pool, execute, waitForGate, NULL);

  synchronized(condition, {
    while (counter == 0) {
      $(condition, wait);
    }
  });

  ThreadPoolWork *pending = $(pool, execute, currentPthread, NULL);
  ck_assert(pthread_equal((pthread_t) $(pool, wait, pending), pthread_self()));

  synchronized(condition, {
    counter = 0;
  });

  for (int i = 0; i < 8; i++) {
    $(pool, detach, recordThread, NULL);
  }

  synchronized(condition, {
    ck_assert_int_eq(0, counter);
    gate = true;
    $(condition, broadcast);
  });

  const pthread_t worker = (pthread_t) $(pool, wait, blocked);
  ck_assert(pthread_equal(worker, pthread_self()) == 0);

  synchronized(condition, {
    while (counter < 8) {
      $(condition, wait);
    }
  });

  for (int i = 0; i < 8; i++) {
    ck_assert(pthread_equal(detachedThreads[i], worker));
  }

  release(pool);
  release(condition);

} END_TEST

static ident waitForGateOrCancel(Thread *thread) {

  synchronized(condition, {
    counter++;
    $(condition, broadcast);
  });

  while (__atomic_load_n(&thread->isCancelled, __ATOMIC_ACQUIRE) == false) {
    usleep(1000);
  }

  return NULL;
}

START_TEST(drainOnDealloc) {

  condition = $(alloc(Condition), init);
  counter = 0;

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 1);

  $(pool, detach, waitForGateOrCancel, NULL);

  synchronized(condition, {
    while (counter == 0) {
      $(condition, wait);
    }
  });

  for (int i = 0; i < 8; i++) {
    $(pool, detach, incrementAndSignal, NULL);
  }

  release(pool);

  ck_assert_int_eq(9, counter);

  release(condition);

} END_TEST

static pthread_t outerThread, innerThread;

static ident recordInner(Thread *thread) {
  innerThread = pthread_self();
  return (ident) ((intptr_t) thread->data + 1);
}

static ident waitOnPending(Thread *thread) {

  ThreadPool *pool = thread->data;
  outerThread = pthread_self();

  synchronized(condition, {
    counter++;
    $(condition, broadcast);
  });

  ThreadPoolWork *work = $(pool, execute, recordInner, (ident) 41);
  return $(pool, wait, work);
}

START_TEST(nestedWait) {

  condition = $(alloc(Condition), init);
  counter = 0;

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 1);

  ThreadPoolWork *work = $(pool, execute, waitOnPending, pool);

  synchronized(condition, {
    while (counter == 0) {
      $(condition, wait);
    }
  });

  ck_assert_int_eq(42, (intptr_t) $(pool, wait, work));
  ck_assert(pthread_equal(outerThread, pthread_self()) == 0);
  ck_assert(pthread_equal(innerThread, outerThread));

  release(pool);
  release(condition);

} END_TEST

static ident untilCancelled(Thread *thread) {

  synchronized(condition, {
    counter++;
    $(condition, broadcast);
  });

  while (__atomic_load_n(&thread->isCancelled, __ATOMIC_ACQUIRE) == false) {
    usleep(1000);
  }

  synchronized(condition, {
    gate = true;
  });

  return NULL;
}

START_TEST(cancelOnDealloc) {

  condition = $(alloc(Condition), init);
  counter = 0;
  gate = false;

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 1);

  $(pool, detach, untilCancelled, NULL);

  synchronized(condition, {
    while (counter == 0) {
      $(condition, wait);
    }
  });

  release(pool);

  ck_assert(gate);

  release(condition);

} END_TEST

static ident increment(Thread *thread) {

  synchronized(condition, {
    counter++;
  });

  return NULL;
}

START_TEST(concurrent) {

  condition = $(alloc(Condition), init);
  counter = 0;

  ThreadPool *pool = $(alloc(ThreadPool), initWithSize, 4);

  ThreadPoolWork *work[64];
  for (int i = 0; i < 64; i++) {
    work[i] = $(pool, execute, increment, NULL);
  }

  for (int i = 0; i < 64; i++) {
    $(pool, wait, work[i]);
  }

  ck_assert_int_eq(64, counter);

  release(pool);
  release(condition);

} END_TEST

START_TEST(init) {

  ThreadPool *pool = $(alloc(ThreadPool), init);
  ck_assert(pool != NULL);
  ck_assert(pool->size > 0);

  release(pool);

} END_TEST

int main(int argc, char **argv) {

  TCase *tcase = tcase_create("ThreadPool");
  tcase_add_test(tcase, cancelOnDealloc);
  tcase_add_test(tcase, concurrent);
  tcase_add_test(tcase, detach);
  tcase_add_test(tcase, drainOnDealloc);
  tcase_add_test(tcase, executeAndWait);
  tcase_add_test(tcase, init);
  tcase_add_test(tcase, nestedWait);
  tcase_add_test(tcase, queued);
  tcase_add_test(tcase, sizeZero);

  Suite *suite = suite_create("ThreadPool");
  suite_add_tcase(suite, tcase);

  SRunner *runner = srunner_create(suite);

  srunner_run_all(runner, CK_VERBOSE);
  int failed = srunner_ntests_failed(runner);

  srunner_free(runner);

  return failed;
}
