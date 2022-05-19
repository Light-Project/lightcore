/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 * devices id form: linux/drivers/mfd/lpc_ich.c
 */

#define DRIVER_NAME "intel-ich"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <driver/platform.h>
#include <driver/pci.h>
#include <driver/mfd/intel-ich.h>
#include <driver/gpio/ich.h>
#include <driver/mtd/ich.h>
#include <driver/mtd/intel.h>
#include <driver/watchdog/tco.h>
#include <printk.h>

struct ich_chipinfo {
    const char *name;
    const char *itco_version;
    const char *gpio_version;
    const char *spi_version;
};

const char ich_tco_v1[] = TCO_MATCH_ID(1);
const char ich_tco_v2[] = TCO_MATCH_ID(2);
const char ich_tco_v3[] = TCO_MATCH_ID(3);
const char ich_tco_v4[] = TCO_MATCH_ID(4);
const char ich_tco_v5[] = TCO_MATCH_ID(5);

const char ich_gpio_i3100[] = INTEL_GPIO_I3100;
const char ich_gpio_v5[]    = INTEL_GPIO_V5;
const char ich_gpio_v6[]    = INTEL_GPIO_V6;
const char ich_gpio_v7[]    = INTEL_GPIO_V7;
const char ich_gpio_v9[]    = INTEL_GPIO_V9;
const char ich_gpio_v10cp[] = INTEL_GPIO_V10CP;
const char ich_gpio_v10cs[] = INTEL_GPIO_V10CS;
const char ich_gpio_avn[]   = INTEL_GPIO_V10CS;

const char ich_spi_leg[] = INTEL_SPI_LEG_MATCH_ID;
const char ich_spi_ich[] = INTEL_SPI_ICH_MATCH_ID;
const char ich_spi_byt[] = INTEL_SPI_BYT_MATCH_ID;
const char ich_spi_lpt[] = INTEL_SPI_LPT_MATCH_ID;
const char ich_spi_bxt[] = INTEL_SPI_BXT_MATCH_ID;
const char ich_spi_cnl[] = INTEL_SPI_CNL_MATCH_ID;

