/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>

#define PASER_TEXT_DEF  64
#define PASER_VARN_DEF  32
#define PASER_PIPE_DEF  64

struct paser_transition {
    unsigned int form;
    unsigned int to;
    char code;
    int depth;
    int dstack;
    int sstack;
    bool cross;
    bool dcross;
    bool dtrans;
};

enum paser_state {
    KSHELL_STATE_NULL       = 0,
    KSHELL_STATE_TEXT       = 1,
    KSHELL_STATE_ESC        = 2,
    KSHELL_STATE_QUOTE      = 3,
    KSHELL_STATE_DQUOTE     = 4,
    KSHELL_STATE_GRAMMAR    = 5,

    KSHELL_STATE_FUNCNAME   = 6,
    KSHELL_STATE_ARRAYNAME  = 7,
    KSHELL_STATE_EXPBODY    = 8,
    KSHELL_STATE_EXPQUOTE   = 9,
    KSHELL_STATE_EXPINDEX   = 10,

    KSHELL_STATE_VARIABLE   = 11,
    KSHELL_STATE_QVARIABLE  = 12,
    KSHELL_STATE_EVARIABLE  = 13,

    KSHELL_STATE_OUTPUT     = 14,
    KSHELL_STATE_QOUTPUT    = 15,
    KSHELL_STATE_EOUTPUT    = 16,
    KSHELL_STATE_LOUTPUT    = 20,
    KSHELL_STATE_LQOUTPUT   = 21,
    KSHELL_STATE_LEOUTPUT   = 22,

    KSHELL_STATE_RETVAL     = 17,
    KSHELL_STATE_QRETVAL    = 18,
    KSHELL_STATE_ERETVAL    = 19,

    KSHELL_STATE_VARNAME    = 23,
    KSHELL_STATE_QVARNAME   = 24,
    KSHELL_STATE_EVARNAME   = 25,
    KSHELL_STATE_LVARNAME   = 26,
    KSHELL_STATE_LQVARNAME  = 27,
    KSHELL_STATE_LEVARNAME  = 28,
};

