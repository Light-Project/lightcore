/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <ctype.h>
#include <string.h>
#include <sched.h>
#include <ascii.h>
#include <kmalloc.h>
#include <driver/buzzer.h>

#define READLINE_BUFFER_DEF     64
#define READLINE_WORKSPACE_DEF  64
#define READLINE_CLIPBRD_DEF    64
#define READLINE_ALT_OFFSET     0x80

#define READLINE_BELL_PITCH     750
#define READLINE_BELL_DURATION	(CONFIG_SYSTICK_FREQ / 8)

unsigned int readline_read(struct readline_state *rstate, char *str, unsigned int len)
{
    return rstate->read(str, len, rstate->data);
}

void readline_write(struct readline_state *rstate, const char *str, unsigned int len)
{
    rstate->write(str, len, rstate->data);
}

static void readline_state_setup(struct readline_state *rstate)
{
    rstate->pos = 0;
    rstate->len = 0;
    rstate->curr = NULL;
    rstate->esc_state = READLINE_ESC_NORM;
}

static inline void readline_cursor_save(struct readline_state *rstate)
{
    readline_write(rstate, "\e[s", 3);
}

static inline void readline_cursor_restore(struct readline_state *rstate)
{
    readline_write(rstate, "\e[u", 3);
}

static bool readline_cursor_left(struct readline_state *rstate)
{
    if (rstate->pos) {
        readline_write(rstate, "\e[D", 3);
        --rstate->pos;
        return true;
    }

    return false;
}

static bool readline_cursor_right(struct readline_state *rstate)
{
    if (rstate->pos < rstate->len) {
        readline_write(rstate, "\e[C", 3);
        ++rstate->pos;
        return true;
    }

    return false;
}

static bool readline_cursor_offset(struct readline_state *rstate, unsigned int offset)
{
    bool retval = true;

    while (rstate->pos != offset && retval) {
        if (rstate->pos > offset)
            retval = readline_cursor_left(rstate);
        else if (rstate->pos < offset)
            retval = readline_cursor_right(rstate);
    }

    return retval;
}

static void readline_cursor_home(struct readline_state *rstate)
{
    readline_cursor_offset(rstate, 0);
}

static void readline_cursor_end(struct readline_state *rstate)
{
    readline_cursor_offset(rstate, rstate->len);
}

static void readline_fill(struct readline_state *rstate, unsigned int len)
{
    while (len--)
        readline_write(rstate, " ", 1);
}

static state readline_insert(struct readline_state *rstate, const char *str, unsigned int len)
{
    if (rstate->len + len >= rstate->bsize) {
        unsigned int nbsize = rstate->bsize;
        void *nblk;

        while (rstate->len + len >= nbsize)
            nbsize *= 2;

        nblk = krealloc(rstate->buff, nbsize, GFP_KERNEL);
        if (!nblk)
            return -ENOMEM;

        rstate->buff = nblk;
        rstate->bsize = nbsize;
    }

    memmove(rstate->buff + rstate->pos + len, rstate->buff + rstate->pos, rstate->len - rstate->pos + 1);
    memmove(rstate->buff + rstate->pos, str, len);
    rstate->pos += len;
    rstate->len += len;

    readline_write(rstate, &rstate->buff[rstate->pos - len], len);
    readline_cursor_save(rstate);
    readline_write(rstate, &rstate->buff[rstate->pos], rstate->len - rstate->pos);
    readline_cursor_restore(rstate);
    return -ENOERR;
}

static void readline_delete(struct readline_state *rstate, unsigned int len)
{
    min_adj(len, rstate->len - rstate->pos);

    memmove(rstate->buff + rstate->pos, rstate->buff + rstate->pos + len, rstate->len - rstate->pos + 1);
    rstate->len -= len;

    readline_cursor_save(rstate);
    readline_write(rstate, &rstate->buff[rstate->pos], rstate->len - rstate->pos);
    readline_fill(rstate, len);
    readline_cursor_restore(rstate);
}

static void readline_backspace(struct readline_state *rstate, unsigned int len)
{
    unsigned int pos = rstate->pos;

    min_adj(len, rstate->pos);
    readline_cursor_offset(rstate, pos - len);
    readline_delete(rstate, len);
}

static void readline_clear(struct readline_state *rstate)
{
    readline_state_setup(rstate);
    readline_write(rstate, "\e[2J\e[1;1H", 10);
    readline_write(rstate, rstate->prompt, rstate->plen);
}

