// Tests for petrzela-tomas-1-lin.c (thread pipe).
//
// The source is #included so that it is tested unmodified. mini_test.h does
// not include <unistd.h>, whose pipe() would clash with the typedef `pipe`.
#include "mini_test.h"

#include <stdint.h>
#include <string.h>

#include "../petrzela-tomas-1-lin.c"

#define SETTLE_MS 100 // time given to a thread to (not) get past a blocking call

// ---------------------------------------------------------------- helpers

typedef struct
{
    struct pipe *p;
    unsigned char *buf;
    uint size;
    uint result;
    atomic_int done;
} job;

static void *write_job(void *arg)
{
    job *j = arg;
    j->result = pipe_write(j->p, j->buf, j->size);
    atomic_store(&j->done, 1);
    return NULL;
}

static void *read_job(void *arg)
{
    job *j = arg;
    j->result = pipe_read(j->p, j->buf, j->size);
    atomic_store(&j->done, 1);
    return NULL;
}

static pthread_t start(void *(*fn)(void *), job *j)
{
    pthread_t t;
    atomic_store(&j->done, 0);
    j->result = 0;
    pthread_create(&t, NULL, fn, j);
    return t;
}

static int still_blocked(job *j)
{
    mt_sleep_ms(SETTLE_MS);
    return atomic_load(&j->done) == 0;
}

static unsigned char pattern(unsigned i)
{
    return (unsigned char)(i * 31u + 7u);
}

// ----------------------------------------------------- single-thread tests

static void test_create_free(void)
{
    struct pipe *p = pipe_create(8);
    CHECK(p != NULL);
    CHECK(!p->is_closed);
    pipe_free(p);
}

static void test_write_read_roundtrip(void)
{
    struct pipe *p = pipe_create(16);
    unsigned char in[5] = {1, 2, 3, 4, 5}, out[5] = {0};
    CHECK_EQ(pipe_write(p, in, 5), 5);
    CHECK_EQ(pipe_read(p, out, 5), 5);
    CHECK(memcmp(in, out, 5) == 0);
    pipe_free(p);
}

static void test_all_byte_values(void)
{
    struct pipe *p = pipe_create(256);
    unsigned char in[256], out[256];
    for (int i = 0; i < 256; i++)
        in[i] = (unsigned char)i;
    CHECK_EQ(pipe_write(p, in, 256), 256);
    CHECK_EQ(pipe_read(p, out, 256), 256);
    CHECK(memcmp(in, out, 256) == 0);
    pipe_free(p);
}

static void test_partial_reads(void)
{
    struct pipe *p = pipe_create(16);
    unsigned char in[5] = {10, 20, 30, 40, 50}, out[5] = {0};
    pipe_write(p, in, 5);
    CHECK_EQ(pipe_read(p, out, 3), 3);
    CHECK(memcmp(out, in, 3) == 0);
    CHECK_EQ(pipe_read(p, out, 2), 2);
    CHECK(out[0] == 40 && out[1] == 50);
    pipe_free(p);
}

static void test_read_returns_available_not_requested(void)
{
    // read must not wait for the full amount once at least one byte exists
    struct pipe *p = pipe_create(16);
    unsigned char in[2] = {9, 8}, out[10] = {0};
    pipe_write(p, in, 2);
    CHECK_EQ(pipe_read(p, out, 10), 2);
    CHECK(out[0] == 9 && out[1] == 8);
    pipe_free(p);
}

static void test_zero_size_operations(void)
{
    struct pipe *p = pipe_create(4);
    unsigned char b = 0;
    CHECK_EQ(pipe_write(p, &b, 0), 0);
    CHECK_EQ(pipe_read(p, &b, 0), 0); // would block forever if it waited
    pipe_free(p);
}

