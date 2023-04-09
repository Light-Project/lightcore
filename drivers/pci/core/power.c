/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "pci-power"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <driver/pci.h>
#include <export.h>

static const char *pci_power_name[] = {
    [PCI_POWER_D0]  = "D0",
    [PCI_POWER_D1]  = "D1",
    [PCI_POWER_D2]  = "D2",
    [PCI_POWER_D3H] = "D3hot",
    [PCI_POWER_D3C] = "D3cold",
    [PCI_POWER_ERR] = "error",
    [PCI_POWER_UNKNOWN] = "unknown",
};

static state pci_power_real_set(struct pci_device *pdev, enum pci_power power)
{

    if (power == pdev->power_state)
        return -ENOERR;

    if (power > PCI_POWER_D3C)
        return -EINVAL;

    if ((power == PCI_POWER_D1 && !pdev->power_has_d1) ||
        (power == PCI_POWER_D2 && !pdev->power_has_d2)) {
        pci_err(pdev, "device not support %s\n",
            pci_power_name[pdev->power_state]);
        return -EIO;
    }

    if (power != PCI_POWER_D0 && pdev->power_state > power &&
        pdev->power_state <= PCI_POWER_D3C) {
        pci_err(pdev, "illegal power transition (%s -> %s)\n",
            pci_power_name[pdev->power_state],
            pci_power_name[power]);
        return -EINVAL;
    }

    return -ENOERR;
}

state pci_power_on(struct pci_device *pdev)
{
    return pci_power_real_set(pdev, PCI_POWER_D0);
}
EXPORT_SYMBOL(pci_power_on);

state pci_power_state_set(struct pci_device *pdev, enum pci_power power)
{
    power = clamp(power, PCI_POWER_D0, PCI_POWER_D3C);
    if ((power == PCI_POWER_D1 || power == PCI_POWER_D2) && pci_no_d1d2(pdev))
        return -ENOERR;

    if (pdev->power_state == power)
        return -ENOERR;

    return -ENOERR;
}
EXPORT_SYMBOL(pci_power_state_set);

/**
 * pci_update_current_state - read power state of given device and cache it.
 * @pdev: pci device to handle.
 * @power: state to cache in case the device doesn't have the pm capability.
 */
void pci_power_update_state(struct pci_device *pdev, enum pci_power power)
{
    uint16_t pmcsr;

    if (pdev->pm_cap) {
        pmcsr = pci_config_readw(pdev, pdev->pm_cap + PCI_PM_CTRL);
        pdev->power_state = (pmcsr & PCI_PM_CTRL_STATE_MASK);
    } else {
        pdev->power_state = power;
    }
}
EXPORT_SYMBOL(pci_power_update_state);

void pci_power_setup(struct pci_device *pdev)
{
    uint16_t val;
    uint8_t pos;

    pos = pci_capability_find(pdev, PCI_CAP_ID_PM);
    if (!pos)
        return;

    val = pci_config_readw(pdev, pos + PCI_PM_PMC);
    if ((val & PCI_PM_CAP_VER_MASK) > 3) {
        pci_err(pdev, "unsupported PM cap regs version (%d)\n",
            val & PCI_PM_CAP_VER_MASK);
        return;
    }

    if (!pci_no_d1d2(pdev)) {
        if (val & PCI_PM_CAP_D1)
            pdev->power_has_d1 = true;
        if (val & PCI_PM_CAP_D2)
            pdev->power_has_d2 = true;
        if (pdev->power_has_d1 || pdev->power_has_d2)
            pci_info(pdev, "power supports %s %s\n",
                pdev->power_has_d1 ? "D1" : "",
                pdev->power_has_d2 ? "D2" : "");
    }

    if (val & PCI_PM_CAP_PME_MASK) {
        pci_info(pdev, "PME# supported from %s %s %s %s %s\n",
            (val & PCI_PM_CAP_PME_D0) ? " D0" : "",
            (val & PCI_PM_CAP_PME_D1) ? " D1" : "",
            (val & PCI_PM_CAP_PME_D2) ? " D2" : "",
            (val & PCI_PM_CAP_PME_D3hot) ? " D3H" : "",
            (val & PCI_PM_CAP_PME_D3cold) ? " D3C" : "");
        pdev->pme_support = val >> PCI_PM_CAP_PME_SHIFT;
        pdev->pme_poll = true;
    }

    pdev->pm_cap = pos;
    pdev->d3h_delay = PCI_PM_D3H_WAIT;
    pdev->d3c_delay = PCI_PM_D3C_WAIT;
}
EXPORT_SYMBOL(pci_power_setup);
