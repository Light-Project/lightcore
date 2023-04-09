/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include <kboot.h>
#include <size.h>
#include <minmax.h>
#include <asm/regs.h>
#include <arch/x86/msr.h>

#undef CONFIG_PGTABLE_LEVEL5
#include <asm/page.h>
#include <arch/x86/page.h>

struct pgd page_pmd[4][PTRS_PER_PMD] __aligned(0x1000);
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

static void pgd_huge_set(struct pgd *pgd, struct pgd *pud)
{
    pgd->p      = true;
    pgd->rw     = PAGE_RW_RW;
    pgd->us     = PAGE_US_USER;
    pgd->addr   = (uintptr_t)pud >> PAGE_SHIFT;
}

static void pmd_link(struct pgd **pmd, phys_addr_t pa, size_t size)
{
    for (size >>= PMD_SHIFT; size; --size) {
        pmd_huge_set((*pmd)++, pa);
        pa += PMD_SIZE;
    }
}

static void pud_link(struct pgd **pud, struct pgd **pmd, phys_addr_t pa, size_t size)
{
    struct pgd *tpmd;
    size_t tmp;

    for (; (tmp = min(size, PUD_SIZE)); size -= tmp) {
        pmd_link(({tpmd = *pmd; pmd;}), pa, tmp);
        pgd_huge_set((*pud)++, tpmd);
        pa += PUD_SIZE;
    }
}

static void pgd_link(struct pgd **pgd, struct pgd **pud, struct pgd **pmd, phys_addr_t pa, size_t size)
{
    struct pgd *tpud;
    size_t tmp;

    for (; (tmp = min(size, PGDIR_SIZE)); size -= tmp) {
        pud_link(({tpud = *pud; pud;}), pmd, pa, tmp);
        pgd_huge_set((*pgd)++, tpud);
        pa += PGDIR_SIZE;
    }
}

static void section_link(unsigned int pud_index, unsigned int pmd_index,
                         uintptr_t va, phys_addr_t pa, size_t size)
{
    struct pgd *pgd, *pud, *pmd;

    pgd = &page_dir[(va & BIT_LOW_MASK(48)) >> PGDIR_SHIFT];
    pud = &page_pud[pud_index][(va & BIT_LOW_MASK(39)) >> PUD_SHIFT];
    pmd = &page_pmd[pmd_index][(va & BIT_LOW_MASK(30)) >> PMD_SHIFT];

    pgd_link(&pgd, &pud, &pmd, pa, size);
}

void kernel_map(void)
{
    uint64_t val;

    section_link(0, 0, 0x0000000000000000, CONFIG_RAM_BASE, PUD_SIZE * 2);
    section_link(1, 2, CONFIG_PAGE_OFFSET, CONFIG_RAM_BASE, PUD_SIZE * 2);

    val = cr0_get();
    val &= ~(CR0_CD | CR0_NW);
    cr0_set(val);
}
