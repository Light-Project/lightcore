/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <ascii.h>
#include <kmalloc.h>
#include <asm/proc.h>

static state readline_history_add(struct readline_state *state, const char *cmd, unsigned int len)
{
    struct readline_history *history;

    history = kmalloc(sizeof(*history) + len, GFP_KERNEL);
    if (!history)
        return -ENOMEM;

    history->len = len;
    memcpy(history->cmd, cmd, len);
    list_add(&state->history, &history->list);
    return -ENOERR;
}

static struct readline_history *readline_history_prev(struct readline_state *state)
{
    struct readline_history *prev;

    if (state->curr) {
        prev = list_next_entry(state->curr, list);
        if (list_entry_check_head(prev, &state->history, list))
            return NULL;
    } else {
        prev = list_first_entry_or_null(&state->history,
                    struct readline_history, list);
    }

    return (state->curr = prev);
}

static struct readline_history *readline_history_next(struct readline_state *state)
{
    struct readline_history *next;

    if (!state->curr)
        return NULL;

    next = list_prev_entry(state->curr, list);
    if (list_entry_check_head(next, &state->history, list))
        next = NULL;

    return (state->curr = next);
}

static void readline_state_setup(struct readline_state *state)
{
    state->pos = 0;
    state->len = 0;
    state->curr = NULL;
    state->esc_state = READLINE_ESC_NORM;
}

static __always_inline unsigned int
readline_read(struct readline_state *state, char *str, unsigned int len)
{
    return state->read(str, len, state->data);
}

static __always_inline void
readline_write(struct readline_state *state, const char *str, unsigned int len)
{
    state->write(str, len, state->data);
}

static inline void readline_cursor_save(struct readline_state *state)
{
    readline_write(state, "\e[s", 3);
}

static inline void readline_cursor_restore(struct readline_state *state)
{
    readline_write(state, "\e[u", 3);
}

static void readline_cursor_left(struct readline_state *state)
{
    if (state->pos) {
        readline_write(state, "\e[D", 3);
        --state->pos;
    }
}

static void readline_cursor_right(struct readline_state *state)
{
    if (state->pos < state->len) {
        readline_write(state, "\e[C", 3);
        ++state->pos;
    }
}

static void readline_cursor_home(struct readline_state *state)
{
    while (state->pos)
        readline_cursor_left(state);
}

static void readline_cursor_end(struct readline_state *state)
{
    while (state->pos < state->len)
        readline_cursor_right(state);
}

static void readline_fill(struct readline_state *state, unsigned int len)
{
    while (len--)
        readline_write(state, " ", 1);
}

static state readline_insert(struct readline_state *state, char *str, unsigned int len)
{
    void *nblk;

    if (state->len + len >= state->bsize) {
        state->bsize *= 2;
        nblk = krealloc(state->buff, state->bsize, GFP_KERNEL);
        if (!nblk)
            return -ENOMEM;
        state->buff = nblk;
    }

    memmove(state->buff + state->pos + len, state->buff + state->pos, state->len - state->pos + 1);
    memmove(state->buff + state->pos, str, len);
    state->pos += len;
    state->len += len;

    readline_write(state, &state->buff[state->pos - len], len);
    readline_cursor_save(state);
    readline_write(state, &state->buff[state->pos], state->len - state->pos);
    readline_cursor_restore(state);
    return -ENOERR;
}

static void readline_delete(struct readline_state *state, unsigned int len)
{
    len = min(len, state->len - state->pos);

    memmove(state->buff + state->pos, state->buff + state->pos + len, state->len - state->pos + 1);
    state->len -= len;

    readline_cursor_save(state);
    readline_write(state, &state->buff[state->pos], state->len - state->pos);
    readline_fill(state, len);
    readline_cursor_restore(state);
}

static void readline_clear(struct readline_state *state)
{
    readline_state_setup(state);
    readline_write(state, "\e[2J\e[1;1H", 10);
    readline_write(state, state->prompt, state->plen);
}

