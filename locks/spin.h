#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>

/**
 * Based off of test-and-test-and-set lock.
 */

struct _spinlock {
    bool flag;
};

typedef struct _spinlock spinlock_t;

static inline void spinlock_lock(spinlock_t *lock) {
    while (atomic_flag_test_and_set(&lock->flag))
        /* busy wait */
        while (lock->flag)
            /* busy wait until lock appears to be free*/;

    asm volatile ("" : : : "memory");
}

static inline void spinlock_unlock(spinlock_t *lock) {
    atomic_store(&lock->flag, false);
    asm volatile ("" : : : "memory");
}

#endif
