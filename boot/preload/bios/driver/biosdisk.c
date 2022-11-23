/* SPDX-License-Identifier: GPL-2.0-oreg-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <state.h>
#include <minmax.h>
#include <asm/io.h>

/**
 * struct dap_table - Disk Address Packet.
 * @length: packet length.
 * @blocks: block count.
 * @address: buffer address.
 * @segment: buffer segment.
 * @block: LBA number.
 */
struct dap_table {
    uint8_t length;
    uint8_t reserved;
    uint16_t blocks;
    uint16_t address;
    uint16_t segment;
    uint64_t block;
} __packed;

static state legacy_read(char dev, void *buf, uint32_t lba, int count)
{
    struct bios_reg oreg, ireg = {};
    unsigned int mcylinder, mhead, msector;
    unsigned int xfer;

    /* Get disk parameters */
    ireg.ah = 0x8;
    ireg.dl = dev;
    bios_int(0x13, &ireg, &oreg);

    mhead = oreg.dh;
    mcylinder = ((oreg.cl & 0xc0) << 2) | oreg.ch;
    msector = oreg.cl & 0x3f;

    /* Service number:  %AH */
    /* Sector length:   %AL */
    /* Buffer addr:     %BX */
    /* Cylinder:        %CH */
    /* Sector:          %CL */
    /* Head number:     %DH */
    /* Dev number:      %DL */
    /* Buffer offset:   %ES */
    ireg.ah = 0x2;
    ireg.dl = dev;
    ireg.es = MACHINE_SCRATCH_SEG;

    for (; (xfer = min(count, MACHINE_SCRATCH_SIZE / 512)); count -= xfer) {
        unsigned int cylinder, head, sector;

        /* Unit conversion */
        sector = (lba % msector) + 1;
        head = (lba / msector) % (mhead + 1);
        cylinder = ((lba / msector) / (mhead + 1)) % (mcylinder + 1);
        min_adj(xfer, msector - sector + 1);

        ireg.al = xfer;
        ireg.cl = sector;
        ireg.ch = cylinder;
        ireg.dh = head;

        bios_int(0x13, &ireg, &oreg);
        if (oreg.ah) {
            pr_boot("CHS read error: 0x%x\n", oreg.ah);
            return -EIO;
        }

        /* Bounce back to real address */
        setup_bigreal();
        bigreal_memcpy(buf, (void *)MACHINE_SCRATCH_ADDR, xfer * 512);
        buf += xfer * 512;
        lba += xfer;
    }

    return -ENOERR;
}

static state lba_read(char dev, void *buf, uint32_t lba, int count)
{
    struct bios_reg oreg, ireg = {};
    struct dap_table dap;
    unsigned int xfer;

    /* First, let's check whether the hard disk supports LBA */
    ireg.ah = 0x41;
    bios_int(0x13, &ireg, &oreg);
    if (oreg.ah != 0x01) {
        pr_boot("LBA not supported: %d\n", oreg.ah);
        return -EIO;
    }

    /* Service number:      %AH */
    /* Dev number:          %DL */
    /* DAP table address:   %SI */
    /* DAP table offset:    %DS */
    ireg.ah = 0x42;
    ireg.dl = dev;
    ireg.si = (uint16_t)(uintptr_t)&dap;

    for (; (xfer = min(count, MACHINE_SCRATCH_SIZE / 512)); count -= xfer) {
        /* Then we prepare the data table for LBA transmission */
        dap.length = sizeof(dap),
        dap.segment = MACHINE_SCRATCH_SEG,
        dap.block = lba,
        dap.blocks = xfer,

        /* Finally, LBA read transmission is initiated through int 13/42 */
        bios_int(0x13, &ireg, &oreg);
        if (oreg.ah) {
            pr_boot("LBA read error: 0x%x\n", oreg.ah);
            return -EIO;
        }

        /* Bounce back to real address */
        setup_bigreal();
        bigreal_memcpy(buf, (void *)MACHINE_SCRATCH_ADDR, xfer * 512);
        buf += xfer * 512;
        lba += xfer;
    }

    return -ENOERR;
}

void biosdisk_read(uint8_t dev, void *buff, uint32_t lba, int count)
{
    if ((dev < 0x80 || lba_read(dev, buff, lba, count))
        && legacy_read(dev, buff, lba, count))
        panic("BIOS disk service error");
}
