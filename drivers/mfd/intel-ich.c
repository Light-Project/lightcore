/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define DRIVER_NAME "intel-ich"
#define pr_fmt(fmt) DRIVER_NAME ": " fmt

#include <string.h>
#include <initcall.h>
#include <driver/pci.h>
#include <driver/gpio/ich.h>
#include <driver/mtd/intel.h>
#include <driver/watchdog/tco.h>
#include <printk.h>

struct ich_device {
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

const char ich_spi_byt[] = INTEL_SPI_BYT_MATCH_ID;
const char ich_spi_lpt[] = INTEL_SPI_LPT_MATCH_ID;
const char ich_spi_bxt[] = INTEL_SPI_BXT_MATCH_ID;
const char ich_spi_cnl[] = INTEL_SPI_CNL_MATCH_ID;

static struct ich_device lpc_baytrail = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_cpt = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_cptd = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_cptm = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_pbg = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ppt = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_avn = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_braswell = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_dh89xxcc = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_coleto = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_cich = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_6300esb = {
    .name = "",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich = {
    .name = "ICH",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich0 = {
    .name = "ICH0",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich2 = {
    .name = "ICH2",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich2m = {
    .name = "ICH2-M",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich3 = {
    .name = "ICH3",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich3m = {
    .name = "ICH3-M",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich4 = {
    .name = "ICH4",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich4m = {
    .name = "ICH4-M",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich5 = {
    .name = "ICH5",
    .itco_version = ich_tco_v1,
};

static struct ich_device lpc_ich6 = {
    .name = "ICH6",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich6m = {
    .name = "ICH6-M",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich6w = {
    .name = "ICH6-W",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_631xesb = {
    .name = "",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich7 = {
    .name = "ICH7",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich7dh = {
    .name = "ICH7-DH",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich7m = {
    .name = "ICH7-M",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_nm10 = {
    .name = "NM10",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich7mdh = {
    .name = "ICH7DH",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich8 = {
    .name = "ICH8",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich8me = {
    .name = "ICH8-ME",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich8dh = {
    .name = "ICH8-DH",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich8do = {
    .name = "ICH8-DO",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich8m = {
    .name = "ICH8-M",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich9 = {
    .name = "ICH9",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich9r = {
    .name = "ICH9-R",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich9dh = {
    .name = "ICH9-DH",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich9do = {
    .name = "ICH9-DO",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich9m = {
    .name = "ICH9-M",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich9me = {
    .name = "ICH9-ME",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich10 = {
    .name = "ICH10",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich10r = {
    .name = "ICH10-R",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_ich10do = {
    .name = "ICH10-DO",
    .itco_version = ich_tco_v2,
};

static struct ich_device lpc_glk = {

};

static struct ich_device lpc_cougarmountain = {

};

static struct ich_device lpc_ich10d = {

};

static struct ich_device lpc_pch = {

};

static struct ich_device lpc_pchm = {

};

static struct ich_device lpc_p55 = {

};

static struct ich_device lpc_pm55 = {

};

static struct ich_device lpc_h55 = {

};

static struct ich_device lpc_qm57 = {

};

static struct ich_device lpc_h57 = {

};

static struct ich_device lpc_hm55 = {

};

static struct ich_device lpc_q57 = {

};

static struct ich_device lpc_hm57 = {

};

static struct ich_device lpc_pchmsff = {

};

static struct ich_device lpc_qs57 = {

};

static struct ich_device lpc_3400 = {

};

static struct ich_device lpc_3420 = {

};

static struct ich_device lpc_3450 = {

};

static struct ich_device lpc_ep80579 = {

};

static struct ich_device lpc_apl = {

};

static struct ich_device lpc_lpt = {

};

static struct ich_device lpc_9s = {

};

static struct ich_device lpc_wbg = {

};

static struct ich_device lpc_lpt_lp = {

};

static struct ich_device lpc_wpt_lp = {

};

static struct ich_device lpc_lewisburg = {

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

static state intel_ich_probe(struct pci_device *pdev, void *pdata)
{
    struct ich_device *ich = pdata;

    dev_info(&pdev->dev, "%s\n", ich->name);

    return -ENOERR;
}

static struct pci_driver intel_ich_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .id_table = intel_ich_ids,
    .probe = intel_ich_probe,
};

static state intel_ich_init(void)
{
    return pci_driver_register(&intel_ich_driver);
}
driver_initcall(intel_ich_init);
