//
// author: Tomáš Petržela
// e-mail: tomas.petrzela02 at upol.cz
//
// pipes for threads
//
// pipes for threads is an object that can be used for communication between
// threads.
//
// This object will be represented by a structure with name 'pipe' and this structure
// will have the following functions:
//
// struct pipe *pipe_create(uint size)
//      - creates an object of type pipe and create buffer of size 'size'. Buffer
//        will be used for communication from one pipe to another pipe. The buffer
//        can hold exactly 'size' bytes. Size 0 is not useful (nothing could ever
//        be written), so the buffer is then created with size 1.
//      - returns pointer to the created pipe.
//
// uint pipe_write(struct pipe *p, unsigned char *data, uint size)
//      - writes 'size' bytes from 'data' to the pipe 'p'. If there is not enough
//        space in the buffer, then the function will wait until there is enough
//        space in the buffer. Thread will be woken up when there is enough space
//        in the buffer.
//      - returns number of bytes written to the buffer. It is less than 'size'
//        only if the pipe was closed before all bytes were written (0 if the
//        pipe was already closed).
//
// uint pipe_read(struct pipe *p, unsigned char *data, uint size)
//      - reads at most 'size' bytes from the pipe 'p' to 'data'. If the buffer
//        is empty, then the function will wait until there is at least one byte
//        in the buffer. Thread will be woken up when data are written. Then the
//        function reads all currently available bytes (up to 'size') and does
//        not wait for more.
//      - returns number of bytes read from the buffer. It returns 0 only if
//        'size' is 0 or if the pipe is closed and the buffer is empty.
//
// void pipe_close(struct pipe *p)
//      - closes the pipe 'p'. All threads that are waiting in pipe_write or
//        pipe_read will be woken up. After close, pipe_write writes nothing and
//        returns the number of bytes written before the close (0 if there were
//        none). pipe_read still returns data that are in the buffer and returns
//        0 when the buffer is empty.
//
// void pipe_free(struct pipe *p)
//      - frees the pipe 'p' and all resources that are used by the pipe. No
//        thread may use the pipe during or after this call.
//

#define _POSIX_C_SOURCE 200112L

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>

#define error(message) \
    printf(message);   \
    exit(1);
#define errorf(message, ...)      \
    printf(message, __VA_ARGS__); \
    exit(1);

typedef unsigned int uint;

#pragma region QUEUE

#define Q_OK (0)
#define Q_EMPTY (1)
#define Q_FULL (2)

struct queue
{
    uint size;
    uint head;
    uint tail;
    void **values;
};

struct queue *queue_create(uint size)
{
    if (size == 0)
    {
        error("Unable to create queue of size 0\n");
    }

    struct queue *q = malloc(sizeof(struct queue));
    if (q == NULL)
    {
        errorf("Unable to allocate memory for new queue of size %u\n", size);
    }

    q->values = malloc(sizeof(void *) * size);
    if (q->values == NULL)
    {
        errorf("Unable to allocate memory for queue values of size %u\n", size);
    }
    q->size = size;
    q->head = 0;
    q->tail = 0;
    return q;
}

void queue_free(struct queue *q)
{
    free(q->values);
    free(q);
}

int queue_put(struct queue *q, void *value)
{
    unsigned head_next = (q->head + 1) % q->size;
    if (head_next == q->tail)
    {
        return Q_FULL;
    }

    q->values[q->head] = value;
    q->head = head_next;

    return Q_OK;
}

int queue_get(struct queue *q, void **result)
{
    if (q->head == q->tail)
    {
        return Q_EMPTY;
    }

    *result = q->values[q->tail];
    q->tail = (q->tail + 1) % q->size;

    return Q_OK;
}

bool queue_full(struct queue *q)
{
    return ((q->head + 1) % q->size) == q->tail;
}

bool queue_empty(struct queue *q)
{
    return q->head == q->tail;
}

#pragma endregion

// Pipe struct
// All fields are protected by 'lock'. The queue has one more slot than the
// pipe capacity, because a ring buffer can not distinguish full from empty
// without a free slot.
typedef struct pipe
{
    struct queue *queue;
    bool is_closed;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} pipe;

// Throw error if there is try to do acction NULL pipe
void pipe_null_error(struct pipe *p, char *acction)
{
    if (p == NULL)
    {
        errorf("Unable to %s to NULL pipe\n", acction);
    }
}

// Throw error if there is try to write to NULL pipe
void pipe_null_write_error(struct pipe *p)
{
    pipe_null_error(p, "write");
}

// Throw error if there is try to read from NULL pipe
void pipe_null_read_error(struct pipe *p)
{
    pipe_null_error(p, "read");
}

