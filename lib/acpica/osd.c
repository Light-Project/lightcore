/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "acpi-osd"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <string.h>
#include <memory.h>
#include <kmalloc.h>
#include <ioremap.h>
#include <delay.h>
#include <spinlock.h>
#include <driver/acpi.h>
#include <driver/pci.h>
#include <asm/io.h>
#include <printk.h>

#include "accommon.h"
#include "acnamesp.h"

/*
 * ACPI Table interfaces
 */

ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (
    void)
{
    ACPI_PHYSICAL_ADDRESS PhyAdd;

    AcpiFindRootPointer(&PhyAdd);

    return PhyAdd;
}

#define ACPI_MAX_OVERRIDE_LEN 100
static bool acpi_rev_override;
static char acpi_os_name[ACPI_MAX_OVERRIDE_LEN];

ACPI_STATUS
AcpiOsPredefinedOverride (
    const ACPI_PREDEFINED_NAMES *InitVal,
    ACPI_STRING                 *NewVal)
{
    if (!InitVal || !NewVal)
        return (AE_BAD_PARAMETER);

    *NewVal = NULL;

    if (!memcmp(InitVal->Name, "_OS_", 4) && strlen(acpi_os_name)) {
        pr_info("Overriding _OS definition to '%s'\n", acpi_os_name);
        *NewVal = acpi_os_name;
    }

    if (!memcmp(InitVal->Name, "_REV", 4) && acpi_rev_override) {
        pr_info("Overriding _REV return value to 5\n");
        *NewVal = (char *)5;
    }

    return (AE_OK);
}

ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable)
{
    if (!ExistingTable || !NewTable)
        return (AE_BAD_PARAMETER);

    *NewTable = NULL;
    return (AE_OK);
}

ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength)
{
    *NewAddress = 0;
    *NewTableLength = 0;
    return AE_OK;
}

/*
 * Spinlock primitives
 */

void
AcpiOsDeleteLock (
    ACPI_SPINLOCK           Handle)
{
    ACPI_FREE(Handle);
}

ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK           Handle)
{
	irqflags_t flags;
	spin_lock_irqsave(Handle, &flags);
	return flags;
}

void
AcpiOsReleaseLock (
    ACPI_SPINLOCK           Handle,
    ACPI_CPU_FLAGS          Flags)
{
	spin_unlock_irqrestore(Handle, &Flags);
}

/*
 * Semaphore primitives
 */

ACPI_STATUS
AcpiOsCreateSemaphore(
    UINT32          max_units,
    UINT32          initial_units,
    ACPI_HANDLE     *handle)
{
    printk("AcpiOsCreateSemaphore\n");
    return (AE_OK);
}

ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_HANDLE         Handle)
{
    printk("AcpiOsDeleteSemaphore\n");
    return (AE_OK);
}

ACPI_STATUS
AcpiOsWaitSemaphore (
    ACPI_HANDLE         Handle,
    UINT32              Units,
    UINT16              Timeout)
{
    printk("AcpiOsWaitSemaphore\n");
    return (AE_OK);
}

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_HANDLE         Handle,
    UINT32              Units)
{
    printk("AcpiOsSignalSemaphore\n");
    return (AE_OK);
}

/*
 * Memory allocation and mapping
 */

void *
AcpiOsMapMemory (
    ACPI_PHYSICAL_ADDRESS   Where,
    ACPI_SIZE               Length)
{
    return ioremap(Where, Length);
}

void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size)
{
    iounmap(LogicalAddress);
}

ACPI_STATUS
AcpiOsGetPhysicalAddress (
    void                    *LogicalAddress,
    ACPI_PHYSICAL_ADDRESS   *PhysicalAddress)
{
    if (!LogicalAddress || !PhysicalAddress)
        return (AE_BAD_PARAMETER);

    *PhysicalAddress = va_to_pa(LogicalAddress);

    return (AE_OK);
}

/*
 * Memory/Object Cache
 */

ACPI_STATUS
AcpiOsCreateCache (
    char                    *CacheName,
    UINT16                  ObjectSize,
    UINT16                  MaxDepth,
    ACPI_CACHE_T            **ReturnCache)
{
    *ReturnCache = kcache_create(CacheName, ObjectSize, 0);
    return *ReturnCache ? (AE_OK) : (AE_ERROR);
}

ACPI_STATUS
AcpiOsDeleteCache (
    ACPI_CACHE_T            *Cache)
{
    kcache_release(Cache);
    return (AE_OK);
}

ACPI_STATUS
AcpiOsPurgeCache (
    ACPI_CACHE_T            *Cache)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsReleaseObject (
    ACPI_CACHE_T            *Cache,
    void                    *Object)
{
    kcache_free(Cache, Object);
    return (AE_OK);
}

/*
 * Interrupt handlers
 */

