#ifndef _HLIST_H_
#define _HLIST_H_

#include <stddef.h>
#include <types.h>

typedef struct hlist_node{
    struct hlist_node *next, **pprev;
} hlist_node_t;

typedef struct hlist_head{
    hlist_node_t *first;
} hlist_head_t;

#define HLIST_HEAD(name) \
        hlist_head_t name = {NULL};


#endif
