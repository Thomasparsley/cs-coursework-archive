// Tests for petrzela-tomas-3-lin.c (lalloc / lfree).
#include "mini_test.h"

#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "../petrzela-tomas-3-lin.c"

static void test_alloc_returns_non_null(void)
{
    pair *p = lalloc();
    CHECK(p != NULL);
    lfree(p);
}

static void test_fields_zero_initialised(void)
{
    pair *p = lalloc();
    CHECK(p != NULL);
    CHECK(p->ar == NULL);
    CHECK(p->dr == NULL);
    lfree(p);
}

static void test_zero_init_after_reuse(void)
{
    // Dirty a block, free it, allocate again; whatever block comes back must
    // be cleared again.
    for (int round = 0; round < 1000; round++)
    {
        pair *p = lalloc();
        CHECK(p != NULL);
        p->ar = (void *)(uintptr_t)0xdeadbeef;
        p->dr = (void *)(uintptr_t)0xcafebabe;
        lfree(p);

        pair *q = lalloc();
        CHECK(q != NULL);
        CHECK(q->ar == NULL);
        CHECK(q->dr == NULL);
        lfree(q);
    }
}

static void test_alignment(void)
{
    pair *p = lalloc();
    CHECK(p != NULL);
    CHECK(((uintptr_t)p % alignof(pair)) == 0);
    CHECK(((uintptr_t)p % alignof(max_align_t)) == 0);
    lfree(p);
}

static void test_pair_layout(void)
{
    CHECK(sizeof(pair) == 2 * sizeof(void *));
    CHECK(offsetof(pair, ar) == 0);
    CHECK(offsetof(pair, dr) == sizeof(void *));
}

static void test_fields_are_independent(void)
{
    pair *p = lalloc();
    int a = 1, d = 2;
    p->ar = &a;
    p->dr = &d;
    CHECK(p->ar == &a);
    CHECK(p->dr == &d);
    CHECK(*(int *)p->ar == 1);
    CHECK(*(int *)p->dr == 2);
    lfree(p);
}

static void test_many_alloc_then_free(void)
{
    enum { N = 100000 };
    pair **ptrs = malloc(N * sizeof(pair *));
    CHECK(ptrs != NULL);

    for (int i = 0; i < N; i++)
    {
        ptrs[i] = lalloc();
        CHECK(ptrs[i] != NULL);
        ptrs[i]->ar = (void *)(uintptr_t)(i + 1);
    }
    // all live blocks are distinct and intact
    int intact = 1;
    for (int i = 0; i < N; i++)
    {
        if (ptrs[i]->ar != (void *)(uintptr_t)(i + 1) || ptrs[i]->dr != NULL)
            intact = 0;
        if (i > 0 && ptrs[i] == ptrs[i - 1])
            intact = 0;
    }
    CHECK(intact);

    for (int i = 0; i < N; i++)
        lfree(ptrs[i]);
    free(ptrs);
}

static void test_linked_list(void)
{
    // Build a Lisp-style list (1 2 3 ... N) and walk it.
    enum { N = 5000 };
    pair *head = NULL;
    for (long i = N; i >= 1; i--)
    {
        pair *cell = lalloc();
        CHECK(cell != NULL);
        cell->ar = (void *)(intptr_t)i;
        cell->dr = head;
        head = cell;
    }

    long expected = 1;
    long sum = 0;
    for (pair *it = head; it != NULL; it = it->dr)
    {
        CHECK_EQ((long)(intptr_t)it->ar, expected);
        sum += (long)(intptr_t)it->ar;
        expected++;
    }
    CHECK_EQ(expected, N + 1);
    CHECK_EQ(sum, (long)N * (N + 1) / 2);

    while (head != NULL)
    {
        pair *next = head->dr;
        lfree(head);
        head = next;
    }
}

static void test_interleaved_alloc_free(void)
{
    enum { N = 256 };
    pair *slots[N];
    memset(slots, 0, sizeof(slots));
    unsigned seed = 12345;
    for (int step = 0; step < 100000; step++)
    {
        seed = seed * 1103515245u + 12345u;
        int i = (int)((seed >> 16) % N);
        if (slots[i] == NULL)
        {
            slots[i] = lalloc();
            CHECK(slots[i] != NULL);
            CHECK(slots[i]->ar == NULL && slots[i]->dr == NULL);
            slots[i]->ar = slots[i];
        }
        else
        {
            lfree(slots[i]);
            slots[i] = NULL;
        }
    }
    for (int i = 0; i < N; i++)
        lfree(slots[i]);
}

static void test_free_null(void)
{
    lfree(NULL);
    lfree(NULL);
    CHECK(1);
}

int main(void)
{
    static const mt_case cases[] = {
        {"alloc_returns_non_null", test_alloc_returns_non_null},
        {"fields_zero_initialised", test_fields_zero_initialised},
        {"zero_init_after_reuse", test_zero_init_after_reuse},
        {"alignment", test_alignment},
        {"pair_layout", test_pair_layout},
        {"fields_are_independent", test_fields_are_independent},
        {"many_alloc_then_free", test_many_alloc_then_free},
        {"linked_list", test_linked_list},
        {"interleaved_alloc_free", test_interleaved_alloc_free},
        {"free_null", test_free_null},
    };
    return mt_run("lalloc", cases, TEST_COUNT(cases));
}
