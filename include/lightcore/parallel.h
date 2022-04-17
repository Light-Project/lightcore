/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LIGHTCORE_PARALLEL_H_
#define _LIGHTCORE_PARALLEL_H_

#define PARALLEL_STATUS_ERROR       0x08    /* PIN  15: Error Input Status  */
#define PARALLEL_STATUS_SELECT      0x10    /* PIN  13: Select-in Input Status */
#define PARALLEL_STATUS_PAPEROUT    0x20    /* PIN  12: Paper-out Input Status */
#define PARALLEL_STATUS_ACK         0x40    /* PIN  10: Ack Input Status */
#define PARALLEL_STATUS_BUSY        0x80    /* PIN ~11: Busy Input Status */

#define PARALLEL_CONTROL_STROBE     0x01    /* PIN  ~1: Strobe Status */
#define PARALLEL_CONTROL_AUTOFD     0x02    /* PIN ~14: Line-feed Status */
#define PARALLEL_CONTROL_INIT       0x04    /* PIN  16: Initialization Status */
#define PARALLEL_CONTROL_SELECT     0x08    /* PIN ~17: Select-printer Status */

#endif  /* _LIGHTCORE_PARALLEL_H_ */