static struct ich_chipinfo lpc_ich = {
    .name = "ICH",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich0 = {
    .name = "ICH0",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich2 = {
    .name = "ICH2",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich2m = {
    .name = "ICH2-M",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich3 = {
    .name = "ICH3",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich3m = {
    .name = "ICH3-M",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich4 = {
    .name = "ICH4",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich4m = {
    .name = "ICH4-M",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_cich = {
    .name = "C-ICH",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich5 = {
    .name = "ICH5/ICH5R",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_6300esb = {
    .name = "6300ESB",
    .itco_version = ich_tco_v1,
};

static struct ich_chipinfo lpc_ich6 = {
    .name = "ICH6",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v6,
};

static struct ich_chipinfo lpc_ich6m = {
    .name = "ICH6-M",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v6,
};

static struct ich_chipinfo lpc_ich6w = {
    .name = "ICH6-W",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v6,
};

static struct ich_chipinfo lpc_631xesb = {
    .name = "631xESB/632xESB",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v6,
};

static struct ich_chipinfo lpc_ich7 = {
    .name = "ICH7",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_leg,
};

static struct ich_chipinfo lpc_ich7dh = {
    .name = "ICH7-DH",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_leg,
};

static struct ich_chipinfo lpc_ich7m = {
    .name = "ICH7-M/U",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_leg,
};

static struct ich_chipinfo lpc_ich7mdh = {
    .name = "ICH7-MDH",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_leg,
};

static struct ich_chipinfo lpc_nm10 = {
    .name = "NM10",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
};

static struct ich_chipinfo lpc_ich8 = {
    .name = "ICH8/R",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich8me = {
    .name = "ICH8-ME",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich8dh = {
    .name = "ICH8-DH",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich8do = {
    .name = "ICH8-DO",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich8m = {
    .name = "ICH8-M",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v7,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich9 = {
    .name = "ICH9",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v9,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich9r = {
    .name = "ICH9-R",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v9,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich9dh = {
    .name = "ICH9-DH",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v9,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich9do = {
    .name = "ICH9-DO",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v9,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich9m = {
    .name = "ICH9-M",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v9,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich9me = {
    .name = "ICH9-ME",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v9,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich10 = {
    .name = "ICH10",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v10cs,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich10r = {
    .name = "ICH10-R",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v10cs,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich10d = {
    .name = "ICH10-D",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v10cp,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_ich10do = {
    .name = "ICH10-DO",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v10cp,
    .spi_version = ich_spi_ich,
};

static struct ich_chipinfo lpc_pch = {
    .name = "PCH Desktop Full Featured",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_pchm = {
    .name = "PCH Mobile Full Featured",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_p55 = {
    .name = "P55",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_pm55 = {
    .name = "PM55",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_h55 = {
    .name = "H55",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_hm55 = {
    .name = "HM55",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_h57 = {
    .name = "H57",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_hm57 = {
    .name = "HM57",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_q57 = {
    .name = "Q57",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_qm57 = {
    .name = "QM57",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};
static struct ich_chipinfo lpc_pchmsff = {
    .name = "PCH Mobile SFF Full Featured",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_qs57 = {
    .name = "QS57",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_3400 = {
    .name = "3400",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_3420 = {
    .name = "3420",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_3450 = {
    .name = "3450",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_ep80579 = {
    .name = "EP80579",
    .itco_version = ich_tco_v2,
};

static struct ich_chipinfo lpc_cpt = {
    .name = "Cougar Point",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_cptd = {
    .name = "Cougar Point Desktop",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_cptm = {
    .name = "Cougar Point Mobile",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_pbg = {
    .name = "Patsburg",
    .itco_version = ich_tco_v2,
};

static struct ich_chipinfo lpc_dh89xxcc = {
    .name = "DH89xxCC",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_ppt = {
    .name = "Panther Point",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_lpt = {
    .name = "Lynx Point",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
    .spi_version = ich_spi_lpt,
};

static struct ich_chipinfo lpc_lpt_lp = {
    .name = "Lynx Point-LP",
    .itco_version = ich_tco_v2,
    .spi_version = ich_spi_lpt,
};

static struct ich_chipinfo lpc_wbg = {
    .name = "Wellsburg",
    .itco_version = ich_tco_v2,
};

static struct ich_chipinfo lpc_avn = {
    .name = "Avoton SoC",
    .itco_version = ich_tco_v3,
    .gpio_version = ich_gpio_avn,
    .spi_version = ich_spi_bxt,
};

static struct ich_chipinfo lpc_baytrail = {
    .name = "Bay Trail SoC",
    .itco_version = ich_tco_v3,
    .spi_version = ich_spi_byt,
};

static struct ich_chipinfo lpc_coleto = {
    .name = "Coleto Creek",
    .itco_version = ich_tco_v2,
};

static struct ich_chipinfo lpc_wpt_lp = {
    .name = "Wildcat Point_LP",
    .itco_version = ich_tco_v2,
    .spi_version = ich_spi_lpt,
};

static struct ich_chipinfo lpc_braswell = {
    .name = "Braswell SoC",
    .itco_version = ich_tco_v3,
    .spi_version = ich_spi_byt,
};

static struct ich_chipinfo lpc_lewisburg = {
    .name = "Lewisburg",
    .itco_version = ich_tco_v2,
};

static struct ich_chipinfo lpc_9s = {
    .name = "9 Series",
    .itco_version = ich_tco_v2,
    .gpio_version = ich_gpio_v5,
};

static struct ich_chipinfo lpc_apl = {
    .name = "Apollo Lake SoC",
    .itco_version = ich_tco_v5,
    .spi_version = ich_spi_bxt,
};

static struct ich_chipinfo lpc_glk = {
    .name = "Gemini Lake SoC",
    .spi_version = ich_spi_bxt,
};

static struct ich_chipinfo lpc_cougarmountain = {
    .name = "Cougar Mountain SoC",
    .itco_version = ich_tco_v3,
};

static const struct pci_device_id intel_ich_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x0f1c), .data = &lpc_baytrail        },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c41), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c42), .data = &lpc_cptd            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c43), .data = &lpc_cptm            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c44), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c45), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c46), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c47), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c48), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c49), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c4a), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c4b), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c4c), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c4d), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c4e), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c4f), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c50), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c51), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c52), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c53), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c54), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c55), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c56), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c57), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c58), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c59), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c5a), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c5b), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c5c), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c5d), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c5e), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1c5f), .data = &lpc_cpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1d40), .data = &lpc_pbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1d41), .data = &lpc_pbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e40), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e41), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e42), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e43), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e44), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e45), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e46), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e47), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e48), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e49), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e4a), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e4b), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e4c), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e4d), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e4e), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e4f), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e50), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e51), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e52), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e53), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e54), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e55), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e56), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e57), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e58), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e59), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e5a), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e5b), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e5c), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e5d), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e5e), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1e5f), .data = &lpc_ppt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1f38), .data = &lpc_avn             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1f39), .data = &lpc_avn             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1f3a), .data = &lpc_avn             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x1f3b), .data = &lpc_avn             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x229c), .data = &lpc_braswell        },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2310), .data = &lpc_dh89xxcc        },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2390), .data = &lpc_coleto          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2410), .data = &lpc_ich             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2420), .data = &lpc_ich0            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2440), .data = &lpc_ich2            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x244c), .data = &lpc_ich2m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2450), .data = &lpc_cich            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2480), .data = &lpc_ich3            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x248c), .data = &lpc_ich3m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x24c0), .data = &lpc_ich4            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x24cc), .data = &lpc_ich4m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x24d0), .data = &lpc_ich5            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x25a1), .data = &lpc_6300esb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2640), .data = &lpc_ich6            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2641), .data = &lpc_ich6m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2642), .data = &lpc_ich6w           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2670), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2671), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2672), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2673), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2674), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2675), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2676), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2677), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2678), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2679), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267a), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267b), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267c), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267d), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267e), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x267f), .data = &lpc_631xesb         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27b0), .data = &lpc_ich7dh          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27b8), .data = &lpc_ich7            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27b9), .data = &lpc_ich7m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27bc), .data = &lpc_nm10            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x27bd), .data = &lpc_ich7mdh         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2810), .data = &lpc_ich8            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2811), .data = &lpc_ich8me          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2812), .data = &lpc_ich8dh          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2814), .data = &lpc_ich8do          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2815), .data = &lpc_ich8m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2912), .data = &lpc_ich9dh          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2914), .data = &lpc_ich9do          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2916), .data = &lpc_ich9r           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2917), .data = &lpc_ich9me          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2918), .data = &lpc_ich9            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2919), .data = &lpc_ich9m           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3197), .data = &lpc_glk             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x2b9c), .data = &lpc_cougarmountain  },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3a14), .data = &lpc_ich10do         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3a16), .data = &lpc_ich10r          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3a18), .data = &lpc_ich10           },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3a1a), .data = &lpc_ich10d          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b00), .data = &lpc_pch             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b01), .data = &lpc_pchm            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b02), .data = &lpc_p55             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b03), .data = &lpc_pm55            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b06), .data = &lpc_h55             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b07), .data = &lpc_qm57            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b08), .data = &lpc_h57             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b09), .data = &lpc_hm55            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b0a), .data = &lpc_q57             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b0b), .data = &lpc_hm57            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b0d), .data = &lpc_pchmsff         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b0f), .data = &lpc_qs57            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b12), .data = &lpc_3400            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b14), .data = &lpc_3420            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x3b16), .data = &lpc_3450            },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x5031), .data = &lpc_ep80579         },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x5ae8), .data = &lpc_apl             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c40), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c41), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c42), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c43), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c44), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c45), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c46), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c47), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c48), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c49), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c4a), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c4b), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c4c), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c4d), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c4e), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c4f), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c50), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c51), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c52), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c53), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c54), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c55), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c56), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c57), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c58), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c59), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c5a), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c5b), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c5c), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c5d), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c5e), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8c5f), .data = &lpc_lpt             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8cc1), .data = &lpc_9s              },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8cc2), .data = &lpc_9s              },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8cc3), .data = &lpc_9s              },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8cc4), .data = &lpc_9s              },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8cc6), .data = &lpc_9s              },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d40), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d41), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d42), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d43), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d44), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d45), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d46), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d47), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d48), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d49), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d4a), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d4b), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d4c), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d4d), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d4e), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d4f), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d50), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d51), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d52), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d53), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d54), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d55), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d56), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d57), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d58), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d59), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d5a), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d5b), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d5c), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d5d), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d5e), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x8d5f), .data = &lpc_wbg             },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c40), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c41), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c42), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c43), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c44), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c45), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c46), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9c47), .data = &lpc_lpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc1), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc2), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc3), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc5), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc6), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc7), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0x9cc9), .data = &lpc_wpt_lp          },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c1), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c2), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c3), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c4), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c5), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c6), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa1c7), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa242), .data = &lpc_lewisburg       },
    { PCI_DEVICE(PCI_VENDOR_ID_INTEL, 0xa243), .data = &lpc_lewisburg       },
    { }, /* NULL */
};

