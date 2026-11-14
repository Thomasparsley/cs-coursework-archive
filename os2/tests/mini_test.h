// Tiny test harness: CHECK macros, a test registry and a watchdog thread that
// turns a hang into a failure (the whole run is aborted with exit code 2).
//
// Deliberately does not include <unistd.h>: petrzela-tomas-1-lin.c defines a
// typedef named `pipe`, which clashes with pipe() from <unistd.h>.
#ifndef MINI_TEST_H
#define MINI_TEST_H

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static atomic_int mt_checks;
static atomic_int mt_failures;
static const char *_Atomic mt_current = "";
static atomic_long mt_deadline_ms;

static inline long mt_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long)ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}

static inline void mt_sleep_ms(long ms)
{
    struct timespec ts;
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

#define CHECK(cond)                                                       \
    do {                                                                  \
        atomic_fetch_add(&mt_checks, 1);                                  \
        if (!(cond)) {                                                    \
            atomic_fetch_add(&mt_failures, 1);                            \
            printf("  FAIL %s:%d: %s (in %s)\n", __FILE__, __LINE__,      \
                   #cond, (const char *)atomic_load(&mt_current));                                    \
        }                                                                 \
    } while (0)

#define CHECK_EQ(a, b) CHECK((long long)(a) == (long long)(b))

// Waits until *flag becomes non-zero. Returns 1 if it did, 0 on timeout.
static inline int mt_wait_flag(atomic_int *flag, long timeout_ms)
{
    long end = mt_now_ms() + timeout_ms;
    while (mt_now_ms() < end)
    {
        if (atomic_load(flag))
            return 1;
        mt_sleep_ms(1);
    }
    return atomic_load(flag);
}

static void *mt_watchdog(void *arg)
{
    (void)arg;
    for (;;)
    {
        mt_sleep_ms(50);
        if (mt_now_ms() > atomic_load(&mt_deadline_ms))
        {
            printf("TIMEOUT (hang) in test: %s\n", (const char *)atomic_load(&mt_current));
            fflush(stdout);
            _Exit(2);
        }
    }
    return NULL;
}

typedef void (*mt_fn)(void);

typedef struct
{
    const char *name;
    mt_fn fn;
} mt_case;

#define TEST_TIMEOUT_MS 20000

// Runs all cases; returns process exit code.
static inline int mt_run(const char *suite, const mt_case *cases, int count)
{
    atomic_store(&mt_deadline_ms, mt_now_ms() + TEST_TIMEOUT_MS);
    pthread_t wd;
    pthread_create(&wd, NULL, mt_watchdog, NULL);
    pthread_detach(wd);

    int failed_tests = 0;
    for (int i = 0; i < count; i++)
    {
        atomic_store(&mt_current, cases[i].name);
        atomic_store(&mt_deadline_ms, mt_now_ms() + TEST_TIMEOUT_MS);
        int before = atomic_load(&mt_failures);
        cases[i].fn();
        int bad = atomic_load(&mt_failures) != before;
        printf("%s %s\n", bad ? "FAIL" : "ok  ", cases[i].name);
        failed_tests += bad;
    }
    printf("%s: %d tests, %d failed, %d checks\n", suite, count, failed_tests,
           atomic_load(&mt_checks));
    return failed_tests ? 1 : 0;
}

#define TEST_COUNT(arr) ((int)(sizeof(arr) / sizeof((arr)[0])))

#endif
