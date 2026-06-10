#include <check.h>
#include <stdlib.h>
#include <pthread.h>
#include "Objectively/Object.h"
#include "Objectively/Class.h"

#define NUM_THREADS 4
#define ITERATIONS 1000

static void *retain_release_worker(void *arg) {
    Object *obj = (Object *)arg;
    for (int i = 0; i < ITERATIONS; i++) {
        retain(obj);
        release(obj);
    }
    return NULL;
}

START_TEST(test_refcount_no_use_after_free)
{
    // Invariant: An object's reference count must never go below zero,
    // and concurrent retain/release must not cause use-after-free.
    // After all threads complete, the object must still be valid with refcount == 1.

    Object *obj = (Object *)$$(Object, alloc(Object));
    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->referenceCount, 1);

    pthread_t threads[NUM_THREADS];
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_create(&threads[t], NULL, retain_release_worker, obj);
    }
    for (int t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    // After balanced retain/release from all threads, refcount must be exactly 1
    ck_assert_msg(obj->referenceCount == 1,
        "Reference count is %d, expected 1 after balanced retain/release",
        obj->referenceCount);

    // Boundary: retain then release should leave refcount unchanged
    retain(obj);
    ck_assert_int_eq(obj->referenceCount, 2);
    release(obj);
    ck_assert_int_eq(obj->referenceCount, 1);

    // Final release deallocates
    release(obj);
}
END_TEST

START_TEST(test_refcount_boundary_single_thread)
{
    // Invariant: Multiple retains followed by equal releases must reach zero exactly once
    Object *obj = (Object *)$$(Object, alloc(Object));
    ck_assert_ptr_nonnull(obj);

    // Retain 3 times (refcount -> 4)
    retain(obj);
    retain(obj);
    retain(obj);
    ck_assert_int_eq(obj->referenceCount, 4);

    // Release 3 times (refcount -> 1)
    release(obj);
    release(obj);
    release(obj);
    ck_assert_int_eq(obj->referenceCount, 1);

    release(obj); // final dealloc
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_refcount_no_use_after_free);
    tcase_add_test(tc_core, test_refcount_boundary_single_thread);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}