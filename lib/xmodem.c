/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <xmodem.h>
#include <crypto/crc.h>
#include <string.h>
#include <kmalloc.h>
#include <proc.h>
#include <export.h>

static __always_inline state
xmodem_read(struct xmodem_ops *ops, void *data, int size)
{
    return ops->read(data, size, ops->pdata);
}

static __always_inline state
xmodem_write(struct xmodem_ops *ops, const void *data, int size)
{
    return ops->write(data, size, ops->pdata);
}

static __always_inline state
xmodem_inb(struct xmodem_ops *ops, uint8_t *data)
{
    return ops->read(data, BYTES_PER_U8, ops->pdata);
}

static __always_inline state
xmodem_outb(struct xmodem_ops *ops, uint8_t data)
{
    return ops->write(&data, BYTES_PER_U8, ops->pdata);
}

int xmodem_receive(struct xmodem_ops *ops, void *data, int size)
{
    struct xmodem_packet *packet;
    uint8_t reqchar, checksum;
    unsigned int count, packid = 1;
    unsigned int retry = ops->retry;
    int rxsize = 0, packsize;
    int retval = -ENOERR;

    reqchar = ops->crc16 ? XMODEM_RDY : XMODEM_NAK;
    packet = kmalloc(sizeof(*packet) + XMODEM_EXTEND + BYTES_PER_U16, GFP_KERNEL);
    if (!packet)
        return -ENOMEM;

    for (;;) {
        for (count = 0; count < ops->timeout; ++count) {
            retval = xmodem_outb(ops, reqchar);
            if (retval)
                goto finish;

            retval = xmodem_inb(ops, &packet->type);
            if (retval) {
                if (retval == -ETIMEDOUT)
                    continue;
                goto notsync;
            }

            switch (packet->type) {
                case XMODEM_SOH:
                    packsize = XMODEM_NORMAL;
                    goto receive;

                case XMODEM_STX:
                    packsize = XMODEM_EXTEND;
                    goto receive;

                case XMODEM_EOT:
                    retval = xmodem_outb(ops, XMODEM_ACK);
                    if (!retval)
                        retval = rxsize;
                    goto finish;

                case XMODEM_CAN:
                    retval = xmodem_outb(ops, XMODEM_ACK);
                    if (!retval)
                        retval = -ECANCELED;
                    goto finish;

                default:
                    retval = -ECONNRESET;
                    goto notsync;
            }
        }

notsync:
        xmodem_outb(ops, XMODEM_CAN);
        xmodem_outb(ops, XMODEM_CAN);
        xmodem_outb(ops, XMODEM_CAN);

        retval = retval ?: -ENOTSYNC;
        goto finish;

receive:
        retval = sizeof(packet->body) + packsize;
        retval += ops->crc16 ? BYTES_PER_U16 : BYTES_PER_U8;
        retval = xmodem_read(ops, &packet->packid, retval);
        if (retval) {
            if (retval == -ETIMEDOUT)
                goto retry;
            break;
        }

        if (packet->packid != packid ||
            packet->rpackid != 0xff - packid)
            goto retry;

        if (ops->crc16) {
            if (unaligned_get_be16(packet->data + packsize) !=
                crc_ccittf(packet->data, packsize, 0))
                goto retry;
        } else {
            for (count = checksum = 0; count < packsize; ++count)
                checksum += packet->data[count];
            if (checksum != *(packet->data + packsize))
                goto retry;
        }

        if (size < packsize) {
            retval = -ENOMEM;
            goto notsync;
        }

        min_adj(packsize, size);
        memcpy(data, &packet->data, packsize);

        rxsize += packsize;
        data += packsize;
        size -= packsize;

        reqchar = XMODEM_ACK;
        continue;

retry:
        if (!retry--) {
            retval = -EREMOTEIO;
            goto notsync;
        }
        reqchar = XMODEM_NAK;
    }

finish:
    kfree(packet);
    return retval;
}
EXPORT_SYMBOL(xmodem_receive);