struct ich_device {
    struct platform_device tco;
    struct resource tco_res[3];
    struct platform_device gpio;
    struct resource gpio_res[2];
    struct platform_device spi;
    struct resource spi_res[1];
    uint8_t gpio_base, gctl_base;
    uint8_t acpi_save, pmc_save, gctl_save;
};

static void intel_ich_enable_acpi(struct pci_device *pdev, const struct ich_chipinfo *info)
{
    struct ich_device *idev = pci_get_devdata(pdev);
    uint8_t val;

    if (info->itco_version == ich_tco_v3) {
        /*
         * Some chipsets (eg Avoton) enable the ACPI space in the
         * ACPI BASE register.
         */

        val = pci_config_readb(pdev, INTEL_ICH_APCI);
        pci_config_writeb(pdev, INTEL_ICH_APCI, val | BIT(1));

        if (!idev->acpi_save)
            idev->acpi_save = val;
    } else {
        /*
         * Most chipsets enable the ACPI space in the ACPI control
         * register.
         */

        val = pci_config_readb(pdev, INTEL_ICH_PMC);
        pci_config_writeb(pdev, INTEL_ICH_PMC, val | BIT(7));

        if (!idev->pmc_save)
            idev->pmc_save = val;
    }
}

static void intel_ich_enable_pmc(struct pci_device *pdev)
{
    struct ich_device *idev = pci_get_devdata(pdev);
    uint8_t val;

    val = pci_config_readl(pdev, INTEL_ICH_PMC);
    pci_config_writel(pdev, INTEL_ICH_PMC, val | BIT(1));

    if (!idev->pmc_save)
        idev->pmc_save = val;
}