static const struct paser_transition transition_table[] = {
    { KSHELL_STATE_TEXT,       KSHELL_STATE_ESC,        '\\',    0,   0,   0, false, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_QUOTE,      '\'',    0,   0,   0, false, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_DQUOTE,      '"',    0,   0,   0, false, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_GRAMMAR,     '{',  + 1,   0,   0, false,  true, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_FUNCNAME,    '@',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_ARRAYNAME,   '#',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_ESC,        KSHELL_STATE_TEXT,       '\0',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_QUOTE,      KSHELL_STATE_TEXT,       '\'',    0,   0,   0, false, false, false},
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_TEXT,        '"',    0,   0,   0, false, false, false},
    { KSHELL_STATE_GRAMMAR,    KSHELL_STATE_TEXT,        '}',  - 1,   0,   0, false,  true, false},

    { KSHELL_STATE_FUNCNAME,   KSHELL_STATE_EXPBODY,     '{',  + 1,   0,   0,  true,  true, false},
    { KSHELL_STATE_FUNCNAME,   KSHELL_STATE_EXPQUOTE,    '(',  + 1,   0,   0,  true,  true, false},
    { KSHELL_STATE_FUNCNAME,   KSHELL_STATE_EXPINDEX,    '[',  + 1,   0,   0,  true, false, false},
    { KSHELL_STATE_ARRAYNAME,  KSHELL_STATE_EXPBODY,     '{',  + 1,   0,   0,  true,  true, false},
    { KSHELL_STATE_ARRAYNAME,  KSHELL_STATE_EXPQUOTE,    '(',  + 1,   0,   0,  true,  true, false},
    { KSHELL_STATE_ARRAYNAME,  KSHELL_STATE_EXPINDEX,    '[',  + 1,   0,   0,  true, false, false},
    { KSHELL_STATE_EXPBODY,    KSHELL_STATE_TEXT,        '}',  - 1,   0,   0,  true,  true, false},
    { KSHELL_STATE_EXPQUOTE,   KSHELL_STATE_TEXT,        ')',  - 1,   0,   0,  true,  true, false},
    { KSHELL_STATE_EXPINDEX,   KSHELL_STATE_TEXT,        ']',  - 1,   0,   0,  true, false, false},

    { KSHELL_STATE_TEXT,       KSHELL_STATE_VARIABLE,    '$',    0,   0,   0, false, false, false},
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_QVARIABLE,   '$',    0,   0,   0, false, false, false},
    { KSHELL_STATE_EXPQUOTE,   KSHELL_STATE_EVARIABLE,   '$',    0,   0, + 1, false, false,  true},
    { KSHELL_STATE_EXPINDEX,   KSHELL_STATE_EVARIABLE,   '$',    0,   0, + 1, false, false,  true},

    { KSHELL_STATE_TEXT,       KSHELL_STATE_OUTPUT,      '`',    0, + 1,   0, false, false, false},
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_QOUTPUT,     '`',    0, + 1,   0, false, false, false},
    { KSHELL_STATE_EXPQUOTE,   KSHELL_STATE_EOUTPUT,     '`',    0, + 1, + 1, false, false,  true},
    { KSHELL_STATE_EXPINDEX,   KSHELL_STATE_EOUTPUT,     '`',    0, + 1, + 1, false, false,  true},

    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_VARNAME,    '\0',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_RETVAL,      '(',  + 1,   0,   0, false,  true, false},
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_LOUTPUT,     '[',  + 1,   0,   0, false,  true, false},
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_LVARNAME,    '{',  + 1,   0,   0, false, false, false},
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_QVARNAME,   '\0',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_QRETVAL,     '(',  + 1,   0,   0, false,  true, false},
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_LQOUTPUT,    '[',  + 1,   0,   0, false,  true, false},
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_LQVARNAME,   '{',  + 1,   0,   0, false, false, false},
    { KSHELL_STATE_EVARIABLE,  KSHELL_STATE_EVARNAME,   '\0',    0,   0,   0,  true, false,  true},
    { KSHELL_STATE_EVARIABLE,  KSHELL_STATE_ERETVAL,     '(',  + 1, + 1,   0, false,  true, false},
    { KSHELL_STATE_EVARIABLE,  KSHELL_STATE_LEOUTPUT,    '[',  + 1, + 1,   0, false,  true, false},
    { KSHELL_STATE_EVARIABLE,  KSHELL_STATE_LEVARNAME,   '{',  + 1, + 1,   0, false, false, false},

    { KSHELL_STATE_OUTPUT,     KSHELL_STATE_TEXT,        '`',    0,   0,   0, false, false, false},
    { KSHELL_STATE_QOUTPUT,    KSHELL_STATE_DQUOTE,      '`',    0,   0,   0, false, false, false},
    { KSHELL_STATE_EOUTPUT,    KSHELL_STATE_NULL,        '`',    0,   0, - 1, false, false,  true},
    { KSHELL_STATE_LOUTPUT,    KSHELL_STATE_TEXT,        ']',  - 1,   0,   0, false,  true, false},
    { KSHELL_STATE_LQOUTPUT,   KSHELL_STATE_DQUOTE,      ']',  - 1,   0,   0, false,  true, false},
    { KSHELL_STATE_LEOUTPUT,   KSHELL_STATE_NULL,        ']',  - 1, - 1, - 1, false,  true, false},

    { KSHELL_STATE_RETVAL,     KSHELL_STATE_TEXT,        ')',  - 1,   0,   0, false,  true, false},
    { KSHELL_STATE_QRETVAL,    KSHELL_STATE_DQUOTE,      ')',  - 1,   0,   0, false,  true, false},
    { KSHELL_STATE_ERETVAL,    KSHELL_STATE_NULL,        ')',  - 1, - 1, - 1, false,  true, false},

    { KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ' ',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ';',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_LVARNAME,   KSHELL_STATE_TEXT,        '}',  - 1,   0,   0, false, false, false},
    { KSHELL_STATE_QVARNAME,   KSHELL_STATE_TEXT,        '"',    0,   0,   0, false, false, false},
    { KSHELL_STATE_QVARNAME,   KSHELL_STATE_DQUOTE,      ' ',    0,   0,   0,  true, false, false},
    { KSHELL_STATE_LQVARNAME,  KSHELL_STATE_DQUOTE,      '}',  - 1,   0,   0, false, false, false},
    { KSHELL_STATE_EVARNAME,   KSHELL_STATE_TEXT,        ')',    0,   0, - 1,  true, false,  true},
    { KSHELL_STATE_EVARNAME,   KSHELL_STATE_TEXT,        ']',    0,   0, - 1,  true, false,  true},
    { KSHELL_STATE_EVARNAME,   KSHELL_STATE_NULL,        ' ',    0,   0, - 1,  true, false,  true},
    { KSHELL_STATE_EVARNAME,   KSHELL_STATE_NULL,        ',',    0,   0, - 1,  true, false,  true},
    { KSHELL_STATE_LEVARNAME,  KSHELL_STATE_NULL,        '}',  - 1, - 1, - 1, false,  true, false},
};

