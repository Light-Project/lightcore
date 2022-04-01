/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <state.h>
#include <asm/io.h>

struct dap_table {
    uint8_t size;       /* 0x00: DAP_TABLE Size */
    uint8_t Res0;       /* 0x01: Reserved0 */
    uint16_t len;       /* 0x02: Blockcount */
    uint16_t buf_addr;  /* 0x04: Buff Addr */
    uint16_t buf_of;    /* 0x06: Buff Offset */
    uint32_t lbal;      /* 0x08: LBA Low */
    uint32_t lbah;      /* 0x0c: LBA High */
} __packed;

static state legacy_read(char dev, void *buf, uint32_t lba, int count)
{
    struct bios_reg or, ir = {};
    unsigned int mcylinder, mhead, msector;
    unsigned int cylinder, head, sector;
    uint8_t bound[512];

    /* Get disk parameters */
    ir.ah = 0x8;
    ir.dl = dev;
    bios_int(0x13, &ir, &or);

    mhead = or.dh;
    mcylinder = ((or.cl & 0xc0) << 2) | or.ch;
    msector = or.cl & 0x3f;

    /* Service number:  %AH */
    /* Sector length:   %AL */
    /* Buffer addr:     %BX */
    /* Cylinder:        %CH */
    /* Sector:          %CL */
    /* Head number:     %DH */
    /* Dev number:      %DL */
    /* Buffer offset:   %ES */
    ir.ah = 0x2;
    ir.al = 1;
    ir.bx = (uint16_t)(uint32_t)&bound;
    ir.dl = dev;

    while (count--) {
        /* Unit conversion */
        sector = (lba % msector) + 1;
        head = (lba / msector) % (mhead + 1);
        cylinder = ((lba / msector) / (mhead + 1)) % (mcylinder + 1);

        ir.cl = (uint8_t)sector;
        ir.ch = (uint8_t)cylinder;
        ir.dh = (uint8_t)head;

        bios_int(0x13, &ir, &or);
        if (or.ah) {
            pr_boot("CHS read error: 0x%x\n", or.ah);
            return -EIO;
        }

        /* Bounce back to real address */
        setup_bigreal();
        bigreal_memcpy(buf, bound, 512);
        buf += 512;
        ++lba;
    }

    return -ENOERR;
}

static state lba_read(char dev, void *buf, uint32_t lba, int count)
{
    struct dap_table dap;
    struct bios_reg regs;
    uint8_t bound[512];

    /* First, let's check whether the hard disk supports LBA */
    memset(&regs, 0, sizeof(regs));
    regs.ax = 0x41 << 8 ;
    bios_int(0x13, &regs, &regs);
    if (regs.ah != 0x01) {
        pr_boot("LBA not supported: %d\n", regs.ah);
        return -EIO;
    }

    while (count--) {
        /* Then we prepare the data table for LBA transmission */
        memset(&dap, 0, sizeof(dap));
        dap.size = sizeof(dap),
        dap.len = 0x01,
        dap.buf_addr = (uint16_t)(uint32_t)&bound,
        dap.lbal = (uint32_t)lba,

        /* Service number:      %AH */
        /* Dev number:          %DL */
        /* DAP table address:   %SI */
        /* DAP table offset:    %DS */
        memset(&regs, 0, sizeof(regs));
        regs.ah = 0x42;
        regs.dl = dev;
        regs.si = (uint16_t)(uint32_t)&dap;

        /* Finally, LBA read transmission is initiated through int 13/42 */
        bios_int(0x13, &regs, &regs);
        if (regs.ah) {
            pr_boot("LBA read error: 0x%x\n", regs.ah);
            return -EIO;
        }

        /* Bounce back to real address */
        setup_bigreal();
        bigreal_memcpy(buf, bound, 512);
        buf += 512;
        ++lba;
    }

    return -ENOERR;
}

void biosdisk_read(uint8_t dev, void *buff, uint32_t lba, int count)
{
    if ((dev < 0x80 || lba_read(dev, buff, lba, count))
        && legacy_read(dev, buff, lba, count))
        panic("BIOS disk service error");
}
