/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * osd.c - OS-dependent functions
 */

#include <driver/acpi.h>
#include "accommon.h"
#include "acnamesp.h"

#include <asm/io.h>

/*
 * ACPI Table interfaces
 */

ACPI_PHYSICAL_ADDRESS
AcpiOsGetRootPointer (
    void)
{
    // ACPI_PHYSICAL_ADDRESS *Pa;

    return 0;   
}

ACPI_STATUS
AcpiOsPredefinedOverride (
    const ACPI_PREDEFINED_NAMES *InitVal,
    ACPI_STRING                 *NewVal)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_TABLE_HEADER       **NewTable)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsPhysicalTableOverride (
    ACPI_TABLE_HEADER       *ExistingTable,
    ACPI_PHYSICAL_ADDRESS   *NewAddress,
    UINT32                  *NewTableLength)
{
    return (AE_OK);
}

/*
 * Spinlock primitives
 */
void
AcpiOsDeleteLock (
    ACPI_SPINLOCK           Handle)
{

}

ACPI_CPU_FLAGS
AcpiOsAcquireLock (
    ACPI_SPINLOCK           Handle)
{
    return 0;   
}

void
AcpiOsReleaseLock (
    ACPI_SPINLOCK           Handle,
    ACPI_CPU_FLAGS          Flags)
{

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

    return (AE_OK);
}

ACPI_STATUS
AcpiOsDeleteSemaphore (
    ACPI_HANDLE         Handle)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsWaitSemaphore (
    ACPI_HANDLE         Handle,
    UINT32              Units,
    UINT16              Timeout)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsSignalSemaphore (
    ACPI_HANDLE         Handle,
    UINT32              Units)
{
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

    return NULL;
}

void
AcpiOsUnmapMemory (
    void                    *LogicalAddress,
    ACPI_SIZE               Size)
{

}

ACPI_STATUS
AcpiOsGetPhysicalAddress (
    void                    *LogicalAddress,
    ACPI_PHYSICAL_ADDRESS   *PhysicalAddress)
{
    return (AE_OK);
}

/*
 * Memory/Object Cache
 */
#ifndef ACPI_USE_LOCAL_CACHE
ACPI_STATUS
AcpiOsCreateCache (
    char                    *CacheName,
    UINT16                  ObjectSize,
    UINT16                  MaxDepth,
    ACPI_CACHE_T            **ReturnCache)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsDeleteCache (
    ACPI_CACHE_T            *Cache)
{
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
    return (AE_OK);
}
#endif /* ACPI_USE_LOCAL_CACHE */

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

}

void
AcpiOsSleep (
    UINT64                  Milliseconds)
{

}

void
AcpiOsStall (
    UINT32                  Microseconds)
{

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
    return (AE_OK);
}

ACPI_STATUS
AcpiOsWriteMemory (
    ACPI_PHYSICAL_ADDRESS   Address,
    UINT64                  Value,
    UINT32                  Width)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsReadPciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  *Value,
    UINT32                  Width)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsWritePciConfiguration (
    ACPI_PCI_ID             *PciId,
    UINT32                  Reg,
    UINT64                  Value,
    UINT32                  Width)
{
    return (AE_OK);
}


/*
 * Miscellaneous
 */
UINT64
AcpiOsGetTimer (
    void)
{
    return 0;
}

ACPI_STATUS
AcpiOsSignal (
    UINT32                  Function,
    void                    *Info)
{
    return (AE_OK);
}

ACPI_STATUS
AcpiOsEnterSleep (
    UINT8                   SleepState,
    UINT32                  RegaValue,
    UINT32                  RegbValue)
{
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
    static char buffer[512];

    vsnprintf(buffer, sizeof(buffer), Format, Args);

    printk("%s", buffer);
}

/*
 * Debug IO
 */
ACPI_STATUS ACPI_INIT_FUNCTION 
AcpiOsInitialize (
    void)
{


    return (AE_OK);
}

ACPI_STATUS 
AcpiOsTerminate (
    void)
{

    return (AE_OK);
}