static state readline_save_workspace(struct readline_state *rstate)
{
    if (rstate->len > rstate->worksize) {
        unsigned int nbsize = rstate->worksize;
        void *nblk;

        while (rstate->len > nbsize)
            nbsize *= 2;

        nblk = kmalloc(nbsize, GFP_KERNEL);
        if (!nblk)
            return -ENOMEM;

        kfree(rstate->workspace);
        rstate->workspace = nblk;
        rstate->worksize = nbsize;
    }

    rstate->worklen = rstate->len;
    memcpy((char *)rstate->workspace, rstate->buff, rstate->len);

    return -ENOERR;
}

static void readline_restory_workspace(struct readline_state *rstate)
{
    if (rstate->worklen)
        readline_insert(rstate, rstate->workspace, rstate->worklen);
}

static state readline_save_clipbrd(struct readline_state *rstate, unsigned int *clippos)
{
    unsigned int start, length;

    if (rstate->clipview) {
        start = min(rstate->clippos, rstate->pos);
        length = max(rstate->clippos, rstate->pos) - start;
    } else {
        start = 0;
        length = rstate->len;
    }

    if (clippos)
        *clippos = start;

    if (length > rstate->clipsize) {
        unsigned int nbsize = rstate->clipsize;
        void *nblk;

        while (length > nbsize)
            nbsize *= 2;

        nblk = kmalloc(nbsize, GFP_KERNEL);
        if (!nblk)
            return -ENOMEM;

        kfree(rstate->clipbrd);
        rstate->clipbrd = nblk;
        rstate->clipsize = nbsize;
    }

    rstate->clipview = false;
    rstate->cliplen = length;
    memcpy((char *)rstate->clipbrd, rstate->buff + start, length);

    return -ENOERR;
}

static void readline_restory_clipbrd(struct readline_state *rstate)
{
    if (rstate->cliplen)
        readline_insert(rstate, rstate->clipbrd, rstate->cliplen);
}

static state readline_history_add(struct readline_state *rstate, const char *cmd, unsigned int len)
{
    struct readline_history *history;

    if (rstate->worklen)
        rstate->worklen = 0;

    history = list_first_entry_or_null(&rstate->history, struct readline_history, list);
    if (history && history->len == len && !strncmp(history->cmd, cmd, len))
        return -ENOERR;

    history = kmalloc(sizeof(*history) + len, GFP_KERNEL);
    if (!history)
        return -ENOMEM;

    history->len = len;
    list_head_init(&history->list);

    memcpy(history->cmd, cmd, len);
    list_add(&rstate->history, &history->list);

    return -ENOERR;
}

static struct readline_history *readline_history_prev(struct readline_state *rstate, const char *cmd, unsigned int len, bool complete)
{
    struct readline_history *prev;
    state retval;

    if (!rstate->curr) {
        retval = readline_save_workspace(rstate);
        if (retval)
            return ERR_PTR(retval);
    }

    if (complete && rstate->worklen) {
        if (rstate->curr) for (prev = list_next_entry(rstate->curr, list);
                               !list_entry_check_head(prev, &rstate->history, list);
                               prev = list_next_entry(prev, list)) {
            if (!strncmp(rstate->workspace, prev->cmd, rstate->worklen))
                goto finish;

        } else for (prev = list_first_entry(&rstate->history, struct readline_history, list);
                    !list_entry_check_head(prev, &rstate->history, list);
                    prev = list_next_entry(prev, list)) {
            if (!strncmp(rstate->workspace, prev->cmd, rstate->worklen))
                goto finish;
        }
        return NULL;
    }

    else if (rstate->curr) {
        prev = list_next_entry(rstate->curr, list);
        if (list_entry_check_head(prev, &rstate->history, list))
            return NULL;
    } else {
        prev = list_first_entry_or_null(&rstate->history,
                    struct readline_history, list);
    }

finish:
    rstate->curr = prev;
    return prev;
}

static struct readline_history *readline_history_next(struct readline_state *rstate, bool complete)
{
    struct readline_history *next;

    if (!rstate->curr)
        return NULL;

    if (complete && rstate->worklen) {
        for (next = list_prev_entry(rstate->curr, list);
             !list_entry_check_head(next, &rstate->history, list);
             next = list_prev_entry(next, list)) {
            if (!strncmp(rstate->workspace, next->cmd, rstate->worklen))
                goto finish;
        }
        next = NULL;
        goto finish;
    }

    else {
        next = list_prev_entry(rstate->curr, list);
        if (list_entry_check_head(next, &rstate->history, list))
            next = NULL;
    }

finish:
    rstate->curr = next;
    return next;
}

