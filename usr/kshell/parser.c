/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>
#include <crash.h>

#define PASER_TEXT_DEF  64
#define PASER_VARG_DEF  32
#define PASER_PIPE_DEF  64

struct paser_transition {
    unsigned int form;
    unsigned int to;
    char code;
    int depth;
    bool cross;
};

enum paser_state {
    KSHELL_STATE_TEXT       = 0,
    KSHELL_STATE_ESC        = 1,
    KSHELL_STATE_QUOTE      = 2,
    KSHELL_STATE_DQUOTE     = 3,
    KSHELL_STATE_GRAMMAR    = 4,

    KSHELL_STATE_VARIABLE   = 5,
    KSHELL_STATE_QVARIABLE  = 6,
    KSHELL_STATE_EVARIABLE  = 7,

    KSHELL_STATE_OUTPUT     = 8,
    KSHELL_STATE_LOUTPUT    = 9,
    KSHELL_STATE_QOUTPUT    = 10,
    KSHELL_STATE_LQOUTPUT   = 11,

    KSHELL_STATE_RETVAL     = 12,
    KSHELL_STATE_QRETVAL    = 13,

    KSHELL_STATE_VARNAME    = 14,
    KSHELL_STATE_QVARNAME   = 15,
    KSHELL_STATE_LVARNAME   = 16,
    KSHELL_STATE_LQVARNAME  = 17,
};

static const struct paser_transition transition_table[] = {
    {KSHELL_STATE_TEXT,       KSHELL_STATE_ESC,        '\\',    0,  false},
    {KSHELL_STATE_TEXT,       KSHELL_STATE_QUOTE,      '\'',    0,  false},
    {KSHELL_STATE_TEXT,       KSHELL_STATE_DQUOTE,      '"',    0,  false},
    {KSHELL_STATE_TEXT,       KSHELL_STATE_GRAMMAR,     '{',  + 1,  false},
    {KSHELL_STATE_ESC,        KSHELL_STATE_TEXT,       '\0',    0,   true},
    {KSHELL_STATE_QUOTE,      KSHELL_STATE_TEXT,       '\'',    0,  false},
    {KSHELL_STATE_DQUOTE,     KSHELL_STATE_TEXT,        '"',    0,  false},
    {KSHELL_STATE_GRAMMAR,    KSHELL_STATE_TEXT,        '}',  - 1,  false},

    {KSHELL_STATE_TEXT,       KSHELL_STATE_VARIABLE,    '$',    0,  false},
    {KSHELL_STATE_DQUOTE,     KSHELL_STATE_QVARIABLE,   '$',    0,  false},

    {KSHELL_STATE_TEXT,       KSHELL_STATE_OUTPUT,      '`',    0,  false},
    {KSHELL_STATE_DQUOTE,     KSHELL_STATE_QOUTPUT,     '`',    0,  false},

    {KSHELL_STATE_VARIABLE,   KSHELL_STATE_VARNAME,    '\0',    0,   true},
    {KSHELL_STATE_VARIABLE,   KSHELL_STATE_RETVAL,      '(',  + 1,  false},
    {KSHELL_STATE_VARIABLE,   KSHELL_STATE_LOUTPUT,     '[',  + 1,  false},
    {KSHELL_STATE_VARIABLE,   KSHELL_STATE_LVARNAME,    '{',  + 1,  false},
    {KSHELL_STATE_QVARIABLE,  KSHELL_STATE_QVARNAME,   '\0',    0,   true},
    {KSHELL_STATE_QVARIABLE,  KSHELL_STATE_QRETVAL,     '(',  + 1,  false},
    {KSHELL_STATE_QVARIABLE,  KSHELL_STATE_LQOUTPUT,    '[',  + 1,  false},
    {KSHELL_STATE_QVARIABLE,  KSHELL_STATE_LQVARNAME,   '{',  + 1,  false},

    {KSHELL_STATE_OUTPUT,     KSHELL_STATE_TEXT,        '`',    0,  false},
    {KSHELL_STATE_QOUTPUT,    KSHELL_STATE_DQUOTE,      '`',    0,  false},
    {KSHELL_STATE_LOUTPUT,    KSHELL_STATE_TEXT,        ']',  - 1,  false},
    {KSHELL_STATE_LQOUTPUT,   KSHELL_STATE_DQUOTE,      ']',  - 1,  false},

    {KSHELL_STATE_RETVAL,     KSHELL_STATE_TEXT,        ')',  - 1,  false},
    {KSHELL_STATE_QRETVAL,    KSHELL_STATE_DQUOTE,      ')',  - 1,  false},

    {KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ' ',    0,   true},
    {KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ';',    0,   true},
    {KSHELL_STATE_LVARNAME,   KSHELL_STATE_TEXT,        '}',  - 1,  false},
    {KSHELL_STATE_QVARNAME,   KSHELL_STATE_TEXT,        '"',    0,  false},
    {KSHELL_STATE_QVARNAME,   KSHELL_STATE_DQUOTE,      ' ',    0,   true},
    {KSHELL_STATE_LQVARNAME,  KSHELL_STATE_DQUOTE,      '}',  - 1,  false},
};

