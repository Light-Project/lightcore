/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include "parser.h"
#include "token.h"

static state kmcvm_main(struct kshell_context *ctx, int argc, char *argv[])
{
    Token *head;
    Node *node;
    mpool_t *pool;
    const char *token;

    token = kshell_getenv(ctx, "COMPUTE");

    pool = mpool_create(MPOOL_DEFAULT_SIZE, NULL);
    if (pool == NULL) {
        return -ENOMEM;
    }

    head = tokenize(token, pool);
    if (head == NULL) {
        mpool_destroy(pool);
        return -EINVAL;
    }

    //skip head
    node = parser(list_next_entry(head, list), pool);

    if (node == NULL) {
        mpool_destroy(pool);
        return -EINVAL;
    }

    kshell_printf(ctx, "%d\n", compute(node));

    mpool_destroy(pool);

    return -ENOERR;
}

static struct kshell_command kmcvm_cmd = {
    .name = "kmcvm",
    .desc = "kernal memory c lang compiler",
    .exec = kmcvm_main,
};

static state kmcvm_init(void)
{
    return kshell_register(&kmcvm_cmd);
}
kshell_initcall(kmcvm_init);
