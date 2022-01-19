/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <string.h>
#include <ctype.h>
#include <ascii.h>
#include <driver/vt.h>

static void vt_state_bs(struct vt_context *context)
{
    if (context->cur.xpos) {
        context->cur.xpos--;
    }
}

static void vt_state_cr(struct vt_context *context)
{
}

static void vt_state_lf(struct vt_context *context)
{
}

static void vt_state_del(struct vt_context *context)
{
}

static void vt_state_goto(struct vt_context *context, unsigned int xpos, unsigned int ypos)
{


}

static void vt_state_cur_save(struct vt_context *context)
{


}

static void vt_state_cur_restore(struct vt_context *context)
{


}

static void vt_state_csi_J(struct vt_context *context, unsigned var)
{

    switch (var) {
        case 0:
            break;

        case 1:
            break;

        case 3:
            fallthrough;

        case 2:
            break;

        default:
            return;
    }

}

static void vt_state_csi_K(struct vt_context *context, unsigned var)
{

}

static void vt_state_csi_L(struct vt_context *context, unsigned var)
{

}

static void vt_state_csi_M(struct vt_context *context, unsigned var)
{

}

static void vt_state_csi_P(struct vt_context *context, unsigned var)
{

}

static void vt_state_csi_X(struct vt_context *context, unsigned var)
{

}

static void vt_state_csi_at(struct vt_context *context, unsigned var)
{

}

static void vt_state_csi_m(struct vt_context *context, unsigned var)
{

}