static void intel_ich_enable_gpio(struct pci_device *pdev)
{
    struct ich_device *idev = pci_get_devdata(pdev);
    uint8_t val;

    val = pci_config_readl(pdev, idev->gctl_base);
    pci_config_writel(pdev, idev->gctl_base, val | BIT(4));

    if (!idev->pmc_save)
        idev->pmc_save = val;
}

static void intel_ich_restore_config(struct pci_device *pdev)
{
    struct ich_device *idev = pci_get_devdata(pdev);

    if (idev->acpi_save)
        pci_config_writeb(pdev, INTEL_ICH_APCI, idev->acpi_save);
    if (idev->pmc_save)
        pci_config_writeb(pdev, INTEL_ICH_PMC, idev->pmc_save);
    if (idev->gctl_save)
        pci_config_writeb(pdev, idev->gctl_base, idev->gctl_save);
}

static state intel_ich_tco_setup(struct pci_device *pdev, const struct ich_chipinfo *info)
{
    struct ich_device *idev = pci_get_devdata(pdev);
    uint32_t val, base;

    val = pci_config_readl(pdev, INTEL_ICH_APCI);
    base = val & 0xff80;
    if (!base) {
        dev_warn(&pdev->dev, "ACPI I/O uninitialized\n");
        return -ENODEV;
    }

    idev->tco_res[TCO_RES_TCO].start = base + INTEL_ICH_ACPI_TCO;
    idev->tco_res[TCO_RES_TCO].size  = INTEL_ICH_ACPI_TCO_SZ;
    idev->tco_res[TCO_RES_TCO].type  = RESOURCE_PMIO;

    idev->tco_res[TCO_RES_SMI].start = base + INTEL_ICH_ACPI_SMI;
    idev->tco_res[TCO_RES_SMI].size  = INTEL_ICH_ACPI_SMI_SZ;
    idev->tco_res[TCO_RES_SMI].type  = RESOURCE_PMIO;

    intel_ich_enable_acpi(pdev, info);

    if (info->itco_version == ich_tco_v2) {
        /*
         * Get the Memory-Mapped GCS register. To get access to it
         * we have to read RCBA from PCI Config space 0xf0 and use
         * it as base. GCS = RCBA + ICH6_GCS(0x3410).
         */

        val = pci_config_readl(pdev, INTEL_ICH_RCBA);
        base = val & 0xffffc000;
        if (!(val & 0x01)) {
            dev_notice(&pdev->dev, "RCBA is disabled by hardware/BIOS\n");
            return -ENODEV;
        }

        idev->tco_res[TCO_RES_MEM].start = base + INTEL_ICH_RCBA_GCS;
        idev->tco_res[TCO_RES_MEM].size  = INTEL_ICH_RCBA_GCS_SZ;
        idev->tco_res[TCO_RES_MEM].type  = RESOURCE_MMIO;
    } else if (info->itco_version == ich_tco_v3) {
        /*
         * Get the Power Management Configuration register.  To get access
         * to it we have to read the PMC BASE from config space and address
         * the register at offset 0x8.
         */

        intel_ich_enable_pmc(pdev);

        val = pci_config_readl(pdev, INTEL_ICH_PMC);
        base = val & 0xfffffe00;
        idev->tco_res[TCO_RES_MEM].start = base + INTEL_ICH_PMC_BASE;
        idev->tco_res[TCO_RES_MEM].size  = INTEL_ICH_PMC_SZ;
        idev->tco_res[TCO_RES_MEM].type  = RESOURCE_MMIO;
    } else if (info->itco_version != ich_tco_v1) {
        dev_warn(&pdev->dev, "unkown tco type\n");
        return -EINVAL;
    }

    idev->tco.dev.name = "lpt-tco";
    idev->tco.name = info->itco_version;
    idev->tco.resource = idev->tco_res;
    idev->tco.resources_nr = ARRAY_SIZE(idev->tco_res);
    return platform_device_register(&idev->tco);
}

