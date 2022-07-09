/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _FILTER_EWMA_H_
#define _FILTER_EWMA_H_

#include <types.h>
#include <limits.h>

struct ewma_state {
    uint16_t last;
    uint16_t weight;
    bool ready;
};

#define EWMA_STATIC(WEIGHT) \
    {.weight = (WEIGHT)}

#define EWMA_INIT(WEIGHT) \
    (struct ewma_state) EWMA_STATIC(WEIGHT)

#define EWMA_STATE(NAME, WEIGHT) \
    struct ewma_state NAME = EWMA_INIT(WEIGHT)

extern uint16_t ewma_update(struct ewma_state *state, uint16_t value);
extern void ewma_clear(struct ewma_state *state);
extern void ewma_init(struct ewma_state *state, uint16_t weight);

#endif  /* _FILTER_EWMA_H_ */
