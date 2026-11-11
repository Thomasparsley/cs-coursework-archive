//
// author: Tomáš Petržela
// e-mail: tomas.petrzela02 at upol.cz
//
// Dotted pair allocator for Lisp
//
// Dotted pair is a structure that is used for representing lists in Lisp. This
// structure will be represented by a structure with name 'pair'. The pair hold two
// pointers to any other objects. The first pointer is called 'ar' and the second
// pointer is called 'dr'. The pair will be represented by the following structure:
//
// struct pair {
//     void *ar;
//     void *dr;
// };
//
// pair *lalloc()
//      - allocates a new pair and returns pointer to the allocated pair.
//      - returns pointer to the allocated pair. If memory allocation fails, then
//        the function will return NULL.
//
// void lfree(pair *p)
//      - frees the pair 'p' and all resources that are used by the pair.
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct pair
{
    void *ar;
    void *dr;
} pair;

pair *lalloc()
{
    pair *p = malloc(sizeof(pair));
    if (p == NULL)
    {
        return NULL;
    }

    p->ar = NULL;
    p->dr = NULL;

    return p;
}

void lfree(pair *p)
{
    free(p);
}
