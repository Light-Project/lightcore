/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <boot.h>
#include <kernel.h>
#include <driver/mmc.h>

#define SDCARD_VOLTAGE  (MMC_VOLTA_32_33 | MMC_VOLTA_33_34)
static bool sdhc;

bool sdcard_init(void)
{
    struct mmc_cmd cmd;
    struct mmc_cid *cid;
    int timeout = 1000;
    uint32_t rca;
    bool ver2;

    /* Reset sdcard first */
    cmd.cmdtype = MMC_RESP_NONE;
    cmd.cmdidx = MMC_GO_IDLE_STATE;
    cmd.cmdarg = 0;
    mmc_send_cmd(&cmd, NULL);

    mdelay(2);

    /* Check sdcard version */
    cmd.cmdtype = MMC_RESP_R7;
    cmd.cmdidx = MMC_SEND_EXT_CSD;
    cmd.cmdarg = ((SDCARD_VOLTAGE & 0xff8000) != 0) << 8 | 0xaa;
    mmc_send_cmd(&cmd, NULL);
    ver2 = (cmd.response[0] & 0xff) == 0xaa;

    for (;;) {
        cmd.cmdtype = MMC_RESP_R1;
        cmd.cmdidx = MMC_APP_CMD;
        cmd.cmdarg = 0;
        mmc_send_cmd(&cmd, NULL);

        cmd.cmdtype = MMC_RESP_R3;
        cmd.cmdidx = 41;
        cmd.cmdarg = SDCARD_VOLTAGE;
        if (ver2)
            cmd.cmdarg |= MMC_OCR_HCS;
        mmc_send_cmd(&cmd, NULL);

        if (cmd.response[0] & MMC_OCR_BUSY)
            break;

        if (!timeout--)
            return true;
        mdelay(1);
    }

    sdhc = !!(cmd.response[0] & MMC_OCR_HCS);

    /* Set sdcard into identify mode */
    cmd.cmdtype = MMC_RESP_R2;
    cmd.cmdidx = MMC_ALL_SEND_CID;
    cmd.cmdarg = 0;
    mmc_send_cmd(&cmd, NULL);

    cid = (void *)&cmd.response[0];
    pr_boot("Sdcard Manufacturer: 0x%x\n", cid->manfid);
    pr_boot("Sdcard OEM ID: 0x%x\n", cid->oemid);
    pr_boot("Sdcard Type: %s\n", sdhc ? "SDHC" : "SD");

    /* Set the Relative Address */
    cmd.cmdtype = MMC_RESP_R1;
    cmd.cmdidx = MMC_SET_RELATIVE_ADDR;
    cmd.cmdarg = 0;
    mmc_send_cmd(&cmd, NULL);
    rca = (cmd.response[0] >> 16) & 0xffff;

    /* Set sdcard into transfer mode */
    cmd.cmdtype = MMC_RESP_R1;
    cmd.cmdidx = MMC_SELECT_CARD;
    cmd.cmdarg = rca << 16;
    mmc_send_cmd(&cmd, NULL);

    return false;
}

void sdcard_read(uint8_t *buff, uint32_t lba, uint32_t blkcnt)
{
    struct mmc_cmd cmd;
    struct mmc_data data;

    if (blkcnt > 1)
        cmd.cmdidx = MMC_READ_MULTIPLE_BLOCK;
    else
        cmd.cmdidx = MMC_READ_SINGLE_BLOCK;

    if (sdhc)
        cmd.cmdarg = lba;
    else
        cmd.cmdarg = lba * 512;

    cmd.cmdtype = MMC_RESP_R1;
    data.rw = MMC_READ;
    data.blksize = 512;
    data.blkcnt = blkcnt;
    data.buff = buff;

    mmc_send_cmd(&cmd, &data);
}
