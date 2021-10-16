/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_I2C_I801_H_
#define _DRIVER_I2C_I801_H_

enum i801_pci_registers {
    I801_SMBHSTCFG      = 0x040,
    I801_TCOBASE        = 0x050,
    I801_TCOCTL         = 0x054,
};

#define I801_TCOCTL_EN  BIT(8)

#endif /* _DRIVER_I2C_I801_H_ */