static state intel_ich_gpio_setup(struct pci_device *pdev, const struct ich_chipinfo *info)
{
    struct ich_device *idev = pci_get_devdata(pdev);
    uint32_t val, base;

    /* Setup power management base register. */
    val = pci_config_readl(pdev, INTEL_ICH_APCI);
    base = val & 0xff80;
    if (!base) {
        dev_warn(&pdev->dev, "ACPI I/O uninitialized\n");
        goto gpio_base;
    }

	idev->gpio_res[INTEL_RES_GPIO].start = base + INTEL_ICH_ACPI_GPE;
	idev->gpio_res[INTEL_RES_GPIO].size = INTEL_ICH_ACPI_GPE_SZ;
    intel_ich_enable_acpi(pdev, info);

gpio_base:
    /* Setup GPIO base register. */
	val = pci_config_readl(pdev, idev->gpio_base);
    base = val & 0xff80;
    if (!base) {
        dev_warn(&pdev->dev, "GPIO I/O uninitialized\n");
        return -ENODEV;
    }

    idev->gpio_res[INTEL_RES_GPIO].start = base;

    if (info->gpio_version == ich_gpio_v10cp ||
        info->gpio_version == ich_gpio_v5) {
        idev->gpio_res[INTEL_RES_GPIO].size = 128;
    } else {
        idev->gpio_res[INTEL_RES_GPIO].size = 64;
    }

    intel_ich_enable_gpio(pdev);

    idev->gpio.dev.name = "lpt-gpio";
    idev->gpio.name = info->gpio_version;
    idev->gpio.resource = idev->gpio_res;
    idev->gpio.resources_nr = ARRAY_SIZE(idev->gpio_res);
    return platform_device_register(&idev->gpio);
}

