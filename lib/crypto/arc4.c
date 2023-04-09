/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <crypto/arc4.h>
#include <export.h>

static __always_inline void
__arc4_transform(struct arc4_context *ctx, uint8_t *buff,
                 const uint8_t *data, size_t size)
{
    uint8_t proa, prob;
    uint8_t tx, ty, tb;
    uint8_t vx, vy;

    if (!size)
        return;

    proa = ctx->proa;
    prob = ctx->prob;

	vx = ctx->box[proa];
	prob += vx;
	vy = ctx->box[prob];

    ctx->box[prob] = vx;
    vx += vy;
    ctx->box[proa] = vy;
    proa++;

    tx = ctx->box[proa];
    tb = prob + tx;
    ty = ctx->box[tb];
    *buff++ = *data++ ^ ctx->box[vx];

    while (--size) {
		prob = tb;
		vx = tx;
		vy = ty;

        ctx->box[prob] = vx;
        vx += vy;
        ctx->box[proa] = vy;
        proa++;

        tx = ctx->box[proa];
        tb = prob + tx;
        ty = ctx->box[tb];
        *buff++ = *data++ ^ ctx->box[vx];
    }

    ctx->proa = proa;
    ctx->prob = prob;
}

void arc4_transform(struct arc4_context *ctx, void *buff,
                    const void *data, size_t size)
{
    __arc4_transform(ctx, buff, data, size);
}
EXPORT_SYMBOL(arc4_transform);

void arc4_setkey(struct arc4_context *ctx, const uint8_t *key, unsigned int klen)
{
    unsigned int count;
    uint8_t value, loop;

    if (!klen)
        return;

    ctx->proa = 1;
    ctx->prob = 0;

    for (count = 0; count < ARC4_KEY_SIZE; ++count)
        ctx->box[count] = count;

    for (count = 0; count < ARC4_KEY_SIZE; ++count) {
        value = ctx->box[count];
        loop += key[count % klen] + value;
        ctx->box[count] = ctx->box[loop];
        ctx->box[loop] = value;
    }
}
EXPORT_SYMBOL(arc4_setkey);
