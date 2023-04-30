/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

static void pipeline_write(const char *str, unsigned int len, void *data)
{
    struct kshell_context *ctx = data;
    bool nrealloc = false;
    void *nblock;

    while (unlikely(ctx->pipepos + len >= ctx->pipesize)) {
        ctx->pipesize *= 2;
        nrealloc = true;
    }

    if (unlikely(nrealloc)) {
        nblock = krealloc(ctx->pipeline, ctx->pipesize, GFP_KERNEL);
        if (!nblock)
            return;
        ctx->pipeline = nblock;
    }

    strncpy(ctx->pipeline + ctx->pipepos, str, len);
    ctx->pipepos += len;
}

static char *pseudo_pipeline(struct kshell_context *ctx, const char *cmdline,
                             unsigned int *length)
{
    unsigned int opipesize, opipepos;
    char *opipeline, *buffer;
    kshell_write_t owrite;
    void *odata;

    buffer = kmalloc(PASER_PIPE_DEF, GFP_KERNEL);
    if (unlikely(!buffer))
        return NULL;

    opipesize = ctx->pipesize;
    opipepos = ctx->pipepos;
    opipeline = ctx->pipeline;
    owrite = ctx->write;
    odata = ctx->data;

    ctx->pipesize = PASER_PIPE_DEF;
    ctx->pipepos = 0;
    ctx->pipeline = buffer;
    ctx->write = pipeline_write;
    ctx->data = ctx;

    kshell_system(ctx, cmdline);
    buffer = ctx->pipeline;
    buffer[ctx->pipepos] = '\0';
    *length = ctx->pipepos;

    ctx->pipesize = opipesize;
    ctx->pipepos = opipepos;
    ctx->pipeline = opipeline;
    ctx->write = owrite;
    ctx->data = odata;

    return buffer;
}

static inline bool isspilt(char ch)
{
    return ch == ' ' || ch == '\t' || ch == '\n';
}

static int pipeline_parser(char *buff, unsigned int *length)
{
    unsigned int walk;
    int count;

    for (walk = 0; isspilt(buff[walk]); ++walk);
    memmove(buff, buff + walk, *length - walk);
    *length -= walk;

    if (!*length)
        return 0;

    for (count = walk = 0; walk < *length - 1; ++walk) {
        if (isspilt(buff[walk])) {
            if (!isspilt(buff[walk + 1]))
                count++;
            buff[walk] = '\0';
        }
    }

    if (isspilt(buff[walk]))
        buff[walk] = '\0';

    while (!buff[walk--])
        --*length;

    return count;
}