static void vt_state_handle(struct vt_context *context, char code)
{
    switch (code) {
        case ASCII_NUL: /* Null Char */
            return;

        case ASCII_SOH: /* ^A : Start of Heading */
            break;

        case ASCII_STX: /* ^B : Start of Text */
            break;

        case ASCII_ETX: /* ^C : End of Text */
            break;

        case ASCII_EOT: /* ^D : End of Transmission */
            break;

        case ASCII_ENQ: /* ^E : Enquiry */
            break;

        case ASCII_ACK: /* ^F : Acknowledgment */
            break;

        case ASCII_BEL: /* ^G : Bell */
            return;

        case ASCII_BS: /* ^H : Back Space */
            vt_state_bs(context);
            return;

        case ASCII_HT: /* ^I : Horizontal TAB */
            return;

        case ASCII_LF: /* ^J : Line Feed */
        case ASCII_VT: /* ^K : Vertical Tab */
        case ASCII_FF: /* ^L : Form Feed */
            fallthrough;

        case ASCII_CR: /* ^M : Carriage Return */
            vt_state_cr(context);
            return;

        case ASCII_SO: /* ^N : Shift Out / X-On */
            return;

        case ASCII_SI: /* ^O : Shift In / X-Off */
            return;

        case ASCII_DLE: /* ^P : Data Line Escape */
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
            break;

        case ASCII_SYN: /* ^V : Synchronous Idle */
            break;

        case ASCII_ETB: /* ^W : End of Transmit Block */
            break;

        case ASCII_EM: /* ^Y : End of Medium */
            return;

        case ASCII_CAN: /* ^X : Cancel */
        case ASCII_SUB: /* ^Z : Substitute */
            context->state = VT_STATE_SQUARE;
            return;

        case ASCII_ESC: /* Escape */
            context->state = VT_STATE_ESC;
            return;

        case ASCII_FS: /* File Separator */
            break;

        case ASCII_GS: /* Group Separator */
            break;

        case ASCII_RS: /* Record Separator */
            break;

        case ASCII_US: /* Unit Separator */
            break;

        case ASCII_DEL:
            vt_state_del(context);
            return;

        default:
            break;
    }

    switch (context->state) {
        case VT_STATE_ESC:
            switch (code) {
                case '[':
                    context->state = VT_STATE_SQUARE;
                    return;

                case ']':
                    context->state = VT_STATE_NONSTD;
                    return;

                case '_':
                    context->state = VT_STATE_APC;
                    return;

                case '^':
                    context->state = VT_STATE_PM;
                    return;

                case '%':
                    context->state = VT_STATE_PERCENT;
                    return;

                case 'E':
                    vt_state_cr(context);
                    vt_state_lf(context);
                    return;

                case 'M':
                    return;

                case 'D':
                    return;

                case 'H':
                    return;

                case 'P':
                    context->state = VT_STATE_DCS;
                    return;

                case 'Z':
                    return;

                case '7':
                    return;

                case '8':
                    return;

                case '(':
                    context->state = VT_STATE_SETG0;
                    return;

                case ')':
                    context->state = VT_STATE_SETG1;
                    return;

                case '#':
                    context->state = VT_STATE_HASH;
                    return;

                case 'c':
                    return;

                case '>':
                    return;

                case '=':
                    return;

                default:
                    context->state = VT_STATE_NORMAL;
                    return;
            }

        case VT_STATE_NONSTD:
            return;

        case VT_STATE_PALETTE:
            return;

        case VT_STATE_SQUARE:
            return;

        case VT_STATE_GETPARS:
            if (code == ';' && context->par_nr < VT_PAR_NR) {
                context->par_nr++;
                return;
            } else if (isdigit(code)) {
                context->par[context->par_nr] *= 10;
                context->par[context->par_nr] += code - '0';
            }

            if (code >= 0x20 && code <= 0x3f) {
                context->state = VT_STATE_CSIIGNORE;
                return 0;
            }

            context->state = VT_STATE_NORMAL;
            switch (code) {
                case 'h':
                    return;

                case 'l':
                    return;

                case 'c':
                    return;

                case 'm':
                    return;

                case 'n':
                    return;

            }

            switch (code) {
                case 'G': case '`': /* Horizontal position */
                    if (context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, context->par[0], context->cur.ypos);
                    return;

                case 'd': /* Vertical position */
                    if (context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, context->cur.xpos, context->par[0]);
                    return;

                case 'H': case 'f':  /* Horizontal and Vertical position */
                    if (context->par[0])
                        context->par[0]--;
                    if (context->par[1])
                        context->par[1]--;
                    vt_state_goto(context, context->par[1], context->par[0]);
                    return;

                case 'A': /* Upper offset */
                    if (!context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, context->cur.xpos, context->cur.ypos - context->par[0]);
                    return;

                case 'B': case 'e': /* Lower offset */
                    if (!context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, context->cur.xpos, context->cur.ypos + context->par[0]);
                    return;

                case 'D': /* Left offset */
                    if (!context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, context->cur.xpos - context->par[0], context->cur.ypos);
                    return;

                case 'C': case 'a': /* Right offset */
                    if (!context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, context->cur.xpos + context->par[0], context->cur.ypos);
                    return;

                case 'E': /* Vertical position */
                    if (!context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, 0, context->cur.ypos + context->par[0]);
                    return;

                case 'F': /* Vertical position */
                    if (!context->par[0])
                        context->par[0]--;
                    vt_state_goto(context, 0, context->cur.ypos + context->par[0]);
                    return;

                case 'r': /* Reset scrolling region */
                    if (!context->par[0])
                        context->par[0]++;
                    if (!context->par[1])
                        context->par[1] = context->rows;
                    if (context->par[0] < context->par[1] &&
                        context->par[1] <= context->rows) {
                        context->top = context->par[0] - 1;
                        context->bottom = context->par[1];
                        vt_state_goto(context, 0, 0);
                    }
                    return;

                case 'J':
                    vt_state_csi_J(context, context->par[0]);
                    return;

                case 'K':
                    vt_state_csi_K(context, context->par[0]);
                    return;

                case 'L':
                    vt_state_csi_L(context, context->par[0]);
                    return;

                case 'M':
                    vt_state_csi_M(context, context->par[0]);
                    return;

                case 'P':
                    vt_state_csi_P(context, context->par[0]);
                    return;

                case 'X':
                    vt_state_csi_X(context, context->par[0]);
                    return;

                case '@':
                    vt_state_csi_at(context, context->par[0]);
                    return;

                case 'm':
                    vt_state_csi_m(context, context->par[0]);
                    return;

                case 'c':
                    return;

                case 'g':
                    return;

                case 'q':
                    return;

                case 's':
                    vt_state_cur_save(context);
                    return;

                case 'u':
                    vt_state_cur_restore(context);
                    return;

                case ']':
                    return;
            }
            return;

        case VT_STATE_CSIIGNORE:
            if (code >= 20 && code <= 0x3f)
                return;
            context->state = VT_STATE_NORMAL;
            return;

        case VT_STATE_PERCENT:
            context->state = VT_STATE_NORMAL;
            return;

        case VT_STATE_FUNCKEY:
            context->state = VT_STATE_NORMAL;
            return;

        case VT_STATE_HASH:
            context->state = VT_STATE_NORMAL;
            return;

        case VT_STATE_SETG0:
            context->state = VT_STATE_NORMAL;
            return;

        case VT_STATE_SETG1:
            context->state = VT_STATE_NORMAL;
            return;

        case VT_STATE_APC:
            return;

        case VT_STATE_OSC:
            return;

        case VT_STATE_PM:
            return;

        case VT_STATE_DCS:
            return;

        default:
            context->state = VT_STATE_NORMAL;
    }
}

static bool vt_state_is_control(struct vt_context *context, char code)
{
    if (context->state != VT_STATE_NORMAL)
        return true;

    if (code < 32)
        return true;

    if (code == 127)
        return true;

    if (code == 128 + 27)
        return true;

    return false;
}

void vt_state_control(struct vt_context *context, char code)
{
    if (vt_state_is_control(context, code)) {
        vt_state_handle(context, code);
    }
}

static void vt_state_reset(struct vt_context *context)
{
    context->state = VT_STATE_NORMAL;
}