static inline bool is_retvalue(enum paser_state state)
{
    return state >= KSHELL_STATE_RETVAL && state <= KSHELL_STATE_ERETVAL;
}

static inline bool is_output(enum paser_state state)
{
    return state >= KSHELL_STATE_OUTPUT && state <= KSHELL_STATE_LEOUTPUT;
}

static inline bool is_shortvar(enum paser_state state)
{
    return state >= KSHELL_STATE_VARNAME && state <= KSHELL_STATE_EVARNAME;
}

static inline bool is_variable(enum paser_state state)
{
    return state >= KSHELL_STATE_VARNAME && state <= KSHELL_STATE_LEVARNAME;
}

static inline bool is_savepos(enum paser_state state)
{
    return state >= KSHELL_STATE_VARIABLE && state <= KSHELL_STATE_EOUTPUT;
}

static inline bool is_text(enum paser_state nstate, enum paser_state cstate)
{
    return nstate == KSHELL_STATE_TEXT && cstate != KSHELL_STATE_ESC;
}

static enum paser_state parser_state(enum paser_state state, char code, char *result,
                                     int *dstack, unsigned int *dpos,
                                     int *sstack, unsigned int *spos)
{
    const struct paser_transition *major, *minor = NULL;
    int odstack = dstack[*dpos];
    bool serror = false;
    unsigned int count;

    for (count = 0; count < ARRAY_SIZE(transition_table); ++count) {
        major = &transition_table[count];
        if (major->code == code) {
            if (major->form == state) {
                count = 0;
                break;
            } else if (odstack && major->to == state)
                minor = major;
        } else if (!major->code && major->form == state)
            minor = major;
    }

    if (count && minor)
        major = minor;
    else if (count)
        major = NULL;

    if (major && major->depth) {
        if (major->dstack > 0) {
            *dpos += major->dstack;
            odstack = dstack[*dpos];
        }
        dstack[*dpos] += major->depth;
        if (dstack[*dpos] < 0) {
            serror = true;
            dstack[*dpos] = 0;
        }
    }

    if (!major || major->cross || serror)
        *result = code;
    else if (major->dcross && dstack[*dpos] && odstack)
        *result = code;
    else
        *result = '\0';

    if (major) {
        if (!dstack[*dpos] || !odstack || major->dtrans) {
            if (major->sstack > 0) {
                sstack[*spos] = state;
                *spos += major->sstack;
            } else if (major->sstack < 0) {
                *spos += major->sstack;
                state = sstack[*spos];
            }
            state = major->to ?: state;
        }
        if (!dstack[*dpos] && *dpos && major->dstack < 0)
            *dpos += major->dstack;
    }

    return state;
}

#define PARSER_EXPANSION(texp, vexp) {                          \
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
}

#define PARSER_EXP_TPOS(tpos) PARSER_EXPANSION((tpos) + 1, 0)
#define PARSER_EXP_APOS(tpos, vops) PARSER_EXPANSION((tpos) + 1, (vops) + 1)