static void readline_history_clear(struct readline_state *rstate)
{
    struct readline_history *history, *next;

    list_for_each_entry_safe(history, next, &rstate->history, list) {
        list_del(&history->list);
        kfree(history);
    }

    rstate->curr = NULL;
}

static state readline_complete(struct readline_state *rstate)
{
    struct kshell_command *cmd, *tmp;
    unsigned int line, count = 0;
    const char *start;
    state retval;

    LIST_HEAD(complete);
    start = strrnchr(rstate->buff, rstate->pos - 1, ' ');
    line = start ? ++start - rstate->buff : 0;

    if (line >= rstate->pos)
        return -ENOERR;

    spin_lock(&kshell_lock);
    list_for_each_entry(cmd, &kshell_list, list) {
        if ((!start && strncmp(cmd->name, rstate->buff, rstate->pos)) ||
            (start && strncmp(cmd->name, start, rstate->pos - line)))
            continue;
        count++;
        list_add(&complete, &cmd->complete);
    }
    spin_unlock(&kshell_lock);

    if (count == 0)
        return -ENOERR;

    cmd = list_first_entry(&complete, struct kshell_command, complete);
    if (count == 1) {
        readline_backspace(rstate, rstate->pos - line);
        retval = readline_insert(rstate, cmd->name, strlen(cmd->name));
        readline_insert(rstate, " ", 1);
        goto finish;
    }

    readline_save_workspace(rstate);
    readline_write(rstate, "\n", 1);

    while (count) {
        for (line = 4; count && line; --count, --line) {
            readline_write(rstate, cmd->name, strlen(cmd->name));
            readline_write(rstate, "\t", 1);
            cmd = list_next_entry(cmd, complete);
        }
        readline_write(rstate, "\n", 1);
    }

    readline_state_setup(rstate);
    readline_write(rstate, rstate->prompt, rstate->plen);
    readline_restory_workspace(rstate);

finish:
    list_for_each_entry_safe(cmd, tmp, &complete, complete)
        list_del(&cmd->complete);

    return retval;
}

