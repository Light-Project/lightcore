/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <minmax.h>
#include <log2.h>
#include <string.h>
#include <kmalloc.h>
#include <filter/median.h>
#include <export.h>

static long minheap_cmp(const struct heap_node *na, const struct heap_node *nb)
{
    struct median_node *ma = heap_to_median(na);
    struct median_node *mb = heap_to_median(nb);
    return ma->value < mb->value ? -1 : 1;
}

static long maxheap_cmp(const struct heap_node *na, const struct heap_node *nb)
{
    struct median_node *ma = heap_to_median(na);
    struct median_node *mb = heap_to_median(nb);
    return ma->value > mb->value ? -1 : 1;
}

int median_update(struct median_state *state, int value)
{
    int index = state->count & state->mask;
    struct median_node *node = state->nodes + index;
    struct heap_node *minroot, *maxroot;

    node->value = value;
    state->count++;

    /* Delete overlap node */
    if (!HEAP_EMPTY_NODE(&node->node)) {
        if (node->minmax)
            heap_delete(&state->minheap, &node->node, minheap_cmp);
        else
            heap_delete(&state->maxheap, &node->node, maxheap_cmp);
    }

    /* Insert node to median heap */
    maxroot = HEAP_ROOT_NODE(&state->maxheap);
    if (maxroot && heap_to_median(maxroot)->value < value) {
        heap_insert(&state->minheap, &node->node, minheap_cmp);
        node->minmax = true;
    } else {
        heap_insert(&state->maxheap, &node->node, maxheap_cmp);
        node->minmax = false;
    }

    /* Rebalance median heap */
    if (HEAP_NODE_COUNT(&state->maxheap) <
        HEAP_NODE_COUNT(&state->minheap)) {
        minroot = HEAP_ROOT_NODE(&state->minheap);
        heap_delete(&state->minheap, minroot, minheap_cmp);
        heap_insert(&state->maxheap, minroot, maxheap_cmp);
        heap_to_median(minroot)->minmax = false;
    } else if (HEAP_NODE_COUNT(&state->maxheap) -
               HEAP_NODE_COUNT(&state->minheap) > 1) {
        maxroot = HEAP_ROOT_NODE(&state->maxheap);
        heap_delete(&state->maxheap, maxroot, maxheap_cmp);
        heap_insert(&state->minheap, maxroot, minheap_cmp);
        heap_to_median(maxroot)->minmax = true;
    }

    maxroot = HEAP_ROOT_NODE(&state->maxheap);
    return heap_to_median(maxroot)->value;
}
EXPORT_SYMBOL(median_update);

void median_clear(struct median_state *state)
{
    state->count = 0;
}
EXPORT_SYMBOL(median_clear);

struct median_state *median_alloc(int length, gfp_t flags)
{
    struct median_state *state;
    int count;

    length = roundup_power_of_2(length);
    if (length < 2)
        return NULL;

    state = kzalloc(sizeof(*state) + sizeof(*state->nodes) * length, flags);
    if (unlikely(!state))
        return NULL;

    for (count = 0; count < length; ++count)
        HEAP_CLEAR_NODE(&state->nodes[count].node);

    state->mask = length - 1;
    return state;
}
EXPORT_SYMBOL(median_alloc);

void median_free(struct median_state *state)
{
    kfree(state);
}
EXPORT_SYMBOL(median_free);

