#ifndef BAKERY_H
#define BAKERY_H

#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Based off of Lamport's bakery algorithm.
 */
struct _bakery_lock {
    unsigned maxthreads;
    volatile bool *choosing;
    volatile uint64_t *number;
};

typedef struct _bakery_lock bakery_lock_t;

static void bakery_lock_init(bakery_lock_t *lock, unsigned maxthreads) {
    lock->maxthreads = maxthreads;
    lock->choosing = calloc(maxthreads, sizeof lock->choosing);
    lock->number = calloc(maxthreads, sizeof lock->number);
}

static void bakery_lock_destroy(bakery_lock_t *lock) {
    free((void *) lock->choosing);
    free((void *) lock->number);
    lock->maxthreads = 0;
}

static void bakery_lock(bakery_lock_t *lock, unsigned threadnum) {
    assert(threadnum < lock->maxthreads);

    atomic_store(&lock->choosing[threadnum], true);
    uint64_t m = 0;

    for (unsigned i=0; i<lock->maxthreads; ++i) {
        if (lock->number[i] > m)
            m = lock->number[i];
    }

    m++;
    atomic_store(&lock->number[threadnum], m);
    atomic_store(&lock->choosing[threadnum], false);

    for (unsigned i=0; i<lock->maxthreads; ++i) {
        if (i == threadnum)
            continue;
        while (lock->choosing[i])
            /* busy wait */;

        while (true) {
            uint64_t t = lock->number[i];
            if (t == 0 || (t > m || (t == m && i > threadnum)))
                break;
        }
    }

    asm volatile ("" : : : "memory");
}

static void bakery_unlock(bakery_lock_t *lock, unsigned threadnum) {
    assert(threadnum < lock->maxthreads);

    atomic_store(&lock->number[threadnum], 0);

    asm volatile ("" : : : "memory");
}

#endif