static void test_capacity_exact(void)
{
    // exactly `size` bytes fit without blocking (single thread: would hang
    // and trip the watchdog otherwise)
    for (uint cap = 1; cap <= 9; cap++)
    {
        struct pipe *p = pipe_create(cap);
        unsigned char in[16], out[16];
        for (int i = 0; i < 16; i++)
            in[i] = (unsigned char)(i + 1);
        CHECK_EQ(pipe_write(p, in, cap), cap);
        CHECK_EQ(pipe_read(p, out, 16), cap);
        CHECK(memcmp(in, out, cap) == 0);
        pipe_free(p);
    }
}

static void test_create_zero_has_capacity_one(void)
{
    struct pipe *p = pipe_create(0);
    CHECK(p != NULL);
    unsigned char in = 42, out = 0;
    CHECK_EQ(pipe_write(p, &in, 1), 1); // must not block
    CHECK_EQ(pipe_read(p, &out, 1), 1);
    CHECK_EQ(out, 42);

    // and the capacity is exactly 1: a second byte blocks
    CHECK_EQ(pipe_write(p, &in, 1), 1);
    unsigned char two[2] = {1, 2};
    job w = {p, two, 2, 0, 0};
    pthread_t t = start(write_job, &w);
    CHECK(still_blocked(&w));
    CHECK_EQ(pipe_read(p, &out, 1), 1); // frees the slot for the first byte
    CHECK(still_blocked(&w));           // second byte still has no room
    CHECK_EQ(pipe_read(p, &out, 1), 1);
    CHECK(mt_wait_flag(&w.done, 5000));
    pthread_join(t, NULL);
    CHECK_EQ(w.result, 2);
    CHECK_EQ(pipe_read(p, &out, 1), 1);
    CHECK_EQ(out, 2);
    pipe_free(p);
}

static void test_wraparound_fifo_order(void)
{
    struct pipe *p = pipe_create(4);
    unsigned counter = 0, expect = 0;
    int ok = 1;
    for (int round = 0; round < 1000; round++)
    {
        unsigned char in[3], out[3];
        for (int i = 0; i < 3; i++)
            in[i] = pattern(counter++);
        if (pipe_write(p, in, 3) != 3)
            ok = 0;
        if (pipe_read(p, out, 3) != 3)
            ok = 0;
        for (int i = 0; i < 3; i++)
            if (out[i] != pattern(expect++))
                ok = 0;
    }
    CHECK(ok);
    pipe_free(p);
}

// ----------------------------------------------------------- close semantics

static void test_read_closed_empty_returns_zero(void)
{
    struct pipe *p = pipe_create(4);
    unsigned char out[4];
    pipe_close(p);
    CHECK_EQ(pipe_read(p, out, 4), 0);
    CHECK_EQ(pipe_read(p, out, 4), 0); // stays 0
    pipe_free(p);
}

static void test_data_readable_after_close(void)
{
    struct pipe *p = pipe_create(8);
    unsigned char in[5] = {1, 2, 3, 4, 5}, out[8] = {0};
    pipe_write(p, in, 5);
    pipe_close(p);
    CHECK_EQ(pipe_read(p, out, 2), 2);
    CHECK(out[0] == 1 && out[1] == 2);
    CHECK_EQ(pipe_read(p, out, 8), 3);
    CHECK(out[0] == 3 && out[1] == 4 && out[2] == 5);
    CHECK_EQ(pipe_read(p, out, 8), 0);
    pipe_free(p);
}

static void test_write_to_closed_returns_zero(void)
{
    struct pipe *p = pipe_create(8);
    unsigned char in[3] = {1, 2, 3}, out[3];
    pipe_close(p);
    CHECK_EQ(pipe_write(p, in, 3), 0);
    CHECK_EQ(pipe_read(p, out, 3), 0); // nothing sneaked in
    pipe_free(p);
}

static void test_close_twice(void)
{
    struct pipe *p = pipe_create(2);
    pipe_close(p);
    pipe_close(p);
    CHECK(p->is_closed);
    pipe_free(p);
}

// ------------------------------------------------------------ blocking tests

