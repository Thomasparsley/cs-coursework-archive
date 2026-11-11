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
//        will be used for communication from one pipe to another pipe.
//      - returns pointer to the created pipe.
//
// uint pipe_write(struct pipe *p, unsigned char *data, uint size)
//      - writes 'size' bytes from 'data' to the pipe 'p'. If there is not enough
//        space in the buffer, then the function will wait until there is enough
//        space in the buffer. Thread will be woken up when there is enough space
//        in the buffer.
//      - returns number of bytes written to the buffer.
//
// uint pipe_read(struct pipe *p, unsigned char *data, uint size)
//      - reads 'size' bytes from the pipe 'p' to 'data'. If there is not enough
//        data in the buffer, then the function will wait until there is enough
//        data in the buffer. Thread will be woken up when there is enough data
//        in the buffer.
//      - returns number of bytes read from the buffer.
//
// void pipe_close(struct pipe *p)
//      - closes the pipe 'p'. All threads that are waiting for data in the pipe
//        will be woken up and after close function pipe_write and pipe_read
//        will return 0. If there are some data in the buffer, then they will be
//        accessible for reading. If in some other thread is pipe_write or pipe_read
//        called, then the thread will be woken up and the function will return 0 or
//        return real number of bytes written to the buffer.
//
// void pipe_free(struct pipe *p)
//      - frees the pipe 'p' and all resources that are used by the pipe.
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
    struct queue *q = malloc(sizeof(struct queue));
    q->values = malloc(sizeof(void *) * size);
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
typedef struct pipe
{
    struct queue *queue;
    bool is_closed;
    pthread_spinlock_t *lock;
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
    struct pipe *p = (struct pipe *)malloc(sizeof(struct pipe));
    if (p == NULL)
    {
        errorf("Unable to allocate memory for new pipe of size %d\n", size);
    }

    struct queue *q = queue_create(size);

    p->lock = (pthread_spinlock_t *)malloc(sizeof(pthread_spinlock_t));
    if (p->lock == NULL)
    {
        errorf("Unable to allocate memory for new pipe lock of size %d\n", size);
    }
    pthread_spin_init(p->lock, 0);

    p->queue = q;
    p->is_closed = false;

    return p;
}

// Write to pipe
uint pipe_write(struct pipe *p, unsigned char *data, uint size)
{
    pipe_null_read_error(p);

    if (p->is_closed)
    {
        return 0;
    }

    // Lock the pipe for write
    pthread_spin_lock(p->lock);

    // Write to pipe
    for (uint i = 0; i < size; i++)
    {
        // Check if there is enough space in buffer
        if (queue_full(p->queue))
        {
            // Unlock the pipe
            pthread_spin_unlock(p->lock);

            // Wait for space in buffer
            while (queue_full(p->queue))
            {
            }

            // Lock the pipe for write
            pthread_spin_lock(p->lock);
        }

        unsigned char c = data[i];
        queue_put(p->queue, (void *)c);

        // If pipe is closed, then return number of bytes written to the buffer
        if (p->is_closed)
        {
            pthread_spin_unlock(p->lock);
            return i;
        }
    }

    // Unlock the pipe
    pthread_spin_unlock(p->lock);
    return size;
}

// Read data from pipe
uint pipe_read(struct pipe *p, unsigned char *data, uint size)
{
    pipe_null_read_error(p);

    // If pipe is closed and there is nothing to read, then return 0
    if (p->is_closed && queue_empty(p->queue))
    {
        return 0;
    }

    // Lock the pipe for read
    pthread_spin_lock(p->lock);

    // Read data from buffer
    for (uint i = 0; i < size; i++)
    {
        // If queue is empty, then return number of bytes read from the buffer
        if (queue_empty(p->queue))
        {
            // Unlock the pipe
            pthread_spin_unlock(p->lock);
            return i;
        }

        void *value;
        queue_get(p->queue, &value);
        data[i] = (unsigned char)value;
    }

    // Unlock the pipe
    pthread_spin_unlock(p->lock);
    return size;
}

// Close pipe
void pipe_close(struct pipe *p)
{
    pipe_null_error(p, "close");
    p->is_closed = true;
}

// Completle unallocate memory for pipe
void pipe_free(struct pipe *p)
{
    pipe_null_error(p, "free");

    queue_free(p->queue);
    pthread_spin_destroy(p->lock);
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