static bool readline_handle(struct readline_state *state, char code)
{
    struct readline_history *history;
    unsigned int tmp;

    switch (code) {
        case ASCII_NUL: /* Null Char */
            break;

        case ASCII_SOH: /* ^A : Start of Heading */
            readline_cursor_home(state);
            break;

        case ASCII_STX: /* ^B : Start of Text */
            readline_cursor_left(state);
            break;

        case ASCII_ETX: /* ^C : End of Text */
            state->len = state->pos = 0;
            return true;

        case ASCII_EOT: /* ^D : End of Transmission */
            if(state->pos < state->len)
                readline_delete(state, 1);
            break;

        case ASCII_ENQ: /* ^E : Enquiry */
            readline_cursor_end(state);
            break;

        case ASCII_ACK: /* ^F : Acknowledgment */
            readline_cursor_right(state);
            break;

        case ASCII_BEL: /* ^G : Bell */
            break;

        case ASCII_BS: /* ^H : Back Space */
            if (state->pos) {
                readline_cursor_left(state);
                readline_delete(state, 1);
            }
            break;

        case ASCII_HT: /* ^I : Horizontal TAB */
            break;

        case ASCII_VT: /* ^K : Vertical Tab */
            break;

        case ASCII_FF: /* ^L : Form Feed */
            readline_clear(state);
            break;

        case ASCII_LF: /* ^J : Line Feed */
        case ASCII_CR: /* ^M : Carriage Return */
            if (state->len)
                readline_history_add(state, state->buff, state->len);
            return true;

        case ASCII_SO: /* ^N : Shift Out / X-On */
            history = readline_history_next(state);
            readline_cursor_home(state);
            readline_delete(state, state->len);
            if (history)
                readline_insert(state, history->cmd, history->len);
            break;

        case ASCII_SI: /* ^O : Shift In / X-Off */
            break;

        case ASCII_DLE: /* ^P : Data Line Escape */
            history = readline_history_prev(state);
            if (history) {
                readline_cursor_home(state);
                readline_delete(state, state->len);
                readline_insert(state, history->cmd, history->len);
            }
            break;

        case ASCII_DC1: /* ^Q : Device Control 1 */
            break;

        case ASCII_DC2: /* ^R : Device Control 2 */
            break;

        case ASCII_DC3: /* ^S : Device Control 3 */
            break;

        case ASCII_DC4: /* ^T : Device Control 4 */
            break;

        case ASCII_NAK: /* ^U : Negative Acknowledgement */
            if (state->pos) {
                tmp = state->pos;
                readline_cursor_home(state);
                readline_delete(state, tmp);
            }
            break;

        case ASCII_SYN: /* ^V : Synchronous Idle */
            break;

        case ASCII_ETB: /* ^W : End of Transmit Block */
            break;

        case ASCII_CAN: /* ^X : Cancel */
            break;

        case ASCII_EM: /* ^Y : End of Medium */
            break;

        case ASCII_SUB: /* ^Z : Substitute */
            break;

        default:
            readline_insert(state, &code, 1);
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

                case ASCII_DEL: /* Back Space */
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

                default:
                    state->esc_state = READLINE_ESC_NORM;
                    return true;
            }
            break;

        case READLINE_ESC_CSI:
            if (*code >= '0' && *code <= '9') {
                state->esc_param = state->esc_param * 10 + (*code - '0');
                return false;
            } else {
                if (*code == ';')
                    break;

                state->esc_state = READLINE_ESC_NORM;
                switch (*code) {
                    case 'A': case 'F': /* ^P */
                        *code = 0x10;
                        return true;

                    case 'B': case 'E': /* ^N */
                        *code = 0x0e;
                        return true;

                    case 'C': /* ^F */
                        *code = 0x06;
                        return true;

                    case 'D': /* ^B */
                        *code = 0x02;
                        return true;

                    case '~':
                        switch (state->esc_param) {
                            case 1: /* ^A */
                                *code = 0x01;
                                return true;

                            case 3: /* ^H */
                                *code = 0x08;
                                return true;

                            case 4: /* ^E */
                                *code = 0x05;
                                return true;

                            case 5: /* ^P */
                                *code = 0x10;
                                return true;

                            case 6: /* ^N */
                                *code = 0x0e;
                                return true;

                            default:
                                break;
                        }
                        break;
                       
                    default:
                        break;
                }
            }
            break;

        case READLINE_ESC_SS3:
            state->esc_state = READLINE_ESC_NORM;
            switch (*code) {
                case 'F': /* ^E */
                    *code = 0x05;
                    return true;

                case 'H': /* ^A */
                    *code = 0x01;
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
            cpu_relax();
        } else {
            if (readline_handle(state, code))
                break;
        }
    }

    readline_write(state, "\n", 1);
    state->buff[state->len] = '\0';
    return state->buff;
}

struct readline_state *readline_alloc(readline_read_t read, readline_write_t write, void *data)
{
    struct readline_state *state;

    state = kzalloc(sizeof(*state), GFP_KERNEL);
    if (!state)
        return NULL;

    state->read = read;
    state->write = write;
    state->data = data;

    state->bsize = 512;
    state->buff = kmalloc(512, GFP_KERNEL);
    if (!state->buff)
        return NULL;

    list_head_init(&state->history);
    return state;
}

void readline_free(struct readline_state *state)
{
    struct readline_history *history, *next;

    list_for_each_entry_safe(history, next, &state->history, list) {
        list_del(&history->list);
        kfree(history);
    }

    kfree(state->buff);
    kfree(state);
}
