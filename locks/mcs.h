#ifndef MCS_H
#define MCS_H

#include <stdint.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

struct _qnode {
    volatile struct _qnode *next;
    volatile bool waiting;
};

typedef struct _qnode qnode;

#define QNODE_INIT { NULL, false }

/**
 * MCS queued lock.
 */
struct _mcs_lock {
    volatile qnode *tail;
};

typedef struct _mcs_lock mcs_lock_t;

static inline void mcs_lock(mcs_lock_t *lock, qnode *p) {
    p->next = NULL;
    p->waiting = true;

    volatile qnode *prev = atomic_exchange(&lock->tail, p);

    /* There was another thread already at the tail
     * end of the queue. We've moved it back and
     * placed ourselves on the tail end. */
    if (prev) {
        /* now we set the previous queue node to point to us */
        atomic_store(&prev->next, p);
        while (atomic_load(&p->waiting))
            /* we wait for the previous thread to release */;
    }

    atomic_thread_fence(memory_order_acquire);
}

static inline void mcs_unlock(mcs_lock_t *lock, qnode *p) {
    volatile qnode *succ = atomic_load(&p->next);

    if (!succ) {
        /* If this CAS succeeds, then we were the last thing.
         * We set the tail to NULL.
         */
        if (atomic_compare_exchange_strong(&lock->tail, &p, NULL))
            return;
        /*
         * Otherwise, there was something else at the tail, but it has yet to
         * set our [next] pointer to itself.
         */
        while (!succ)
            succ = atomic_load(&p->next);
    }

    atomic_store(&succ->waiting, false);
    atomic_thread_fence(memory_order_release);
}

#endif
