/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _XMODEM_H_
#define _XMODEM_H_

#include <state.h>
#include <stddef.h>

#define XMODEM_NORMAL 128
#define XMODEM_EXTEND 1024
#define XMODEM_FILLER 0x1a

typedef state (*xmodem_read_t)(void *buff, int size, void *pdata);
typedef state (*xmodem_write_t)(const void *buff, int size, void *pdata);

enum xmodem_types {
    XMODEM_SOH  = 0x01, /* TX: Start of Header */
    XMODEM_STX  = 0x02, /* TX: Start of Extend Header */
    XMODEM_EOT  = 0x04, /* TX: End of Transmission */
    XMODEM_ACK  = 0x06, /* TR: Acknowledge */
    XMODEM_NAK  = 0x15, /* RX: Not Acknowledge */
    XMODEM_CAN  = 0x18, /* TR: Cancel Transmission */
    XMODEM_RDY  = 0x43, /* RX: Ready to Receive Data */
};

struct xmodem_packet {
    uint8_t type;
    struct_group(body,
        uint8_t packid;
        uint8_t rpackid;
        uint8_t data[0];
    );
} __packed;

struct xmodem_ops {
    unsigned int timeout;
    unsigned int retry;
    bool crc16;
    bool extend;

    xmodem_read_t read;
    xmodem_write_t write;
    void *pdata;
};

extern int xmodem_receive(struct xmodem_ops *ops, void *data, int size);
extern int xmodem_transmit(struct xmodem_ops *ops, const void *data, int size);

#endif /* _XMODEM_H_ */
