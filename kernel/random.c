/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <random.h>
#include <crypto.h>
#include <percpu.h>
#include <driver/rng.h>
#include <export.h>

struct random_state {
    uint32_t state[16];
    spinlock_t lock;
};

struct batched_entropy {
    long entropy[CHACHA_BLOCK_SIZE / sizeof(long)];
    unsigned int index;
    spinlock_t lock;
};

static struct random_state main_random;
static DEFINE_PERCPU(struct batched_entropy, batched_entropys);

state random_generator_change(struct rng_device *rdev)
{
    return -ENOERR;
}

static void random_extract_node(struct random_state *ramdom, uint8_t buff[CHACHA_BLOCK_SIZE])
{
    irqflags_t irqflags;

    spin_lock_irqsave(&ramdom->lock, &irqflags);
    chacha20_block(&ramdom->state[0], buff);
    if (ramdom->state[12] == 0)
        ramdom->state[13]++;
    spin_unlock_irqrestore(&ramdom->lock, &irqflags);
}

static void random_extract(uint8_t buff[CHACHA_BLOCK_SIZE])
{
    random_extract_node(&main_random, buff);
}

long random_long(void)
{
    struct batched_entropy *batch;
    irqflags_t irqflags;
    long retval;

    batch = thiscpu_ptr(&batched_entropys);
    spin_lock_irqsave(&batch->lock, &irqflags);
    if (!batch->index % ARRAY_SIZE(batch->entropy)) {
        random_extract((uint8_t *)batch->entropy);
        batch->index = 0;
    }
    retval = batch->entropy[batch->index++];
    spin_unlock_irqrestore(&batch->lock, &irqflags);
    return retval;
}
EXPORT_SYMBOL(random_long);

static void random_initialize_node(struct random_state *ramdom)
{
    chacha_init_const(&ramdom->state[0]);
}

void random_init(void)
{
    random_initialize_node(&main_random);
}
