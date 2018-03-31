#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <assert.h>
#include <locks/ticket.h>

ticket_lock_t lock = TICKET_LOCK_INIT(5000);
pthread_spinlock_t spinlock;

bool use_locks = true;

bool use_pthread_mutex;

volatile int current;

static void *thread_start(void *arg) {
    const int threadnum = (long) arg;
    struct timespec sleepamt;

    if (use_locks) {
        if (use_pthread_mutex)
            pthread_spin_lock(&spinlock);
        else 
            ticket_lock(&lock);
    }

    printf("thread %d: I acquired it!\n", threadnum);
    printf("\tthread %d: these numbers should be in order and contiguous:\n", threadnum);
    for (unsigned i=1; i<=10; ++i)
        printf("\tthread %d: %d\n", threadnum, i);

    current = threadnum;
    sleepamt.tv_sec = 0;
    sleepamt.tv_nsec = 1000;
    for (int i=0; i<10000; ++i) {
        assert(current == threadnum);
        nanosleep(&sleepamt, NULL);
        current = threadnum;
    }

    if (use_locks) {
        if (use_pthread_mutex)
            pthread_spin_unlock(&spinlock);
        else
            ticket_unlock(&lock);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    static struct option options[] = {
        { "numthreads", 1,  0,  'n' },
        { "pmutex", 0,  0,  'p' },
        { "unlocked", 0, 0, 'u' },
        { 0, 0, 0, 0 }
    };

    int numthreads = 1;

    int opt;
    while ((opt = getopt_long(argc, argv, "n:pu", options, NULL)) != -1) {
        switch (opt) {
            case 'n':
            {
                numthreads = strtol(optarg, NULL, 10);
                if (errno) {
                    fprintf(stderr, "Invalid argument for 'numthreads': %s\n", strerror(errno));
                    exit(EXIT_FAILURE);
                } else if (numthreads < 1) {
                    fprintf(stderr, "Invalid argument for 'numthreads': number must be >=1\n");
                    exit(EXIT_FAILURE);
                }
            }   break;
            case 'p':
                use_pthread_mutex = true;
                break;
            case 'u':
                use_locks = false;
                break;
            case '?':
                exit(EXIT_FAILURE);
                break;
        }
    }

    printf("running tests on %d threads with %s...\n", numthreads, 
            use_pthread_mutex ? "pthreads mutexes" : "ticket lock");

    pthread_t threads[numthreads-1];

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    for (long t=1; t<numthreads; ++t) {
        if (pthread_create(&threads[t-1], NULL, &thread_start, (void *) t)) {
            perror("pthread_create");
            abort();
        }
    }

    thread_start((void *) 0L);

    for (long t=0; t<numthreads-1; ++t) {
        pthread_join(threads[t], NULL);
    }

    pthread_spin_destroy(&spinlock);

    return 0;
}