static bool readline_handle(struct readline_state *state, char code)
{
    struct readline_history *history;
    unsigned int tmp;
    bool complete = false;

    if (state->keylock && code != ASCII_DC3)
        return false;

    switch ((unsigned char)code) {
        case ASCII_SOH: /* ^A : Cursor Home */
            readline_cursor_home(state);
            break;

        case ASCII_STX: /* ^B : Cursor Left */
            readline_cursor_left(state);
            break;

        case ASCII_ETX: /* ^C : Break Readline */
            state->len = state->pos = 0;
            state->curr = NULL;
            state->clippos = 0;
            return true;

        case ASCII_EOT: /* ^D : Delete */
            if (state->pos < state->len)
                readline_delete(state, 1);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_ENQ: /* ^E : Cursor End */
            readline_cursor_end(state);
            break;

        case ASCII_ACK: /* ^F : Cursor Right */
            readline_cursor_right(state);
            break;

        case ASCII_BEL: /* ^G : Beep Bell */
            buzzer_beep(READ_ONCE(default_buzzer),
                READLINE_BELL_PITCH, READLINE_BELL_DURATION);
            break;

        case ASCII_BS: /* ^H : Backspace */
            if (state->pos)
                readline_backspace(state, 1);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_HT: /* ^I : Complete */
            if (state->pos)
                readline_complete(state);
            break;

        case ASCII_LF: /* ^J : Line Feed */
            goto linefeed;

        case ASCII_VT: /* ^K : Clear After */
            if (state->len)
                readline_delete(state, state->len - state->pos);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_FF: /* ^L : Form Feed */
            readline_clear(state);
            state->clippos = 0;
            break;

        case ASCII_CR: /* ^M : Carriage Return */
        linefeed:
            if (state->len)
                readline_history_add(state, state->buff, state->len);
            state->clippos = 0;
            return true;

        case ASCII_SO: /* ^N : History Complete Next */
            complete = true;
            goto history_next;

        case ASCII_SI: /* ^O : Clipboard Select */
            state->clipview = true;
            state->clippos = state->pos;
            break;

        case ASCII_DLE: /* ^P : History Complete Prev */
            complete = true;
            goto history_prev;

        case ASCII_DC1: /* ^Q : History Clear */
            readline_history_clear(state);
            break;

        case ASCII_DC2: /* ^R : Clipboard Clear */
            state->cliplen = 0;
            break;

        case ASCII_DC3: /* ^S : Readline Lock */
            state->keylock ^= true;
            break;

        case ASCII_DC4: /* ^T : Repeat Execution */
            history = readline_history_prev(state, state->buff, state->len, complete);
            if (history) {
                readline_cursor_home(state);
                readline_delete(state, state->len);
                readline_insert(state, history->cmd, history->len);
                state->clippos = 0;
            }
            goto linefeed;

        case ASCII_NAK: /* ^U : Clear Before */
            if (state->pos)
                readline_backspace(state, state->pos);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_SYN: /* ^V : History Next */
        history_next:
            history = readline_history_next(state, complete);
            readline_cursor_home(state);
            readline_delete(state, state->len);
            if (history)
                readline_insert(state, history->cmd, history->len);
            else
                readline_restory_workspace(state);
            state->clippos = 0;
            break;

        case ASCII_ETB: /* ^W : History Prev */
        history_prev:
            history = readline_history_prev(state, state->buff, state->len, complete);
            if (history) {
                readline_cursor_home(state);
                readline_delete(state, state->len);
                readline_insert(state, history->cmd, history->len);
                state->clippos = 0;
            }
            break;

        case ASCII_CAN: /* ^X : Clipboard Cut */
            readline_save_clipbrd(state, &tmp);
            readline_cursor_offset(state, tmp);
            readline_delete(state, state->cliplen);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case ASCII_EM: /* ^Y : Clipboard Yank */
            readline_save_clipbrd(state, NULL);
            break;

        case ASCII_SUB: /* ^Z : Clipboard Paste */
            readline_restory_clipbrd(state);
            readline_save_workspace(state);
            state->curr = NULL;
            break;

        case READLINE_ALT_OFFSET + 'b': /* ^[b : Backspace Word */
            for (tmp = state->pos; tmp-- > 1;) {
                if (isalnum(state->buff[tmp]) &&
                    !isalnum(state->buff[tmp - 1])) {
                    readline_backspace(state, state->pos - tmp);
                    break;
                }
            }
            if (!tmp)
                readline_backspace(state, state->pos);
            break;

        case READLINE_ALT_OFFSET + 'd': /* ^[d : Delete Word */
            for (tmp = state->pos; ++tmp < state->len;) {
                if (isalnum(state->buff[tmp - 1]) &&
                    !isalnum(state->buff[tmp])) {
                    readline_delete(state, tmp - state->pos);
                    break;
                }
            }
            if (tmp == state->len)
                readline_delete(state, state->len - state->pos);
            break;

        case READLINE_ALT_OFFSET + 'l': /* ^[l : Cursor Left Word */
            for (tmp = state->pos; tmp-- > 1;) {
                if (isalnum(state->buff[tmp]) &&
                    !isalnum(state->buff[tmp - 1])) {
                    readline_cursor_offset(state, tmp);
                    break;
                }
            }
            if (!tmp)
                readline_cursor_home(state);
            break;

        case READLINE_ALT_OFFSET + 'r': /* ^[r : Cursor Right Word */
            for (tmp = state->pos; ++tmp < state->len;) {
                if (!isalnum(state->buff[tmp - 1]) &&
                    isalnum(state->buff[tmp])) {
                    readline_cursor_offset(state, tmp);
                    break;
                }
            }
            if (tmp == state->len)
                readline_cursor_end(state);
            break;

        default:
            if (isprint(code)) {
                readline_insert(state, &code, 1);
                readline_save_workspace(state);
                state->curr = NULL;
            }
    }

    return false;
}

