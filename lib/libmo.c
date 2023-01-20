/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "libmo"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <libmo.h>
#include <string.h>
#include <kernel.h>
#include <bsearch.h>
#include <stringhash.h>
#include <bitmap.h>
#include <printk.h>
#include <export.h>

#define LIBMO_SWAB(ctx, value) ( \
    (ctx)->swab ? swab32(value) : (value) \
)

static inline state load_swab(struct libmo_context *ctx)
{
    const struct libmo_header *head = ctx->data;

    switch (head->magic) {
        case LIBMO_MAGIC:
            ctx->swab = false;
            break;

        case LIBMO_MAGIC_SWAB:
            ctx->swab = true;
            break;

        default:
            pr_warn("%p: file is not in GNU mo format\n", ctx);
            return -EBADTYPE;
    }

    return -ENOERR;
}

static state load_context(struct libmo_context *ctx)
{
    const struct libmo_header *head = ctx->data;
    state retval;

    if (ctx->size < sizeof(*head)) {
        pr_warn("%p: header truncated\n", ctx);
        return -EFBIG;
    }

    if ((retval = load_swab(ctx)))
        return retval;

    ctx->major = LIBMO_MAJOR(LIBMO_SWAB(ctx, head->revision));
    ctx->minor = LIBMO_MINOR(LIBMO_SWAB(ctx, head->revision));

    if (ctx->major != 0 && ctx->major != 1) {
        pr_warn("%p: unknow revision\n", ctx);
        return -EPROTO;
    }

    ctx->index_num = LIBMO_SWAB(ctx, head->index_num);
    ctx->hash_size = LIBMO_SWAB(ctx, head->hash_size);

    ctx->orig_offset = LIBMO_SWAB(ctx, head->orig_offset);
    ctx->tran_offset = LIBMO_SWAB(ctx, head->tran_offset);
    ctx->hash_offset = LIBMO_SWAB(ctx, head->hash_offset);

    if (!align_check(ctx->orig_offset | ctx->tran_offset, BYTES_PER_U32)) {
        pr_warn("%p: unaligned offset\n", ctx);
        return -EADDRNOTAVAIL;
    }

    if (ctx->orig_offset + ctx->index_num * sizeof(struct libmo_sdesc) >= ctx->size ||
        ctx->tran_offset + ctx->index_num * sizeof(struct libmo_sdesc) >= ctx->size ||
        ctx->hash_offset + ctx->index_num * BYTES_PER_U32 >= ctx->size) {
        pr_warn("%p: offset truncated\n", ctx);
        return -EFBIG;
    }

    return -ENOERR;
}

static uint32_t get_uint32(const struct libmo_context *ctx, uintptr_t offset, uint32_t *valp)
{
    const uint32_t *desc = ctx->data + offset;

    if (offset + BYTES_PER_U32 > ctx->size) {
        pr_warn("%p: value truncated\n", ctx);
        return -EOVERFLOW;
    }

    *valp = LIBMO_SWAB(ctx, *desc);
    return -ENOERR;
}

static const char *get_string(const struct libmo_context *ctx, uintptr_t offset, size_t *lenp)
{
    uint32_t off, len;
    state retval;

    if ((retval = get_uint32(ctx, offset + offsetof(struct libmo_sdesc, offset), &off)) ||
        (retval = get_uint32(ctx, offset + offsetof(struct libmo_sdesc, length), &len)))
        return ERR_PTR(retval);

    if (off + len + 1 > ctx->size) {
        pr_warn("%p: string truncated\n", ctx);
        return ERR_PTR(-EOVERFLOW);
    }

    if (((char *)ctx->data)[off + len]) {
        pr_warn("%p: contains a not nul terminated string\n", ctx);
        return ERR_PTR(-EBADMSG);
    }

    if (lenp)
        *lenp = len;

    return ctx->data + off;
}

state libmo_load(struct libmo_context *ctx, const void *data, size_t size)
{
    *ctx = (struct libmo_context){data, size};
    return load_context(ctx);
}
EXPORT_SYMBOL(libmo_load);