int xmodem_transmit(struct xmodem_ops *ops, const void *data, int size)
{
    struct xmodem_packet *packet;
    uint8_t value, checksum;
    unsigned int count, packid = 1;
    unsigned int retry = ops->retry;
    int xfer, packsize;
    int retval = -ENOERR;
    bool crc16;

    packet = kmalloc(sizeof(*packet) + XMODEM_EXTEND + BYTES_PER_U16, GFP_KERNEL);
    if (!packet)
        return -ENOMEM;

    packet->type = ops->extend ? XMODEM_STX : XMODEM_SOH;
    packsize = ops->extend ? XMODEM_EXTEND : XMODEM_NORMAL;

    for (count = 0; count < ops->timeout; ++count) {
        retval = xmodem_inb(ops, &value);
        if (retval) {
            if (retval == -ETIMEDOUT)
                continue;
            goto notsync;
        }

        switch (value) {
            case XMODEM_NAK:
                crc16 = false;
                goto transmit;

            case XMODEM_RDY:
                crc16 = true;
                goto transmit;

            case XMODEM_CAN:
                retval = xmodem_outb(ops, XMODEM_ACK);
                if (!retval)
                    retval = -ECANCELED;
                goto finish;

            default:
                retval = -ECONNRESET;
                goto notsync;
        }
    }

notsync:
    xmodem_outb(ops, XMODEM_CAN);
    xmodem_outb(ops, XMODEM_CAN);
    xmodem_outb(ops, XMODEM_CAN);

    retval = retval ?: -ENOTSYNC;
    goto finish;

transmit:
    if (!size) {
        for (count = 0; count < ops->timeout; ++count) {
            retval = xmodem_outb(ops, XMODEM_EOT);
            if (retval)
                goto finish;

            retval = xmodem_inb(ops, &value);
            if (retval) {
                if (retval == -ETIMEDOUT)
                    continue;
                goto notsync;
            }

            switch (value) {
                case XMODEM_ACK:
                    goto finish;

                case XMODEM_NAK:
                    if (!retry--)
                        goto notsync;
                    break;

                case XMODEM_CAN:
                    retval = xmodem_outb(ops, XMODEM_ACK);
                    if (!retval)
                        retval = -ECANCELED;
                    goto finish;

                default:
                    retval = -ECONNRESET;
                    goto notsync;
            }
        }
    } else {
        packet->packid = packid;
        packet->rpackid = 0xff - packid;

        xfer = min(size, packsize);
        memcpy(packet->data, data, xfer);
        memset(packet->data + xfer, XMODEM_FILLER, packsize - xfer);

        if (crc16) {
            unaligned_set_be16(packet->data + packsize,
                crc_ccittf(packet->data, packsize, 0));
        } else {
            for (count = checksum = 0; count < packsize; ++count)
                checksum += packet->data[count];
            packet->data[packsize] = checksum;
        }

        for (count = 0; count < ops->timeout; ++count) {
            retval = sizeof(*packet) + packsize;
            retval += ops->crc16 ? BYTES_PER_U16 : BYTES_PER_U8;
            retval = xmodem_write(ops, packet, retval);
            if (retval)
                break;

            retval = xmodem_inb(ops, &value);
            if (retval) {
                if (retval == -ETIMEDOUT)
                    continue;
                goto notsync;
            }

            switch (value) {
                case XMODEM_ACK:
                    size -= xfer;
                    data += xfer;
                    packid++;
                    goto transmit;

                case XMODEM_NAK:
                    if (!retry--)
                        goto notsync;
                    break;

                case XMODEM_CAN:
                    retval = xmodem_outb(ops, XMODEM_ACK);
                    if (!retval)
                        retval = -ECANCELED;
                    goto finish;

                default:
                    retval = -ECONNRESET;
                    goto notsync;
            }
        }
    }
    goto notsync;

finish:
    kfree(packet);
    return retval;
}
EXPORT_SYMBOL(xmodem_transmit);