static void pipeline_write(const char *str, unsigned int len, void *data)
{
    struct kshell_context *ctx = data;
    bool nrealloc = false;
    void *nblock;

    while (ctx->pipepos + len >= ctx->pipesize) {
        ctx->pipesize *= 2;
        nrealloc = true;
    }

    if (nrealloc) {
        nblock = krealloc(ctx->pipeline, ctx->pipesize, GFP_KERNEL);
        if (!nblock)
            return;
        ctx->pipeline = nblock;
    }

    memcpy(ctx->pipeline + ctx->pipepos, str, len);
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

state kshell_parser(struct kshell_context *ctx, const char **buffer,
                    const char **pcmdline, int *argc, char ***argv,
                    bool *constant)
{
    enum paser_state nstate, cstate = KSHELL_STATE_TEXT;
    unsigned int tpos = 0, tsize = PASER_TEXT_DEF;
    unsigned int vpos = 0, vsize = PASER_VARN_DEF;
    unsigned int dpos = 0, spos = 0, count;
    int dstack[8] = {}, sstack[8] = {};
    char *tbuff, code = 0, brack_buff[12];
    char *vbuff, *var, *savepos;
    const char *walk, *cmdline = *pcmdline;
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
        nstate = parser_state(cstate, *walk, &code, dstack, &dpos, sstack, &spos);

        if (is_variable(cstate) && !is_variable(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            var = kshell_getenv(ctx, vbuff);
            if (var) {
                count = strlen(var);
                PARSER_EXP_TPOS(tpos + count + 1)
                strcpy(tbuff + tpos, var);
                tpos += count;
            }
        }

        if (is_retvalue(cstate) && !is_retvalue(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            retval = kshell_system(ctx, vbuff);
            count = scnprintf(brack_buff, sizeof(brack_buff), "%d", retval);
            PARSER_EXP_TPOS(tpos + count + 1)
            strcpy(tbuff + tpos, brack_buff);
            tpos += count;
        }

        if (is_output(cstate) && !is_output(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            var = pseudo_pipeline(ctx, vbuff, &count);
            if (var) {
                unsigned int cmdlen;
                char *newblock;

                cmdlen = strlen(cmdline);
                newblock = kmalloc(cmdlen + count - (walk - savepos), GFP_KERNEL);
                if (!newblock)
                    goto errmem;

                strncpy(newblock, cmdline, savepos - cmdline);
                strncpy(newblock + (savepos - cmdline), var, count);
                strncpy(newblock + (savepos - cmdline) + count, walk + 1, cmdlen - (walk - cmdline));
                kfree(var);

                if (*constant)
                    *constant = false;
                else
                    kfree(*buffer);

                *buffer = newblock;
                walk = newblock + (savepos - cmdline) - 1;
                cmdline = newblock;
                cstate = nstate;
                continue;
            }
        }

        if (!is_savepos(cstate) && is_savepos(nstate))
            savepos = (char *)walk;

        if (is_variable(nstate) || is_retvalue(nstate) || is_output(nstate)) {
            if (code)
                vbuff[vpos++] = code;
        } else if (is_text(nstate, cstate) && code == ' ') {
            if (tpos != 0 && tbuff[tpos - 1]) {
                tbuff[tpos++] = '\0';
                ++(*argc);
            }
        } else if (is_text(nstate, cstate) && code == ';') {
            if (tpos != 0 && tbuff[tpos - 1]) {
                tbuff[tpos++] = '\0';
                ++(*argc);
            }
            *pcmdline = walk + 1;
            break;
        } else if (code) {
            tbuff[tpos++] = code;
        }

        PARSER_EXP_APOS(tpos + 1, vpos + 1)
        cstate = nstate;
    }

    /* If the last one is a variable, parse it out */
    if (is_shortvar(cstate)) {
        vbuff[vpos] = '\0';
        var = kshell_getenv(ctx, vbuff);
        if (var) {
            count = strlen(var);
            PARSER_EXP_TPOS(tpos + count + 1)
            strcpy(tbuff + tpos, var);
            tpos += count;
        }
    }

    /* if there is no end flag, end it */
    if (!*walk && code != ' ') {
        tbuff[tpos++] = '\0';
        ++(*argc);
    }

    /* allocate an argv area */
    count = (*argc + 1) * sizeof(**argv);
    *argv = kmalloc(count + tpos, GFP_KERNEL);
    if (!*argv)
        goto errmem;

    /* use unified memory to store parameters */
    var = (char *)*argv + count;
    memcpy(var, tbuff, tpos);

    /* generate argv pointer */
    for (count = 0; count < *argc; ++count) {
        (*argv)[count] = var;
        while (*var)
            ++var;
        var++;
    }

    (*argv)[count] = NULL;
    kfree(tbuff);

    return -ENOERR;

errmem:
    kfree(tbuff);
    return -ENOMEM;
}