static bool readline_getcode(struct readline_state *state, char *code)
{

    if (!readline_read(state, code, 1))
        return false;

    switch (state->esc_state) {
        case READLINE_ESC_NORM:
            switch (*code) {
                case ASCII_ESC: /* Escape */
                    state->esc_state = READLINE_ESC_ESC;
                    break;

                case ASCII_DEL: /* Backspace */
                    *code = ASCII_BS;
                    return true;

                default:
                    return true;
            }
            break;

        case READLINE_ESC_ESC:
            switch (*code) {
                case '[':
                    state->esc_state = READLINE_ESC_CSI;
                    state->esc_param = 0;
                    break;

                case 'O':
                    state->esc_state = READLINE_ESC_SS3;
                    state->esc_param = 0;
                    break;

                case 'a' ... 'z': /* Alt Alpha */
                    state->esc_state = READLINE_ESC_NORM;
                    *code += READLINE_ALT_OFFSET;
                    return true;

                case ASCII_DEL: /* Alt Backspace */
                    *code = READLINE_ALT_OFFSET + 'b';
                    return true;

                default:
                    state->esc_state = READLINE_ESC_NORM;
                    return true;
            }
            break;

        case READLINE_ESC_CSI:
            if (*code >= '0' && *code <= '9') {
                state->esc_param = state->esc_param * 10 + (*code - '0');
                return false;
            }

            if (*code == ';')
                break;

            state->esc_state = READLINE_ESC_NORM;
            switch (*code) {
                case 'A': /* Cursor Up */
                    if (!state->esc_param)
                        *code = ASCII_DLE;
                    else if (state->esc_param == 15) /* Ctrl */
                        *code = ASCII_ETB;
                    return true;

                case 'B': /* Cursor Down */
                    if (!state->esc_param)
                        *code = ASCII_SO;
                    else if (state->esc_param == 15) /* Ctrl */
                        *code = ASCII_SYN;
                    return true;

                case 'C': /* Cursor Right */
                    if (!state->esc_param)
                        *code = ASCII_ACK;
                    else if (state->esc_param == 15) /* Ctrl */
                        *code = READLINE_ALT_OFFSET + 'r';
                    return true;

                case 'D': /* Cursor Left */
                    if (!state->esc_param)
                        *code = ASCII_STX;
                    else if (state->esc_param == 15) /* Ctrl */
                        *code = READLINE_ALT_OFFSET + 'l';
                    return true;

                case 'E': /* Cursor Middle */
                    *code = ASCII_DC4;
                    return true;

                case 'F': /* Control End */
                    if (!state->esc_param)
                        *code = ASCII_ENQ;
                    else if (state->esc_param == 15) /* Ctrl */
                        *code = ASCII_VT;
                    return true;

                case 'H': /* Control Home */
                    if (!state->esc_param)
                        *code = ASCII_SOH;
                    else if (state->esc_param == 15) /* Ctrl */
                        *code = ASCII_NAK;
                    return true;

                case '~':
                    switch (state->esc_param) {
                        case 2: /* Control Insert */
                            *code = ASCII_SUB;
                            return true;

                        case 3: /* Control Delete */
                            *code = ASCII_EOT;
                            return true;

                        case 5: /* Control Page Up */
                            *code = ASCII_ETB;
                            return true;

                        case 6: /* Control Page Down */
                            *code = ASCII_SYN;
                            return true;

                        case 23: /* Alt Control Insert */
                            *code = ASCII_CAN;
                            return true;

                        case 25: /* Ctrl Control Insert */
                            *code = ASCII_EM;
                            return true;

                        case 27: /* Ctrl Alt Control Insert */
                            *code = ASCII_SI;
                            return true;

                        case 35: /* Ctrl Control Delete */
                            *code = READLINE_ALT_OFFSET + 'd';
                            return true;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
            break;

        case READLINE_ESC_SS3:
            state->esc_state = READLINE_ESC_NORM;
            switch (*code) {
                case 'F': /* ^E */
                    *code = ASCII_ENQ;
                    return true;

                case 'H': /* ^A */
                    *code = ASCII_SOH;
                    return true;

                default:
                    break;
            }
            break;

        default:
            state->esc_state = READLINE_ESC_NORM;
            break;
    }

    return false;
}

char *readline(struct readline_state *state, const char *prompt)
{
    char code;

    readline_state_setup(state);

    if (prompt) {
        state->prompt = prompt;
        state->plen = strlen(prompt);
        readline_write(state, state->prompt, state->plen);
    }

    for (;;) {
        if (!readline_getcode(state, &code)) {
            scheduler_yield();
        } else {
            if (readline_handle(state, code))
                break;
        }
    }

    readline_write(state, "\n", 1);
    state->buff[state->len] = '\0';

    return state->buff;
}

struct readline_state *readline_alloc(kshell_read_t read, kshell_write_t write, void *data)
{
    struct readline_state *state;

    state = kzalloc(sizeof(*state), GFP_KERNEL);
    if (!state)
        return NULL;

    state->read = read;
    state->write = write;
    state->data = data;

    state->bsize = READLINE_BUFFER_DEF;
    state->buff = kmalloc(state->bsize, GFP_KERNEL);
    if (!state->buff)
        return NULL;

    state->worksize = READLINE_WORKSPACE_DEF;
    state->workspace = kmalloc(state->worksize, GFP_KERNEL);
    if (!state->workspace)
        return NULL;

    state->clipsize = READLINE_CLIPBRD_DEF;
    state->clipbrd = kmalloc(state->clipsize, GFP_KERNEL);
    if (!state->clipbrd)
        return NULL;

    list_head_init(&state->history);
    return state;
}

void readline_free(struct readline_state *state)
{
    readline_history_clear(state);
    kfree(state->workspace);
    kfree(state->clipbrd);
    kfree(state->buff);
    kfree(state);
}
