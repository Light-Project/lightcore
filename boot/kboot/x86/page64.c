/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <size.h>
#include <asm/regs.h>
#include <arch/x86/msr.h>

#undef CONFIG_PGTABLE_LEVEL5
#include <asm/page.h>
#include <arch/x86/page.h>

struct pgd page_pmd[2][PTRS_PER_PMD] __aligned(0x1000);
struct pgd page_pud[2][PTRS_PER_PUD] __aligned(0x1000);
struct pgd page_dir[PTRS_PER_PGD] __aligned(0x1000);

static void pmd_huge_set(struct pgd *pmd, phys_addr_t pa)
{
    pmd->p      = true;
    pmd->rw     = PAGE_RW_RW;
    pmd->ps     = PAGE_PS_HUGE;
    pmd->g      = true;
    pmd->addrl  = pa >> PMD_SHIFT;
}

static void pmd_link(struct pgd *pmd, phys_addr_t pa, size_t size)
{
    for (size >>= PMD_SHIFT; size; --size) {
        pmd_huge_set(pmd++, pa);
        pa += PMD_SIZE;
    }
}

static void section_link(unsigned int index, uintptr_t va, phys_addr_t pa)
{
    struct pgd *pgd = &page_dir[(va & BIT_LOW_MASK(48)) >> PGDIR_SHIFT];
    struct pgd *pud = &page_pud[index][(va & BIT_LOW_MASK(39)) >> PUD_SHIFT];

    pgd->p    = true;
    pgd->rw   = PAGE_RW_RW;
    pgd->us   = PAGE_US_USER;
    pgd->addr = (uintptr_t)pud >> PAGE_SHIFT;

    pud->p    = true;
    pud->rw   = PAGE_RW_RW;
    pud->us   = PAGE_US_USER;
    pud->addr = (uintptr_t)page_pmd[index] >> PAGE_SHIFT;

    pmd_link(page_pmd[index], pa, PUD_SIZE);
}

void kernel_map(void)
{
    uint64_t val;

    section_link(0, 0x0000000000000000, CONFIG_RAM_BASE);
    section_link(1, CONFIG_PAGE_OFFSET, CONFIG_RAM_BASE);

    val = cr0_get();
    val &= ~(CR0_CD | CR0_NW);
    cr0_set(val);
}
