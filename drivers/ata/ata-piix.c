/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *  bootloader/x86/driver/ide.c
 *  x86 pre ide driver
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-02-26      first version 
 * 
 */

#include <types.h>
#include <state.h>
#include <driver/pci.h>
#include <init/initcall.h>
#include <driver/ata.h>
#include <printk.h>

enum piix_controller_ids {
    /* controller IDs */
    piix_pata_mwdma,        /* PIIX3 MWDMA only */
    piix_pata_33,           /* PIIX4 at 33Mhz */
    ich_pata_33,            /* ICH up to UDMA 33 only */
    ich_pata_66,            /* ICH up to 66 Mhz */
    ich_pata_100,           /* ICH up to UDMA 100 */
    ich_pata_100_nomwdma1,  /* ICH up to UDMA 100 but with no MWDMA1*/
    ich5_sata,
    ich6_sata,
    ich6m_sata,
    ich8_sata,
    ich8_2port_sata,
    ich8m_apple_sata,       /* locks up on second port enable */
    tolapai_sata,
    piix_pata_vmw,          /* PIIX4 for VMware, spurious DMA_ERR */
    ich8_sata_snb,
    ich8_2port_sata_snb,
    ich8_2port_sata_byt,
};

static const struct pci_device_id piix_pci_tbl[] = {
    /* Intel PIIX3 for the 430HX etc */
    { 0x8086, 0x7010, PCI_ANY_ID, PCI_ANY_ID, 0, 0, piix_pata_mwdma },
    /* VMware ICH4 */
    { 0x8086, 0x7111, 0x15ad, 0x1976, 0, 0, piix_pata_vmw },
    /* Intel PIIX4 for the 430TX/440BX/MX chipset: UDMA 33 */
    /* Also PIIX4E (fn3 rev 2) and PIIX4M (fn3 rev 3) */
    { 0x8086, 0x7111, PCI_ANY_ID, PCI_ANY_ID, 0, 0, piix_pata_33 },
    /* Intel PIIX4 */
    { 0x8086, 0x7199, PCI_ANY_ID, PCI_ANY_ID, 0, 0, piix_pata_33 },
    /* Intel PIIX4 */
    { 0x8086, 0x7601, PCI_ANY_ID, PCI_ANY_ID, 0, 0, piix_pata_33 },
    /* Intel PIIX */
    { 0x8086, 0x84CA, PCI_ANY_ID, PCI_ANY_ID, 0, 0, piix_pata_33 },
    /* Intel ICH (i810, i815, i840) UDMA 66*/
    { 0x8086, 0x2411, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_66 },
    /* Intel ICH0 : UDMA 33*/
    { 0x8086, 0x2421, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_33 },
    /* Intel ICH2M */
    { 0x8086, 0x244A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* Intel ICH2 (i810E2, i845, 850, 860) UDMA 100 */
    { 0x8086, 0x244B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /*  Intel ICH3M */
    { 0x8086, 0x248A, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* Intel ICH3 (E7500/1) UDMA 100 */
    { 0x8086, 0x248B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* Intel ICH4-L */
    { 0x8086, 0x24C1, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* Intel ICH4 (i845GV, i845E, i852, i855) UDMA 100 */
    { 0x8086, 0x24CA, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    { 0x8086, 0x24CB, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* Intel ICH5 */
    { 0x8086, 0x24DB, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* C-ICH (i810E2) */
    { 0x8086, 0x245B, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* ESB (855GME/875P + 6300ESB) UDMA 100  */
    { 0x8086, 0x25A2, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* ICH6 (and 6) (i915) UDMA 100 */
    { 0x8086, 0x266F, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },
    /* ICH7/7-R (i945, i975) UDMA 100*/
    { 0x8086, 0x27DF, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100_nomwdma1 },
    { 0x8086, 0x269E, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100_nomwdma1 },
    /* ICH8 Mobile PATA Controller */
    { 0x8086, 0x2850, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich_pata_100 },

    /* SATA ports */

    /* 82801EB (ICH5) */
    { 0x8086, 0x24d1, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich5_sata },
    /* 82801EB (ICH5) */
    { 0x8086, 0x24df, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich5_sata },
    /* 6300ESB (ICH5 variant with broken PCS present bits) */
    { 0x8086, 0x25a3, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich5_sata },
    /* 6300ESB pretending RAID */
    { 0x8086, 0x25b0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich5_sata },
    /* 82801FB/FW (ICH6/ICH6W) */
    { 0x8086, 0x2651, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich6_sata },
    /* 82801FR/FRW (ICH6R/ICH6RW) */
    { 0x8086, 0x2652, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich6_sata },
    /* 82801FBM ICH6M (ICH6R with only port 0 and 2 implemented).
     * Attach iff the controller is in IDE mode. */
    { 0x8086, 0x2653, PCI_ANY_ID, PCI_ANY_ID,
      PCI_CLASS_STORAGE_IDE << 8, 0xffff00, ich6m_sata },
    /* 82801GB/GR/GH (ICH7, identical to ICH6) */
    { 0x8086, 0x27c0, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich6_sata },
    /* 82801GBM/GHM (ICH7M, identical to ICH6M)  */
    { 0x8086, 0x27c4, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich6m_sata },
    /* Enterprise Southbridge 2 (631xESB/632xESB) */
    { 0x8086, 0x2680, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich6_sata },
    /* SATA Controller 1 IDE (ICH8) */
    { 0x8086, 0x2820, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller 2 IDE (ICH8) */
    { 0x8086, 0x2825, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* Mobile SATA Controller IDE (ICH8M), Apple */
    { 0x8086, 0x2828, 0x106b, 0x00a0, 0, 0, ich8m_apple_sata },
    { 0x8086, 0x2828, 0x106b, 0x00a1, 0, 0, ich8m_apple_sata },
    { 0x8086, 0x2828, 0x106b, 0x00a3, 0, 0, ich8m_apple_sata },
    /* Mobile SATA Controller IDE (ICH8M) */
    { 0x8086, 0x2828, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (ICH9) */
    { 0x8086, 0x2920, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (ICH9) */
    { 0x8086, 0x2921, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (ICH9) */
    { 0x8086, 0x2926, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (ICH9M) */
    { 0x8086, 0x2928, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (ICH9M) */
    { 0x8086, 0x292d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (ICH9M) */
    { 0x8086, 0x292e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (Tolapai) */
    { 0x8086, 0x5028, PCI_ANY_ID, PCI_ANY_ID, 0, 0, tolapai_sata },
    /* SATA Controller IDE (ICH10) */
    { 0x8086, 0x3a00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (ICH10) */
    { 0x8086, 0x3a06, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (ICH10) */
    { 0x8086, 0x3a20, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (ICH10) */
    { 0x8086, 0x3a26, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (PCH) */
    { 0x8086, 0x3b20, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (PCH) */
    { 0x8086, 0x3b21, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (PCH) */
    { 0x8086, 0x3b26, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (PCH) */
    { 0x8086, 0x3b28, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (PCH) */
    { 0x8086, 0x3b2d, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (PCH) */
    { 0x8086, 0x3b2e, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata },
    /* SATA Controller IDE (CPT) */
    { 0x8086, 0x1c00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (CPT) */
    { 0x8086, 0x1c01, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (CPT) */
    { 0x8086, 0x1c08, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (CPT) */
    { 0x8086, 0x1c09, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (PBG) */
    { 0x8086, 0x1d00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (PBG) */
    { 0x8086, 0x1d08, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Panther Point) */
    { 0x8086, 0x1e00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Panther Point) */
    { 0x8086, 0x1e01, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Panther Point) */
    { 0x8086, 0x1e08, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Panther Point) */
    { 0x8086, 0x1e09, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Lynx Point) */
    { 0x8086, 0x8c00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Lynx Point) */
    { 0x8086, 0x8c01, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Lynx Point) */
    { 0x8086, 0x8c08, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata_snb },
    /* SATA Controller IDE (Lynx Point) */
    { 0x8086, 0x8c09, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Lynx Point-LP) */
    { 0x8086, 0x9c00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Lynx Point-LP) */
    { 0x8086, 0x9c01, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Lynx Point-LP) */
    { 0x8086, 0x9c08, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Lynx Point-LP) */
    { 0x8086, 0x9c09, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (DH89xxCC) */
    { 0x8086, 0x2326, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Avoton) */
    { 0x8086, 0x1f20, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Avoton) */
    { 0x8086, 0x1f21, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Avoton) */
    { 0x8086, 0x1f30, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Avoton) */
    { 0x8086, 0x1f31, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (Wellsburg) */
    { 0x8086, 0x8d00, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Wellsburg) */
    { 0x8086, 0x8d08, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata_snb },
    /* SATA Controller IDE (Wellsburg) */
    { 0x8086, 0x8d60, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (Wellsburg) */
    { 0x8086, 0x8d68, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (BayTrail) */
    { 0x8086, 0x0F20, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata_byt },
    { 0x8086, 0x0F21, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata_byt },
    /* SATA Controller IDE (Coleto Creek) */
    { 0x8086, 0x23a6, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata },
    /* SATA Controller IDE (9 Series) */
    { 0x8086, 0x8c88, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata_snb },
    /* SATA Controller IDE (9 Series) */
    { 0x8086, 0x8c89, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_2port_sata_snb },
    /* SATA Controller IDE (9 Series) */
    { 0x8086, 0x8c80, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    /* SATA Controller IDE (9 Series) */
    { 0x8086, 0x8c81, PCI_ANY_ID, PCI_ANY_ID, 0, 0, ich8_sata_snb },
    { }	/* terminate list */
};

// static struct ata_ops piix_ops = {

// };

static state piix_probe(struct pci_device *pdev, int pdata)
{
    
    return -ENOERR;
}

static state piix_remove(struct pci_device *pdev)
{
    
    return -ENOERR;
}

static struct pci_driver piix_pci_driver = {
    .driver = {
        .name = "ata-piix",
    },
    .id_table = piix_pci_tbl,
    .probe = piix_probe,
    .remove = piix_remove,
};

static state piix_ata_init(void)
{
    return pci_driver_register(&piix_pci_driver);
}

driver_initcall(piix_ata_init);