static state intel_ich_spi_setup(struct pci_device *pdev, const struct ich_chipinfo *info)
{
    struct ich_device *idev = pci_get_devdata(pdev);
    uint32_t val, base;

    if (info->spi_version == ich_spi_leg) {
        val = pci_config_readl(pdev, INTEL_ICH_RCBA);
        base = val & 0xffffc000;
        if (!(val & 0x01)) {
            dev_notice(&pdev->dev, "RCBA is disabled by hardware/BIOS\n");
            return -ENODEV;
        }

        idev->spi_res[0].start = base + INTEL_ICH_RCBA_LSPI;
        idev->spi_res[0].size  = INTEL_ICH_RCBA_LSPI_SZ;
        idev->spi_res[0].type  = RESOURCE_MMIO;
    } else if (info->spi_version == ich_spi_ich || info->spi_version == ich_spi_lpt) {
        val = pci_config_readl(pdev, INTEL_ICH_RCBA);
        base = val & 0xffffc000;
        if (!(val & 0x01)) {
            dev_notice(&pdev->dev, "RCBA is disabled by hardware/BIOS\n");
            return -ENODEV;
        }

        idev->spi_res[0].start = base + INTEL_ICH_RCBA_LPT;
        idev->spi_res[0].size  = INTEL_ICH_RCBA_LPT_SZ;
        idev->spi_res[0].type  = RESOURCE_MMIO;
    } else if (info->spi_version == ich_spi_byt) {
        val = pci_config_readl(pdev, INTEL_ICH_BYT);
        base = val & (INTEL_ICH_BYT_SZ - 1);
        if (!(val & 0x01)) {
            dev_notice(&pdev->dev, "BYT is disabled by hardware/BIOS\n");
            return -ENODEV;
        }

        idev->spi_res[0].start = base;
        idev->spi_res[0].size  = INTEL_ICH_BYT_SZ;
        idev->spi_res[0].type  = RESOURCE_MMIO;
    } else if (info->spi_version == ich_spi_bxt) {
        /*
         * The P2SB is hidden by BIOS and we need to unhide it in
         * order to read BAR of the SPI flash device. Once that is
         * done we hide it again.
         */

        pci_bus_config_writeb(pdev->bus, PCI_DEVFN(13, 0), 0xe1, 0);
        pci_bus_config_readl(pdev->bus, PCI_DEVFN(13, 0), PCI_BASE_ADDRESS_0, &val);
        base = val & 0xfffffff0;
        if (val == ~0)
            return -ENODEV;

        idev->spi_res[0].start = base;
        idev->spi_res[0].size  = INTEL_ICH_APL_SZ;
        idev->spi_res[0].type  = RESOURCE_MMIO;

        pci_bus_config_writeb(pdev->bus, PCI_DEVFN(13, 0), 0xe1, 1);
    } else {
        dev_warn(&pdev->dev, "unkown spi type\n");
        return -EINVAL;
    }

    idev->spi.dev.name = "lpt-spi";
    idev->spi.name = info->spi_version;
    idev->spi.resource = idev->spi_res;
    idev->spi.resources_nr = ARRAY_SIZE(idev->spi_res);
    return platform_device_register(&idev->spi);
}

static state intel_ich_probe(struct pci_device *pdev, const void *pdata)
{
    const struct ich_chipinfo *info = pdata;
    struct ich_device *idev;
    state ret1, ret2, ret3;

    ret1 = ret2 = ret3 = -ENOERR;
    dev_info(&pdev->dev, "chipset %s detect\n", info->name);

    idev = dev_kzalloc(&pdev->dev, sizeof(*idev), GFP_KERNEL);
    if (!pdev)
        return -ENOMEM;
    pci_set_devdata(pdev, idev);

    if (info->itco_version == ich_tco_v1) {
        idev->gpio_base = INTEL_ICH0_GBA;
        idev->gctl_base = INTEL_ICH0_GCTL;
    } else {
        idev->gpio_base = INTEL_ICH6_GBA;
        idev->gctl_base = INTEL_ICH6_GCTL;
    }

    if (info->itco_version) {
        ret1 = intel_ich_tco_setup(pdev, info);
        if (ret1)
            pr_warn("tco register failed %d\n", ret1);
    }

    if (info->gpio_version) {
        ret2 = intel_ich_gpio_setup(pdev, info);
        if (ret2)
            pr_warn("gpio register failed %d\n", ret2);
    }

    if (info->spi_version) {
        ret3 = intel_ich_spi_setup(pdev, info);
        if (ret3)
            pr_warn("spi register failed %d\n", ret3);
    }

    return ret1 && ret2 && ret3 ? ret1 : -ENOERR;
}

static void intel_ich_remove(struct pci_device *pdev)
{
    struct ich_device *idev = pci_get_devdata(pdev);

    if (idev->tco_res->start)
        platform_device_unregister(&idev->tco);
    if (idev->gpio_res->start)
        platform_device_unregister(&idev->gpio);
    if (idev->spi_res->size)
        platform_device_unregister(&idev->spi);

    intel_ich_restore_config(pdev);
}

static struct pci_driver intel_ich_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = intel_ich_ids,
    .probe = intel_ich_probe,
    .remove = intel_ich_remove,
};

static state intel_ich_init(void)
{
    return pci_driver_register(&intel_ich_driver);
}
driver_initcall(intel_ich_init);