static inline bool is_retvalue(enum paser_state state)
{
    return state >= KSHELL_STATE_RETVAL && state <= KSHELL_STATE_QRETVAL;
}

static inline bool is_output(enum paser_state state)
{
    return state >= KSHELL_STATE_OUTPUT && state <= KSHELL_STATE_LQOUTPUT;
}

static inline bool is_qoutput(enum paser_state state)
{
    return state >= KSHELL_STATE_QOUTPUT && state <= KSHELL_STATE_LQOUTPUT;
}

static inline bool is_shortvar(enum paser_state state)
{
    return state >= KSHELL_STATE_VARNAME && state <= KSHELL_STATE_QVARNAME;
}

static inline bool is_variable(enum paser_state state)
{
    return state >= KSHELL_STATE_VARNAME && state <= KSHELL_STATE_LQVARNAME;
}

static inline bool is_text(enum paser_state nstate, enum paser_state cstate)
{
    return nstate == KSHELL_STATE_TEXT && cstate != KSHELL_STATE_ESC;
}

static enum paser_state parser_state(enum paser_state state, char code, char *result, int *depth)
{
    const struct paser_transition *major, *minor = NULL;
    int olddepth = *depth;
    unsigned int count;

    for (count = 0; count < ARRAY_SIZE(transition_table); ++count) {
        major = &transition_table[count];
        if (major->code == code) {
            if (major->form == state) {
                count = 0;
                break;
            } else if (*depth && major->to == state)
                minor = major;
        } else if (!major->code && major->form == state)
            minor = major;
    }

    if (count && minor)
        major = minor;
    else if (count)
        major = NULL;

    if (major && major->depth) {
        *depth += major->depth;
        if (*depth < 0)
            *depth = 0;
    }

    if (!major || major->cross || (*depth && olddepth))
        *result = code;
    else
        *result = '\0';

    if (major && (!*depth || !olddepth))
        state = major->to;

    return state;
}

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
    if (!buffer)
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

#define PARSER_EXPANSION(texp, vexp) do {                       \
    void *nblock;                                               \
    int flags = 0;                                              \
                                                                \
    while (unlikely(((texp) >= tsize) && (flags |= 1)))         \
        tsize *= 2;                                             \
                                                                \
    while (unlikely(((vexp) >= vsize) && (flags |= 2)))         \
        vsize *= 2;                                             \
                                                                \
    if (unlikely(flags)) {                                      \
        nblock = krealloc(tbuff, tsize + vsize, GFP_KERNEL);    \
        if (!nblock) {                                          \
            kfree(tbuff);                                       \
            return -ENOMEM;                                     \
        }                                                       \
        tbuff = nblock;                                         \
        vbuff = nblock + tsize;                                 \
    }                                                           \
} while (0)

#define PARSER_EXP_BUFF(text, vext) PARSER_EXPANSION(tpos + (text) + 1, vpos + (vext) + 1)
#define PARSER_EXP_TBUF(text) PARSER_EXP_BUFF(text, 0)
#define PARSER_EXP_VBUF(vext) PARSER_EXP_BUFF(0, vext)