state libmo_verify(const struct libmo_context *ctx)
{
    const char *msgid, *prev;
    unsigned int index;
    state retval;

    if (!ctx->data)
        return -EINVAL;

    /* Verify that the array of messages is sorted. */
    for (index = 0; index < ctx->index_num; ++index) {
        msgid = get_string(ctx, LIBMO_ORIG_OFFSET(ctx, index), NULL);
        if (IS_INVAL(msgid))
            return PTR_INVAL(msgid);

        if (index && strcmp(prev, msgid) > 0) {
            pr_warn("%p: messages array is not sorted\n", ctx);
            return -EINVAL;
        }

        prev = msgid;
    }

    /* Verify the hash table. */
    if (ctx->hash_size) {
        unsigned long *bitmap;

        /* Verify the hash table size. */
        if (ctx->hash_size < 2) {
            pr_warn("%p: hash table size is invalid\n", ctx);
            return -EINVAL;
        }

        /* Verify that the non-empty hash table entries contain the values. */
        bitmap = bitmap_zalloc(ctx->index_num, GFP_KERNEL);
        if (!bitmap)
            return -ENOMEM;

        for (index = 0; index < ctx->hash_size; ++index) {
            uint32_t entry;

            retval = get_uint32(ctx, LIBMO_HASH_OFFSET(ctx, index), &entry);
            if (retval) {
                bitmap_free(bitmap);
                return retval;
            }

            if (entry--) {
                if (entry < ctx->index_num && !bit_test_set(bitmap, entry))
                    continue;
                pr_warn("%p: hash table contains invalid entries\n", ctx);
                bitmap_free(bitmap);
                return -EINVAL;
            }
        }

        if (!bitmap_full(bitmap, ctx->index_num)) {
            pr_warn("%p: some messages are not present in hash table\n", ctx);
            bitmap_free(bitmap);
            return -EINVAL;
        }

        bitmap_free(bitmap);

        /* Verify that the hash table lookup algorithm finds the entry for each message. */
        for (index = 0; index < ctx->index_num; ++index) {
            uint32_t hashval, entry, incr;

            msgid = get_string(ctx, LIBMO_ORIG_OFFSET(ctx, index), NULL);
            hashval = pjwhash(msgid);
            entry = hashval % ctx->hash_size;
            incr = (hashval % (ctx->hash_size - 2)) + 1;

            for (;;) {
                uint32_t value;

                retval = get_uint32(ctx, LIBMO_HASH_OFFSET(ctx, entry), &value);
                if (retval)
                    return retval;

                if (!value) {
                    pr_warn("%p: some messages are at a wrong index in the hash table\n", ctx);
                    return -EINVAL;
                }

                if (value - 1 == index)
                    break;

                if (entry >= ctx->hash_size - incr)
                    entry -= ctx->hash_size - incr;
                else
                    entry += incr;
            }
        }
    }

    return -ENOERR;
}
EXPORT_SYMBOL(libmo_verify);

const char *libmo_lookup(const struct libmo_context *ctx, const char *orig, size_t origlen, size_t *tranlenp)
{
    const char *msgid, *msgstr;
    uint32_t hashval, entry, incr, value;
    size_t msglen, tranlen;
    state retval;

    if (!ctx->data)
        return ERR_PTR(-EINVAL);

    hashval = pjwhash(orig);
    entry = hashval % ctx->hash_size;
    incr = (hashval % (ctx->hash_size - 2)) + 1;

    for (;;) {
        retval = get_uint32(ctx, LIBMO_HASH_OFFSET(ctx, entry), &value);
        if (retval || !value)
            return ERR_PTR(retval);

        msgid = get_string(ctx, LIBMO_ORIG_OFFSET(ctx, value - 1), &msglen);
        if (IS_INVAL(msgid))
            return msgid;

        if ((!origlen || (origlen == msglen)) && !strcmp(orig, msgid)) {
            msgstr = get_string(ctx, LIBMO_TRAN_OFFSET(ctx, value - 1), &tranlen);
            if (IS_INVAL(msgstr))
                return msgstr;

            if (tranlenp)
                *tranlenp = tranlen;

            return msgstr;
        }

        if (entry >= ctx->hash_size - incr)
            entry -= ctx->hash_size - incr;
        else
            entry += incr;
    }
}
EXPORT_SYMBOL(libmo_lookup);