static void test_blocked_reader_woken_by_write(void)
{
    struct pipe *p = pipe_create(8);
    unsigned char out[4] = {0}, in[2] = {7, 9};
    job r = {p, out, 4, 0, 0};
    pthread_t t = start(read_job, &r);
    CHECK(still_blocked(&r));
    CHECK_EQ(pipe_write(p, in, 2), 2);
    CHECK(mt_wait_flag(&r.done, 5000));
    pthread_join(t, NULL);
    CHECK_EQ(r.result, 2);
    CHECK(out[0] == 7 && out[1] == 9);
    pipe_free(p);
}

static void test_blocked_reader_woken_by_close(void)
{
    struct pipe *p = pipe_create(8);
    unsigned char out[4];
    job r = {p, out, 4, 99, 0};
    pthread_t t = start(read_job, &r);
    CHECK(still_blocked(&r));
    pipe_close(p);
    CHECK(mt_wait_flag(&r.done, 5000));
    pthread_join(t, NULL);
    CHECK_EQ(r.result, 0);
    pipe_free(p);
}

static void test_several_blocked_readers_woken_by_close(void)
{
    enum { N = 5 };
    struct pipe *p = pipe_create(8);
    unsigned char out[N][1];
    job r[N];
    pthread_t t[N];
    for (int i = 0; i < N; i++)
    {
        r[i] = (job){p, out[i], 1, 99, 0};
        t[i] = start(read_job, &r[i]);
    }
    mt_sleep_ms(SETTLE_MS);
    pipe_close(p);
    for (int i = 0; i < N; i++)
    {
        CHECK(mt_wait_flag(&r[i].done, 5000));
        pthread_join(t[i], NULL);
        CHECK_EQ(r[i].result, 0);
    }
    pipe_free(p);
}

static void test_blocked_writer_woken_by_drain(void)
{
    struct pipe *p = pipe_create(4);
    unsigned char in[10], out[10];
    for (int i = 0; i < 10; i++)
        in[i] = pattern((unsigned)i);
    job w = {p, in, 10, 0, 0};
    pthread_t t = start(write_job, &w);
    CHECK(still_blocked(&w)); // 4 fit, the rest waits

    // drain until the writer is done
    uint total = 0;
    while (total < 10)
    {
        uint n = pipe_read(p, out + total, 10 - total);
        CHECK(n > 0);
        if (n == 0)
            break;
        total += n;
    }
    CHECK(mt_wait_flag(&w.done, 5000));
    pthread_join(t, NULL);
    CHECK_EQ(w.result, 10);
    CHECK_EQ(total, 10);
    CHECK(memcmp(in, out, 10) == 0);
    pipe_free(p);
}

static void test_blocked_writer_woken_by_close(void)
{
    struct pipe *p = pipe_create(4);
    unsigned char in[10] = {0};
    job w = {p, in, 10, 0, 0};
    pthread_t t = start(write_job, &w);
    CHECK(still_blocked(&w));
    pipe_close(p);
    CHECK(mt_wait_flag(&w.done, 5000));
    pthread_join(t, NULL);
    CHECK_EQ(w.result, 4); // bytes written before the close

    // the 4 bytes that made it are still readable
    unsigned char out[10];
    CHECK_EQ(pipe_read(p, out, 10), 4);
    CHECK_EQ(pipe_read(p, out, 10), 0);
    pipe_free(p);
}

static void test_write_blocks_on_full_pipe(void)
{
    struct pipe *p = pipe_create(3);
    unsigned char in[4] = {1, 2, 3, 4}, out[4];
    job w = {p, in, 4, 0, 0};
    pthread_t t = start(write_job, &w);
    CHECK(still_blocked(&w));
    CHECK_EQ(pipe_read(p, out, 1), 1); // frees one slot
    CHECK(mt_wait_flag(&w.done, 5000));
    pthread_join(t, NULL);
    CHECK_EQ(w.result, 4);
    CHECK_EQ(pipe_read(p, out, 4), 3);
    CHECK(out[0] == 2 && out[1] == 3 && out[2] == 4);
    pipe_free(p);
}