state kshell_parser(struct kshell_context *ctx, const char **pcmdline,
                    int *argc, char ***argv)
{
    enum paser_state nstate, cstate = KSHELL_STATE_TEXT;
    unsigned int tpos = 0, tsize = PASER_TEXT_DEF, count;
    unsigned int vpos = 0, vsize = PASER_VARG_DEF;
    const char *walk, *cmdline = *pcmdline;
    char *vbuff, brack_buff[12];
    char *tbuff, code;
    const char *var;
    int depth = 0;
    state retval;

    *argc = 0;
    *pcmdline = NULL;

    if (!*cmdline)
        return -ENOERR;

    tbuff = kmalloc(tsize + vsize, GFP_KERNEL);
    vbuff = tbuff + tsize;
    if (!tbuff)
        return -ENOMEM;

    for (walk = cmdline; *walk; ++walk) {
        nstate = parser_state(cstate, *walk, &code, &depth);

        if (is_variable(cstate) && !is_variable(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            var = kshell_symbol_get(ctx, vbuff);
            if (!var)
                var = kshell_getenv(ctx, vbuff);
            if (var) {
                count = strlen(var);
                PARSER_EXP_TBUF(count);
                strcpy(tbuff + tpos, var);
                tpos += count;
            }
        }

        if (is_retvalue(cstate) && !is_retvalue(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            retval = kshell_system(ctx, vbuff);
            count = itoad(retval, brack_buff);
            PARSER_EXP_TBUF(count);
            strcpy(tbuff + tpos, brack_buff);
            tpos += count;
        }

        if (is_output(cstate) && !is_output(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            var = pseudo_pipeline(ctx, vbuff, &count);
            if (var) {
                PARSER_EXP_TBUF(count);
                strcpy(tbuff + tpos, var);
                if (!is_qoutput(cstate))
                    (*argc) += pipeline_parser(tbuff + tpos, &count);
                tpos += count;
            }
        }

        if (is_variable(nstate) || is_retvalue(nstate) || is_output(nstate)) {
            if (code) {
                PARSER_EXP_VBUF(1);
                vbuff[vpos++] = code;
            }
        }

        else if (is_text(nstate, cstate) && code == ' ') {
            if (tpos != 0 && tbuff[tpos - 1]) {
                PARSER_EXP_TBUF(1);
                tbuff[tpos++] = '\0';
                ++*argc;
            }
        }

        else if (is_text(nstate, cstate) && code == ';') {
            if (tpos != 0 && tbuff[tpos - 1]) {
                PARSER_EXP_TBUF(1);
                tbuff[tpos++] = '\0';
                ++*argc;
            }
            *pcmdline = walk + 1;
            break;
        }

        else if (code) {
            PARSER_EXP_TBUF(1);
            tbuff[tpos++] = code;
        }

        cstate = nstate;
    }

    /* If the last one is a variable, parse it out */
    if (is_shortvar(cstate)) {
        vbuff[vpos] = '\0';
        var = kshell_symbol_get(ctx, vbuff);
        if (!var)
            var = kshell_getenv(ctx, vbuff);
        if (var) {
            count = strlen(var);
            PARSER_EXP_TBUF(count);
            strcpy(tbuff + tpos, var);
            tpos += count;
        }
    }

    /* If there is no end flag, end it */
    if (!*walk-- && tbuff[tpos - 1] && code != ' ') {
        tbuff[tpos++] = '\0';
        ++*argc;
    }

    /* Allocate an argv area */
    count = (*argc + 1) * sizeof(**argv);
    *argv = kmalloc(count + tpos, GFP_KERNEL);
    if (!*argv)
        goto errmem;

    /* Use unified memory to store parameters */
    vbuff = (char *)*argv + count;
    memcpy(vbuff, tbuff, tpos);

    /* Generate argv pointer */
    for (count = vpos = 0; vpos < tpos; ++vpos) {
        if (!vbuff[vpos])
            continue;
        (*argv)[count++] = vbuff + vpos;
        vpos += strlen(vbuff + vpos);
    }

    BUG_ON(count != *argc);
    (*argv)[count] = NULL;
    kfree(tbuff);

    return -ENOERR;

errmem:
    kfree(tbuff);
    return -ENOMEM;
}
