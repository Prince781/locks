#ifndef TICKET_H
#define TICKET_H

#include <stdint.h>
#include <stdatomic.h>
#include <time.h>

struct _ticket_lock {
    volatile uint64_t next_ticket;
    volatile uint64_t now_serving;
    uint64_t base;
};

typedef struct _ticket_lock ticket_lock_t;

#define TICKET_LOCK_INIT(base_ns)   {0, 0, base_ns}

static void ticket_lock(ticket_lock_t *lock) {
    uint64_t my_ticket = atomic_fetch_add(&lock->next_ticket, 1);

    while (1) {
        uint64_t ns = lock->now_serving;

        if (ns == my_ticket)
            break;
        uint64_t tm = lock->base * (my_ticket - ns);
        struct timespec sleepamt = { tm / 1000000000, tm % 1000000000 };
        nanosleep(&sleepamt, NULL);
    }

    asm volatile ("" : : : "memory");
}

static void ticket_unlock(ticket_lock_t *lock) {
    uint64_t t = lock->now_serving + 1;
    atomic_store(&lock->now_serving, t);

    asm volatile ("" : : : "memory");
}

#endif
