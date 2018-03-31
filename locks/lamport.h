#ifndef LAMPORT_H 
#define LAMPORT_H

#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <assert.h>

/**
 * Implemented using Lamport's Fast Algorithm.
 */
struct _lamport_lock {
    unsigned maxthreads;
    volatile long x;
    volatile long y;
    volatile bool *trying;
};

typedef struct _lamport_lock lamport_lock_t;

static void lamport_lock_init(lamport_lock_t *lock, unsigned maxthreads) {
    lock->maxthreads = maxthreads;
    lock->x = -1;
    lock->y = -1;
    lock->trying = calloc(maxthreads, sizeof lock->trying);
}

static void lamport_lock_destroy(lamport_lock_t *lock) {
    free((void *) lock->trying);
}

static void lamport_lock(lamport_lock_t *lock, unsigned threadnum) {
    while (true) {
        atomic_store(&lock->trying[threadnum], true);
        atomic_store(&lock->x, threadnum);

        if (lock->y != -1) {
            atomic_store(&lock->trying[threadnum], false);
            while (lock->y != -1)
                /* busy wait */;
            continue;
        }
        atomic_store(&lock->y, threadnum);

        if (lock->x != threadnum) {
            atomic_store(&lock->trying[threadnum], false);
            for (unsigned i=0; i<lock->maxthreads; ++i)
                while (lock->trying[i])
                    /* busy wait */;
            if (lock->y != threadnum) {
                while (lock->y != -1)
                    /* busy wait */;
                continue;
            }
        }
        break;
    }
    asm volatile ("" : : : "memory");
}

static void lamport_unlock(lamport_lock_t *lock, unsigned threadnum) {
    atomic_store(&lock->y, -1);
    atomic_store(&lock->trying[threadnum], false);
    asm volatile ("" : : : "memory");
}

#endif
