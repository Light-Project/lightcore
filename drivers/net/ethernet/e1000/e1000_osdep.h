/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _E1000_OSDEP_H_
#define _E1000_OSDEP_H_

#include <types.h>
#include <memory.h>
#include <delay.h>
#include <spinlock.h>
#include <mutex.h>
#include <sched.h>
#include <driver/pci.h>
#include <printk.h>
#include <asm/io.h>

#define GBE_RAM_BASE            0x60000
#define GBE_CONFIG_OFFSET       0x0

#define __le16  le16
#define __le32  le32
#define __le64  le64

#define e_dbg(format, arg...) \
	pr_debug(format, ## arg)

#define GBE_CONFIG_RAM_BASE \
	((unsigned int)(GBE_RAM_BASE + GBE_CONFIG_OFFSET))

#define GBE_CONFIG_BASE_VIRT \
	(pa_to_va(GBE_CONFIG_RAM_BASE))

#define GBE_CONFIG_FLASH_WRITE(base, offset, count, data) \
	(writesw(base + offset, data, count))

#define GBE_CONFIG_FLASH_READ(base, offset, count, data) \
	(readsw(base + (offset << 1), data, count))

#define er32(reg)							\
	(readl(hw->hw_addr + ((hw->mac_type >= e1000_82543)		\
		    ? E1000_##reg : E1000_82542_##reg)))

#define ew32(reg, value)						\
	(writel((hw->hw_addr + ((hw->mac_type >= e1000_82543)	\
			? E1000_##reg : E1000_82542_##reg)), (value)))

#define E1000_WRITE_REG_ARRAY(a, reg, offset, value) ( \
    writel(((a)->hw_addr + \
        (((a)->mac_type >= e1000_82543) ? E1000_##reg : E1000_82542_##reg) + \
        ((offset) << 2)), (value)))

#define E1000_READ_REG_ARRAY(a, reg, offset) ( \
    readl((a)->hw_addr + \
        (((a)->mac_type >= e1000_82543) ? E1000_##reg : E1000_82542_##reg) + \
        ((offset) << 2)))

#define E1000_READ_REG_ARRAY_DWORD E1000_READ_REG_ARRAY
#define E1000_WRITE_REG_ARRAY_DWORD E1000_WRITE_REG_ARRAY

#define E1000_WRITE_REG_ARRAY_WORD(a, reg, offset, value) ( \
    writew((value), ((a)->hw_addr + \
        (((a)->mac_type >= e1000_82543) ? E1000_##reg : E1000_82542_##reg) + \
        ((offset) << 1))))

#define E1000_READ_REG_ARRAY_WORD(a, reg, offset) ( \
    readw((a)->hw_addr + \
        (((a)->mac_type >= e1000_82543) ? E1000_##reg : E1000_82542_##reg) + \
        ((offset) << 1)))

#define E1000_WRITE_REG_ARRAY_BYTE(a, reg, offset, value) ( \
    writeb((value), ((a)->hw_addr + \
        (((a)->mac_type >= e1000_82543) ? E1000_##reg : E1000_82542_##reg) + \
        (offset))))

#define E1000_READ_REG_ARRAY_BYTE(a, reg, offset) ( \
    readb((a)->hw_addr + \
        (((a)->mac_type >= e1000_82543) ? E1000_##reg : E1000_82542_##reg) + \
        (offset)))

#define E1000_WRITE_FLUSH() er32(STATUS)

#define E1000_WRITE_ICH_FLASH_REG(a, reg, value) ( \
    writel(((a)->flash_address + reg)), (value))

#define E1000_READ_ICH_FLASH_REG(a, reg) ( \
    readl((a)->flash_address + reg))

#define E1000_WRITE_ICH_FLASH_REG16(a, reg, value) ( \
    writew(((a)->flash_address + reg)), (value))

#define E1000_READ_ICH_FLASH_REG16(a, reg) ( \
    readw((a)->flash_address + reg))

#endif /* _E1000_OSDEP_H_ */
