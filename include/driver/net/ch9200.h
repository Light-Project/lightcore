/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_NET_CH9200_H_
#define _DRIVER_NET_CH9200_H_

#include <bits.h>

enum ch9200_req {
    CH9200_REQ_READ     = 0x0e,
    CH9200_REQ_WRITE    = 0x0f,
};

/* Address space:
 * 00-63 : MII
 * 64-128: MAC
 */
enum ch9200_registers {
    CH9200_REG_CTRL             = 64,
    CH9200_REG_STATUS           = 66,
    CH9200_REG_INTERRUPT_MASK   = 68,
    CH9200_REG_PHY_COMMAND      = 70,
    CH9200_REG_PHY_DATA         = 72,
    CH9200_REG_STATION_L        = 74,
    CH9200_REG_STATION_M        = 76,
    CH9200_REG_STATION_H        = 78,
    CH9200_REG_HASH_L           = 80,
    CH9200_REG_HASH_M1          = 82,
    CH9200_REG_HASH_M2          = 84,
    CH9200_REG_HASH_H           = 86,
    CH9200_REG_THRESHOLD        = 88,
    CH9200_REG_FIFO_DEPTH       = 90,
    CH9200_REG_PAUSE            = 92,
    CH9200_REG_FLOW_CONTROL     = 94,
};

/* Control bitflag definitions */
#define CH9200_CTRL_LOOPBACK            BIT(14)
#define CH9200_CTRL_BASE100X            BIT(12)
#define CH9200_CTRL_MBPS_10             BIT(11)
#define CH9200_CTRL_DUPLEX_MODE         BIT(10)
#define CH9200_CTRL_PAUSE_FRAME         BIT(9)
#define CH9200_CTRL_PROMISCUOUS         BIT(8)
#define CH9200_CTRL_MULTICAST           BIT(7)
#define CH9200_CTRL_BROADCAST           BIT(6)
#define CH9200_CTRL_HASH                BIT(5)
#define CH9200_CTRL_APPEND_PAD          BIT(4)
#define CH9200_CTRL_APPEND_CRC          BIT(3)
#define CH9200_CTRL_TRANSMITTER_ACTION  BIT(2)
#define CH9200_CTRL_RECEIVER_ACTION     BIT(1)
#define CH9200_CTRL_DMA_ACTION          BIT(0)

/* Status bitflag definitions */
#define CH9200_STATUS_ALIGNMENT         BIT(6)
#define CH9200_STATUS_FIFO_OVER_RUN     BIT(5)
#define CH9200_STATUS_FIFO_UNDER_RUN    BIT(4)
#define CH9200_STATUS_RX_ERROR          BIT(3)
#define CH9200_STATUS_RX_COMPLETE       BIT(2)
#define CH9200_STATUS_TX_ERROR          BIT(1)
#define CH9200_STATUS_TX_COMPLETE       BIT(0)

/* Fifo depth bitflag definitions */
#define CH9200_DEPTH_ETH_TXBD           BIT(15)
#define CH9200_DEPTH_ETN_TX_FIFO_DEPTH  BIT(8)
#define CH9200_DEPTH_ETH_RXBD           BIT(7)
#define CH9200_DEPTH_ETH_RX_FIFO_DEPTH  BIT(0)

#endif /* _DRIVER_NET_CH9200_H_ */