// ------------------------------------------------------- concurrency stress

typedef struct
{
    struct pipe *p;
    unsigned id;
    unsigned count;
    uint chunk;
    uint64_t sum;
    uint64_t bytes;
} actor;

static void *stress_writer(void *arg)
{
    actor *a = arg;
    unsigned char buf[64];
    unsigned sent = 0;
    while (sent < a->count)
    {
        uint n = a->chunk;
        if (n > sizeof(buf))
            n = sizeof(buf);
        if (n > a->count - sent)
            n = a->count - sent;
        for (uint i = 0; i < n; i++)
            buf[i] = pattern(a->id * 1000003u + sent + i);
        uint w = pipe_write(a->p, buf, n);
        for (uint i = 0; i < w; i++)
            a->sum += buf[i];
        a->bytes += w;
        if (w != n)
            break;
        sent += n;
    }
    return NULL;
}

static void *stress_reader(void *arg)
{
    actor *a = arg;
    unsigned char buf[64];
    for (;;)
    {
        uint n = pipe_read(a->p, buf, a->chunk > sizeof(buf) ? (uint)sizeof(buf) : a->chunk);
        if (n == 0)
            break;
        for (uint i = 0; i < n; i++)
            a->sum += buf[i];
        a->bytes += n;
    }
    return NULL;
}

static void run_stress(uint capacity, int writers, int readers, unsigned per_writer)
{
    struct pipe *p = pipe_create(capacity);
    actor w[8], r[8];
    pthread_t wt[8], rt[8];

    for (int i = 0; i < readers; i++)
    {
        r[i] = (actor){p, (unsigned)i, 0, (uint)(1 + i * 5), 0, 0};
        pthread_create(&rt[i], NULL, stress_reader, &r[i]);
    }
    for (int i = 0; i < writers; i++)
    {
        w[i] = (actor){p, (unsigned)i, per_writer, (uint)(1 + i * 7), 0, 0};
        pthread_create(&wt[i], NULL, stress_writer, &w[i]);
    }
    for (int i = 0; i < writers; i++)
        pthread_join(wt[i], NULL);
    pipe_close(p); // readers drain what is left and then see 0
    for (int i = 0; i < readers; i++)
        pthread_join(rt[i], NULL);

    uint64_t sent_sum = 0, sent_bytes = 0, got_sum = 0, got_bytes = 0;
    for (int i = 0; i < writers; i++)
    {
        sent_sum += w[i].sum;
        sent_bytes += w[i].bytes;
    }
    for (int i = 0; i < readers; i++)
    {
        got_sum += r[i].sum;
        got_bytes += r[i].bytes;
    }
    CHECK_EQ(sent_bytes, (uint64_t)writers * per_writer);
    CHECK_EQ(got_bytes, sent_bytes);
    CHECK_EQ(got_sum, sent_sum);
    pipe_free(p);
}

static void test_stress_1w_1r_capacity_1(void) { run_stress(1, 1, 1, 20000); }
static void test_stress_1w_1r_capacity_64(void) { run_stress(64, 1, 1, 200000); }
static void test_stress_4w_3r_capacity_7(void) { run_stress(7, 4, 3, 30000); }
static void test_stress_4w_3r_capacity_4096(void) { run_stress(4096, 4, 3, 100000); }
static void test_stress_many_writers_one_reader(void) { run_stress(16, 8, 1, 10000); }
static void test_stress_one_writer_many_readers(void) { run_stress(16, 1, 8, 50000); }

