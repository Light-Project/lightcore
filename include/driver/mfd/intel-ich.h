/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _DRIVER_MFD_INTEL_ICH_H_
#define _DRIVER_MFD_INTEL_ICH_H_

enum intel_ich_registers {
    /* PCI config regsiters */
    INTEL_ICH_APCI          = 0x40,
    INTEL_ICH_PMC           = 0x44,
    INTEL_ICH_BYT           = 0x54,
    INTEL_ICH_RCBA          = 0xf0,

    /* APCI regsiters */
    INTEL_ICH_ACPI_GPE      = 0x28,
    INTEL_ICH_ACPI_GPE_SZ   = 0x08,
    INTEL_ICH_ACPI_SMI      = 0x30,
    INTEL_ICH_ACPI_SMI_SZ   = 0x04,
    INTEL_ICH_ACPI_TCO      = 0x60,
    INTEL_ICH_ACPI_TCO_SZ   = 0x20,

    /* PMC regsiters */
    INTEL_ICH_PMC_BASE      = 0x08,
    INTEL_ICH_PMC_SZ        = 0x02,

    /* BYT regsiters */
    INTEL_ICH_BYT_SZ        = 512,

    /* RCBA regsiters */
    INTEL_ICH_RCBA_GCS      = 0x3410,
    INTEL_ICH_RCBA_GCS_SZ   = 0x0004,
    INTEL_ICH_RCBA_LPT      = 0x3800,
    INTEL_ICH_RCBA_LPT_SZ   = 512,

    INTEL_ICH_APL_SZ        = 0x1000,
};

#endif  /* _DRIVER_MFD_INTEL_ICH_H_ */
