/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>

#define PASER_TEXT_DEF  64
#define PASER_VARN_DEF  32

struct paser_transition {
    int form, to;
    char code;
    bool cross;
};

enum paser_state {
    KSHELL_STATE_TEXT       = 0,
    KSHELL_STATE_ESC        = 1,
    KSHELL_STATE_QUOTE      = 2,
    KSHELL_STATE_DQUOTE     = 3,
    KSHELL_STATE_VARIABLE   = 4,
    KSHELL_STATE_BRACKETS   = 5,
    KSHELL_STATE_QVARIABLE  = 6,
    KSHELL_STATE_VARNAME    = 7,
    KSHELL_STATE_LVARNAME   = 8,
    KSHELL_STATE_QVARNAME   = 9,
    KSHELL_STATE_LQVARNAME  = 10,
};

static const struct paser_transition trans_table[] = {
    { KSHELL_STATE_TEXT,       KSHELL_STATE_ESC,        '\\',  false },
    { KSHELL_STATE_TEXT,       KSHELL_STATE_QUOTE,      '\'',  false },
    { KSHELL_STATE_TEXT,       KSHELL_STATE_DQUOTE,      '"',  false },
    { KSHELL_STATE_TEXT,       KSHELL_STATE_VARIABLE,    '$',  false },
    { KSHELL_STATE_ESC,        KSHELL_STATE_TEXT,          0,   true },
    { KSHELL_STATE_QUOTE,      KSHELL_STATE_TEXT,       '\'',  false },
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_TEXT,        '"',  false },
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_QVARIABLE,   '$',  false },
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_BRACKETS,    '(',  false },
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_VARNAME,       0,   true },
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_LVARNAME,    '{',  false },
    { KSHELL_STATE_BRACKETS,   KSHELL_STATE_TEXT,        ')',  false },
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_QVARNAME,      0,   true },
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_LQVARNAME,   '{',  false },
    { KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ' ',   true },
    { KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ';',   true },
    { KSHELL_STATE_LVARNAME,   KSHELL_STATE_TEXT,        '}',  false },
    { KSHELL_STATE_QVARNAME,   KSHELL_STATE_TEXT,        '"',  false },
    { KSHELL_STATE_QVARNAME,   KSHELL_STATE_DQUOTE,      ' ',   true },
    { KSHELL_STATE_LQVARNAME,  KSHELL_STATE_DQUOTE,      '}',  false },
    { KSHELL_STATE_LQVARNAME,  KSHELL_STATE_DQUOTE,      ' ',   true },
};

static inline bool is_brackets(enum paser_state state)
{
    return state == KSHELL_STATE_BRACKETS;
}

static inline bool is_variable(enum paser_state state)
{
    return state >= KSHELL_STATE_VARNAME;
}

static inline bool is_text(enum paser_state nstate, enum paser_state cstate)
{
    return nstate == KSHELL_STATE_TEXT && cstate != KSHELL_STATE_ESC;
}

static inline bool is_end(enum paser_state state)
{
    return state == KSHELL_STATE_TEXT || is_variable(state);
}

static enum paser_state parser_state(enum paser_state state, char code, char *result)
{
    const struct paser_transition *major, *minor = NULL;
    unsigned int count;

    for (count = 0; count < ARRAY_SIZE(trans_table); ++count) {
        major = &trans_table[count];
        if (major->form == state && major->code == code) {
            count = 0;
            break;
        } else if (major->form == state && major->code == 0)
            minor = major;
    }

    if (count && minor)
        major = minor;
    else if (count)
        major = NULL;

    if (!major || major->cross)
        *result = code;
    else
        *result = '\0';

    return major ? major->to : state;
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
        if (!var) {                                             \
            kfree(tbuff);                                       \
            return -ENOMEM;                                     \
        }                                                       \
        tbuff = nblock;                                         \
        vbuff = nblock + tsize;                                 \
    }                                                           \
}

state kshell_parser(const char *cmdline, const char **pos, int *argc, char ***argv)
{
    enum paser_state nstate, cstate = KSHELL_STATE_TEXT;
    unsigned int tpos = 0, tsize = PASER_TEXT_DEF;
    unsigned int vpos = 0, vsize = PASER_VARN_DEF;
    char *tbuff, code = 0, brack_buff[20];
    char *vbuff, *var;
    unsigned int count;
    state retval;

    *argc = 0;
    *pos = NULL;

    tbuff = kmalloc(tsize + vsize, GFP_KERNEL);
    vbuff = tbuff + tsize;
    if (!tbuff)
        return -ENOMEM;

    for (cstate = KSHELL_STATE_TEXT; *cmdline; ++cmdline) {
        nstate = parser_state(cstate, *cmdline, &code);

        if (is_variable(cstate) && !is_variable(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            var = kshell_getenv(vbuff);
            count = strlen(var);
            PARSER_EXPANSION(tpos + count + 1, 0)
            strcpy(tbuff + tpos, var);
            tpos += count;
        }

        if (is_brackets(cstate) && !is_brackets(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            retval = kshell_system(vbuff);
            count = scnprintf(brack_buff, sizeof(brack_buff), "%d", retval);
            PARSER_EXPANSION(tpos + count + 1, 0)
            strcpy(tbuff + tpos, brack_buff);
            tpos += count;
        }

        if (is_variable(nstate) || is_brackets(nstate)) {
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
            *pos = cmdline + 1;
            if (is_end(cstate))
                break;
        } else if (code) {
            tbuff[tpos++] = code;
        }

        PARSER_EXPANSION(tpos, vpos)
        cstate = nstate;
    }

    /* If the last one is a variable, parse it out */
    if (is_variable(cstate)) {
        vbuff[vpos] = '\0';
        var = kshell_getenv(vbuff);
        count = strlen(var);
        PARSER_EXPANSION(tpos + count + 1, 0)
        strcpy(tbuff + tpos, var);
        tpos += count;
    }

    /* if there is no end flag, end it */
    if (!*cmdline && code != ' ' && code != ';') {
        tbuff[tpos++] = '\0';
        ++(*argc);
    }

    /* allocate an argv area */
    count = (*argc + 1) * sizeof(**argv);
    *argv = kmalloc(count + tpos, GFP_KERNEL);
    if (!*argv) {
        kfree(tbuff);
        return -ENOMEM;
    }

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
}
