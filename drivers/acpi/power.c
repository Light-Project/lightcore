/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "acpi-power"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <initcall.h>
#include <driver/acpi.h>
#include <driver/pci.h>
#include <driver/power.h>
#include <delay.h>

#ifdef CONFIG_PCI
static void acpi_pci_reset(struct acpi_generic_address *rr, uint8_t value)
{
    unsigned int devfn, reg;
    devfn = PCI_DEVFN((rr->Address >> 32) & 0xffff,
            (rr->Address >> 16) & 0xffff);
    reg = rr->Address & 0xffff;
    pci_raw_config_write(0, 0, devfn, reg, 1, value);
}
#endif

static void acpi_reset(struct power_device *pdev)
{
    ACPI_GENERIC_ADDRESS *rr = &AcpiGbl_FADT.ResetRegister;

    /* ACPI reset register was only introduced with v2 of the FADT */
    if (AcpiGbl_FADT.Header.Revision < 2) {
        pr_notice("version not support reset reg\n");
        return;
    }

    /*
     * Is the reset register supported? The spec says we should be
     * checking the bit width and bit offset, but Windows ignores
     * these fields
     */
    if (!(AcpiGbl_FADT.Flags & ACPI_FADT_RESET_REGISTER)) {
        pr_notice("not support reset reg\n");
        return;
    }

    switch (rr->SpaceId) {
#ifdef CONFIG_PCI
        case ACPI_ADR_SPACE_PCI_CONFIG:
            pr_crit("pci reg reset\n");
            acpi_pci_reset(rr, AcpiGbl_FADT.ResetValue);
            break;
#endif

        case ACPI_ADR_SPACE_SYSTEM_MEMORY:
        case ACPI_ADR_SPACE_SYSTEM_IO:
            pr_crit("memory i/o reg reset\n");
            AcpiReset();
            break;

        default:
            break;
    }

    mdelay(15);
}

static bool acpi_sleep_state_supported(uint8_t state)
{
    ACPI_STATUS status;
    uint8_t typea, typeb;

    status = AcpiGetSleepTypeData(state, &typea, &typeb);
    return ACPI_SUCCESS(status) && (!AcpiGbl_ReducedHardware ||
        (AcpiGbl_FADT.SleepControl.Address && AcpiGbl_FADT.SleepStatus.Address));
}

static void acpi_sleep_prepare(uint8_t acpi_state)
{
    AcpiDisableAllGpes();
    AcpiOsWaitEventsComplete();
}

static void acpi_shutdown(struct power_device *pdev)
{
    irq_local_disable();
    acpi_sleep_prepare(ACPI_STATE_S5);
    AcpiEnterSleepState(ACPI_STATE_S5);
}

static struct device acpi_power = {
    .name = MODULE_NAME,
};

static struct power_ops acpi_power_ops = {
    .reset = acpi_reset,
    .shutdown = acpi_shutdown,
};

static struct power_device acpi_power_device = {
    .dev = &acpi_power,
    .ops = &acpi_power_ops,
};

static state __init acpi_power_init(void)
{
    state retval;

    if (!acpi_sleep_state_supported(ACPI_STATE_S5)) {
        pr_notice("not support s5 state\n");
        return -ENODEV;
    }

    retval = device_register(&acpi_power);
    if (retval)
        return retval;

    return power_register(&acpi_power_device);
}
driver_initcall(acpi_power_init);