// Create and initialize pipe
struct pipe *pipe_create(uint size)
{
    // Pipe of size 0 could never be written to
    if (size == 0)
    {
        size = 1;
    }
    if (size == UINT32_MAX)
    {
        errorf("Unable to allocate memory for new pipe of size %u\n", size);
    }

    struct pipe *p = (struct pipe *)malloc(sizeof(struct pipe));
    if (p == NULL)
    {
        errorf("Unable to allocate memory for new pipe of size %u\n", size);
    }

    // One slot more, see struct pipe
    p->queue = queue_create(size + 1);
    p->is_closed = false;
    pthread_mutex_init(&p->lock, NULL);
    pthread_cond_init(&p->not_empty, NULL);
    pthread_cond_init(&p->not_full, NULL);

    return p;
}

// Write to pipe
uint pipe_write(struct pipe *p, unsigned char *data, uint size)
{
    pipe_null_write_error(p);

    // Lock the pipe for write
    pthread_mutex_lock(&p->lock);

    uint i = 0;
    while (i < size)
    {
        // Wait for space in buffer
        while (queue_full(p->queue) && !p->is_closed)
        {
            // Let readers take what is already written
            pthread_cond_broadcast(&p->not_empty);
            pthread_cond_wait(&p->not_full, &p->lock);
        }

        // If pipe is closed, then return number of bytes written to the buffer
        if (p->is_closed)
        {
            break;
        }

        queue_put(p->queue, (void *)(uintptr_t)data[i]);
        i++;
    }

    // Wake up readers
    if (i > 0)
    {
        pthread_cond_broadcast(&p->not_empty);
    }

    // Unlock the pipe
    pthread_mutex_unlock(&p->lock);
    return i;
}

// Read data from pipe
uint pipe_read(struct pipe *p, unsigned char *data, uint size)
{
    pipe_null_read_error(p);

    if (size == 0)
    {
        return 0;
    }

    // Lock the pipe for read
    pthread_mutex_lock(&p->lock);

    // Wait for data in buffer
    while (queue_empty(p->queue) && !p->is_closed)
    {
        pthread_cond_wait(&p->not_empty, &p->lock);
    }

    // Read all available data from buffer, but not more than size
    uint i = 0;
    while (i < size && !queue_empty(p->queue))
    {
        void *value;
        queue_get(p->queue, &value);
        data[i] = (unsigned char)(uintptr_t)value;
        i++;
    }

    // Wake up writers
    if (i > 0)
    {
        pthread_cond_broadcast(&p->not_full);
    }

    // Unlock the pipe
    pthread_mutex_unlock(&p->lock);
    return i;
}

// Close pipe
void pipe_close(struct pipe *p)
{
    pipe_null_error(p, "close");

    pthread_mutex_lock(&p->lock);
    p->is_closed = true;

    // Wake up all waiting threads
    pthread_cond_broadcast(&p->not_empty);
    pthread_cond_broadcast(&p->not_full);
    pthread_mutex_unlock(&p->lock);
}

// Completle unallocate memory for pipe
void pipe_free(struct pipe *p)
{
    pipe_null_error(p, "free");

    queue_free(p->queue);
    pthread_cond_destroy(&p->not_full);
    pthread_cond_destroy(&p->not_empty);
    pthread_mutex_destroy(&p->lock);
    free(p);
}

/*
void *test_fn_write(void *arg)
{
    struct pipe *p = (struct pipe *)arg;
    unsigned char data[20];
    for (unsigned char i = 0; i < 20; i++)
    {
        data[i] = i;
    }

    pipe_write(p, data, 20);

    pipe_close(p);
}

void *test_fn_read(void *arg)
{
    struct pipe *p = (struct pipe *)arg;

    unsigned char data[5];
    for (unsigned char i = 0; i < 5; i++)
    {
        data[i] = 0;
    }

    while (true)
    {
        pipe_read(p, data, 5);

        for (uint i = 0; i < 5; i++)
        {
            printf("%d ", data[i]);
        }
        printf("\n");
        for (uint i = 0; i < 5; i++)
        {
            data[i] = 0;
        }

        if (p->is_closed)
        {
            break;
        }
    }

    pipe_read(p, data, 5);

    for (uint i = 0; i < 5; i++)
    {
        printf("%d ", data[i]);
    }
    printf("\n");
}

int main()
{
    struct pipe *p = pipe_create(5);

    pthread_t t1, t2;

    pthread_create(&t1, NULL, test_fn_write, p);
    pthread_create(&t2, NULL, test_fn_read, p);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    pipe_free(p);

    return 0;
}
*/