ACPI_STATUS
AcpiOsInstallInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine,
    void                    *Context)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsRemoveInterruptHandler (
    UINT32                  InterruptNumber,
    ACPI_OSD_HANDLER        ServiceRoutine)
{
    return (AE_OK);
}

/*
 * Threads and Scheduling
 */

ACPI_STATUS
AcpiOsExecute (
    ACPI_EXECUTE_TYPE       Type,
    ACPI_OSD_EXEC_CALLBACK  Function,
    void                    *Context)
{
    return (AE_OK);
}

void
AcpiOsWaitEventsComplete (
    void)
{
    printk("AcpiOsWaitEventsComplete\n");
}

void
AcpiOsSleep (
    UINT64                  Milliseconds)
{
    msleep(Milliseconds);
}

void
AcpiOsStall (
    UINT32                  Microseconds)
{
    mdelay(Microseconds);
}

/*
 * Platform and hardware-independent I/O interfaces
 */

ACPI_STATUS
AcpiOsReadPort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  *Value,
    UINT32                  Width)
{
    if (Width <= 8)
        *Value = inb(Address);
    else if (Width <= 16)
        *Value = inw(Address);
    else if (Width <= 32)
        *Value = inl(Address);
    else
        return (AE_ERROR);

    return (AE_OK);
}

ACPI_STATUS
AcpiOsWritePort (
    ACPI_IO_ADDRESS         Address,
    UINT32                  Value,
    UINT32                  Width)
{
    if (Width <= 8)
        outb(Address, Value);
    else if (Width <= 16)
        outw(Address, Value);
    else if (Width <= 32)
        outl(Address, Value);
    else
        return (AE_ERROR);

    return (AE_OK);
}

/*
 * Platform and hardware-independent physical memory interfaces
 */

ACPI_STATUS
AcpiOsReadMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  *Value,
    UINT32                  Width)
{
    void *virt;

    virt = ioremap(Address, Width);
    if (!virt)
        return (AE_ERROR);

    if (Width <= 8)
        *Value = readb(virt);
    else if (Width <= 16)
        *Value = readw(virt);
    else if (Width <= 32)
        *Value = readl(virt);
    else if (Width <= 64)
        *Value = readq(virt);
    else
        *Value = ~0ULL;

    iounmap(virt);
    return (AE_OK);
}

ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width)
{
    void *virt;

    virt = ioremap(Address, Width);
    if (!virt)
        return (AE_ERROR);

    if (Width <= 8)
        writeb(virt, Value);
    else if (Width <= 16)
        writew(virt, Value);
    else if (Width <= 32)
        writel(virt, Value);
    else if (Width <= 64)
        writeq(virt, Value);

    iounmap(virt);
    return (AE_OK);
}

ACPI_STATUS
AcpiOsReadPciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  *Value,
    UINT32                  Width)
{
    state retval;

    if (!Value)
        return (AE_BAD_PARAMETER);

    retval = pci_raw_config_read(
        PciId->Segment,
        PciId->Bus,
        PCI_DEVFN(PciId->Device, PciId->Function),
        Reg, Width / 8, (uint32_t *)Value
    );

    return retval ? (AE_ERROR) : (AE_OK);
}

ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  Value,
    UINT32                  Width)
{
    state retval;

    retval = pci_raw_config_write(
        PciId->Segment,
        PciId->Bus,
        PCI_DEVFN(PciId->Device, PciId->Function),
        Reg, Width / 8, (uint32_t)Value
    );

    return retval ? (AE_ERROR) : (AE_OK);
}

/*
 * Miscellaneous
 */

UINT64
AcpiOsGetTimer (
    void)
{
    printk("AcpiOsGetTimer\n");
    return 0;
}

ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info)
{
    printk("AcpiOsSignal\n");
    return (AE_OK);
}

ACPI_STATUS
AcpiOsEnterSleep (
    UINT8                   SleepState,
    UINT32                  RegaValue,
    UINT32                  RegbValue)
{
    printk("AcpiOsEnterSleep\n");
    return (AE_OK);
}

/*
 * Debug print routines
 */
ACPI_PRINTF_LIKE (1)
void ACPI_INTERNAL_VAR_XFACE
AcpiOsPrintf (
    const char              *Format,
    ...)
{
    va_list Args;

    va_start(Args, Format);
    AcpiOsVprintf(Format, Args);
    va_end(Args);
}

void
AcpiOsVprintf (
    const char              *Format,
    va_list                 Args)
{
    vprintk(Format, Args);
}

/*
 * Debug IO
 */
ACPI_STATUS ACPI_INIT_FUNCTION
AcpiOsInitialize (
    void)
{
    printk("AcpiOsInitialize\n");
    return (AE_OK);
}

ACPI_STATUS
AcpiOsTerminate (
    void)
{
    printk("AcpiOsTerminate\n");
    return (AE_OK);
}
