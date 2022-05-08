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
    unsigned int form;
    unsigned int to;
    char code;
    int stack;
    bool cross;
    bool scross;
};

enum paser_state {
    KSHELL_STATE_TEXT       = 0,
    KSHELL_STATE_ESC        = 1,
    KSHELL_STATE_GRAMMAR    = 2,
    KSHELL_STATE_QUOTE      = 3,
    KSHELL_STATE_DQUOTE     = 4,
    KSHELL_STATE_VARIABLE   = 5,
    KSHELL_STATE_RETVAL     = 6,
    KSHELL_STATE_QVARIABLE  = 7,
    KSHELL_STATE_VARNAME    = 8,
    KSHELL_STATE_LVARNAME   = 9,
    KSHELL_STATE_QVARNAME   = 10,
    KSHELL_STATE_LQVARNAME  = 11,
};

static const struct paser_transition trans_table[] = {
    { KSHELL_STATE_TEXT,       KSHELL_STATE_ESC,        '\\',    0, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_QUOTE,      '\'',    0, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_GRAMMAR,     '{',  + 1, false,  true},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_DQUOTE,      '"',    0, false, false},
    { KSHELL_STATE_TEXT,       KSHELL_STATE_VARIABLE,    '$',    0, false, false},
    { KSHELL_STATE_ESC,        KSHELL_STATE_TEXT,          0,    0,  true, false},
    { KSHELL_STATE_GRAMMAR,    KSHELL_STATE_TEXT,        '}',  - 1, false,  true},
    { KSHELL_STATE_QUOTE,      KSHELL_STATE_TEXT,       '\'',    0, false, false},
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_TEXT,        '"',    0, false, false},
    { KSHELL_STATE_DQUOTE,     KSHELL_STATE_QVARIABLE,   '$',    0, false, false},
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_RETVAL,      '(',  + 1, false,  true},
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_VARNAME,       0,    0,  true, false},
    { KSHELL_STATE_VARIABLE,   KSHELL_STATE_LVARNAME,    '{',  + 1, false, false},
    { KSHELL_STATE_RETVAL,     KSHELL_STATE_TEXT,        ')',  - 1, false,  true},
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_QVARNAME,      0,    0,  true, false},
    { KSHELL_STATE_QVARIABLE,  KSHELL_STATE_LQVARNAME,   '{',  + 1, false, false},
    { KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ' ',    0,  true, false},
    { KSHELL_STATE_VARNAME,    KSHELL_STATE_TEXT,        ';',    0,  true, false},
    { KSHELL_STATE_LVARNAME,   KSHELL_STATE_TEXT,        '}',  - 1, false, false},
    { KSHELL_STATE_QVARNAME,   KSHELL_STATE_TEXT,        '"',    0, false, false},
    { KSHELL_STATE_QVARNAME,   KSHELL_STATE_DQUOTE,      ' ',    0,  true, false},
    { KSHELL_STATE_LQVARNAME,  KSHELL_STATE_DQUOTE,      '}',  - 1, false, false},
    { KSHELL_STATE_LQVARNAME,  KSHELL_STATE_DQUOTE,      ' ',    0,  true, false},
};

static inline bool is_retvalue(enum paser_state state)
{
    return state == KSHELL_STATE_RETVAL;
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

static enum paser_state parser_state(enum paser_state state, int *sstack, char code, char *result)
{
    const struct paser_transition *major, *minor = NULL;
    unsigned int count;
    int osstack = 1;
    bool serror = false;

    for (count = 0; count < ARRAY_SIZE(trans_table); ++count) {
        major = &trans_table[count];
        if (major->code == code) {
            if (major->form == state) {
                count = 0;
                break;
            } else if (major->to == state)
                minor = major;
        } else if (major->form == state && major->code == 0)
            minor = major;
    }

    if (count && minor)
        major = minor;
    else if (count)
        major = NULL;

    if (major && major->stack) {
        osstack = *sstack;
        *sstack += major->stack;
        if (*sstack < 0) {
            serror = true;
            *sstack = 0;
        }
    }

    if (!major || major->cross || serror)
        *result = code;
    else if (major->scross && *sstack && osstack)
        *result = code;
    else
        *result = '\0';

    return major && (!*sstack || !osstack) ? major->to : state;
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

state kshell_parser(const char *cmdline, const char **pos, int *argc, char ***argv)
{
    enum paser_state nstate, cstate = KSHELL_STATE_TEXT;
    unsigned int tpos = 0, tsize = PASER_TEXT_DEF;
    unsigned int vpos = 0, vsize = PASER_VARN_DEF;
    char *tbuff, code = 0, brack_buff[12];
    char *vbuff, *var;
    unsigned int count;
    int sstack = 0;
    state retval;

    *argc = 0;
    *pos = NULL;

    if (!*cmdline)
        return -ENOERR;

    tbuff = kmalloc(tsize + vsize, GFP_KERNEL);
    vbuff = tbuff + tsize;
    if (!tbuff)
        return -ENOMEM;

    for (cstate = KSHELL_STATE_TEXT; *cmdline; ++cmdline) {
        nstate = parser_state(cstate, &sstack, *cmdline, &code);

        if (is_variable(cstate) && !is_variable(nstate)) {
            vbuff[vpos] = '\0';
            vpos = 0;
            var = kshell_getenv(vbuff);
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
            retval = kshell_system(vbuff);
            count = scnprintf(brack_buff, sizeof(brack_buff), "%d", retval);
            PARSER_EXP_TPOS(tpos + count + 1)
            strcpy(tbuff + tpos, brack_buff);
            tpos += count;
        }

        if (is_variable(nstate) || is_retvalue(nstate)) {
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

        PARSER_EXP_APOS(tpos + 1, vpos + 1)
        cstate = nstate;
    }

    /* If the last one is a variable, parse it out */
    if (is_variable(cstate)) {
        vbuff[vpos] = '\0';
        var = kshell_getenv(vbuff);
        if (var) {
            count = strlen(var);
            PARSER_EXP_TPOS(tpos + count + 1)
            strcpy(tbuff + tpos, var);
            tpos += count;
        }
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