static void test_single_stream_order_preserved(void)
{
    // one writer, one reader: the byte stream must arrive in order
    enum { TOTAL = 100000 };
    struct pipe *p = pipe_create(13);
    unsigned char *in = malloc(TOTAL), *out = malloc(TOTAL);
    for (unsigned i = 0; i < TOTAL; i++)
        in[i] = pattern(i);

    job w = {p, in, TOTAL, 0, 0};
    pthread_t t = start(write_job, &w);
    uint got = 0;
    while (got < TOTAL)
    {
        uint n = pipe_read(p, out + got, 17 < TOTAL - got ? 17 : TOTAL - got);
        if (n == 0)
            break;
        got += n;
    }
    pthread_join(t, NULL);
    CHECK_EQ(w.result, TOTAL);
    CHECK_EQ(got, TOTAL);
    CHECK(memcmp(in, out, TOTAL) == 0);
    free(in);
    free(out);
    pipe_free(p);
}

static void test_close_while_stress_running(void)
{
    // closing under load must wake everyone; counters stay consistent:
    // everything a writer reports as written is read by some reader.
    struct pipe *p = pipe_create(5);
    actor w[3], r[3];
    pthread_t wt[3], rt[3];
    for (int i = 0; i < 3; i++)
    {
        r[i] = (actor){p, (unsigned)i, 0, 3, 0, 0};
        pthread_create(&rt[i], NULL, stress_reader, &r[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        w[i] = (actor){p, (unsigned)i, 100000000u, 4, 0, 0};
        pthread_create(&wt[i], NULL, stress_writer, &w[i]);
    }
    mt_sleep_ms(100);
    pipe_close(p);
    uint64_t sent = 0, got = 0;
    for (int i = 0; i < 3; i++)
    {
        pthread_join(wt[i], NULL);
        sent += w[i].bytes;
    }
    for (int i = 0; i < 3; i++)
    {
        pthread_join(rt[i], NULL);
        got += r[i].bytes;
    }
    CHECK(sent > 0);
    CHECK_EQ(got, sent);
    pipe_free(p);
}

int main(void)
{
    static const mt_case cases[] = {
        {"create_free", test_create_free},
        {"write_read_roundtrip", test_write_read_roundtrip},
        {"all_byte_values", test_all_byte_values},
        {"partial_reads", test_partial_reads},
        {"read_returns_available_not_requested", test_read_returns_available_not_requested},
        {"zero_size_operations", test_zero_size_operations},
        {"capacity_exact", test_capacity_exact},
        {"create_zero_has_capacity_one", test_create_zero_has_capacity_one},
        {"wraparound_fifo_order", test_wraparound_fifo_order},
        {"read_closed_empty_returns_zero", test_read_closed_empty_returns_zero},
        {"data_readable_after_close", test_data_readable_after_close},
        {"write_to_closed_returns_zero", test_write_to_closed_returns_zero},
        {"close_twice", test_close_twice},
        {"blocked_reader_woken_by_write", test_blocked_reader_woken_by_write},
        {"blocked_reader_woken_by_close", test_blocked_reader_woken_by_close},
        {"several_blocked_readers_woken_by_close", test_several_blocked_readers_woken_by_close},
        {"blocked_writer_woken_by_drain", test_blocked_writer_woken_by_drain},
        {"blocked_writer_woken_by_close", test_blocked_writer_woken_by_close},
        {"write_blocks_on_full_pipe", test_write_blocks_on_full_pipe},
        {"stress_1w_1r_capacity_1", test_stress_1w_1r_capacity_1},
        {"stress_1w_1r_capacity_64", test_stress_1w_1r_capacity_64},
        {"stress_4w_3r_capacity_7", test_stress_4w_3r_capacity_7},
        {"stress_4w_3r_capacity_4096", test_stress_4w_3r_capacity_4096},
        {"stress_many_writers_one_reader", test_stress_many_writers_one_reader},
        {"stress_one_writer_many_readers", test_stress_one_writer_many_readers},
        {"single_stream_order_preserved", test_single_stream_order_preserved},
        {"close_while_stress_running", test_close_while_stress_running},
    };
    return mt_run("pipe", cases, TEST_COUNT(cases));
}
