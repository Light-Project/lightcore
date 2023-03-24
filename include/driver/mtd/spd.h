/* SPDX-License-Identifier: GPL-2.0-or-later */
/* https://en.wikipedia.org/wiki/Serial_presence_detect */
#ifndef _DRIVER_MTD_SPD_H_
#define _DRIVER_MTD_SPD_H_

enum spd_sdr {

};

enum spd_ddr {

};

enum spd_ddr2 {

};

enum spd_ddr3 {

};

enum spd_ddr4 {

};

enum spd_ddr5 {
    SPD_DDR5_BYTES      = 0x00,     /* Number of bytes in SPD device */
    SPD_DDR5_REVISION   = 0x01,     /* SPD revision for base configuration parameters */
    SPD_DDR5_           = 0x02,     /* Key byte / host bus command protocol type */
    SPD_DDR5_           = 0x03,     /* Key byte / module type */

    SPD_DDR5_FST_DENY   = 0x04,     /* First SDRAM density and package */
    SPD_DDR5_FST_ADDR   = 0x05,     /* First SDRAM addressing */
    SPD_DDR5_FST_WIDTH  = 0x06,     /* First SDRAM I/O width */
    SPD_DDR5_FST_BANK   = 0x07,     /* First SDRAM bank groups & banks per bank group */

    SPD_DDR5_SEC_DENY   = 0x08,     /* Second SDRAM density and package */
    SPD_DDR5_SEC_ADDR   = 0x09,     /* Second SDRAM addressing */
    SPD_DDR5_SEC_WIDTH  = 0x0a,     /* Second SDRAM I/O width */
    SPD_DDR5_SEC_BANK   = 0x0b,     /* Second SDRAM bank groups & banks per bank group */

    SPD_DDR5_OPTIONAL   = 0x0c,     /* SDRAM optional features */
    SPD_DDR5_REFRESH    = 0x0d,     /* Thermal and refresh options */
    SPD_DDR5_VOLTAGE    = 0x10,     /* SDRAM nominal voltage, VDD */
};

#endif /* _DRIVER_MTD_SPD_H_ */
