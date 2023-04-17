/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _MM_PAGEWALK_H_
#define _MM_PAGEWALK_H_

#include <types.h>
#include <stddef.h>
#include <mm/types.h>
#include <mm/pgtable.h>

enum pagewalk_action {
    PAGEWALK_ACT_SUBTREE,
    PAGEWALK_ACT_CONTINUE,
    PAGEWALK_ACT_AGAIN,
};

struct pagewalk {
    struct pagewalk_ops *ops;
    enum pgtbl_modified modified;
    void *pdata;
};

struct pagewalk_ops {
    /* present page */
    state (*pte_entry)(struct pagewalk *walk, pte_t *pte, uintptr_t addr,
                       size_t size, enum pagewalk_action *action);
    state (*pmd_entry)(struct pagewalk *walk, pmd_t *pmd, uintptr_t addr,
                       size_t size, enum pagewalk_action *action);
    state (*pud_entry)(struct pagewalk *walk, pud_t *pud, uintptr_t addr,
                       size_t size, enum pagewalk_action *action);
    state (*p4d_entry)(struct pagewalk *walk, p4d_t *p4d, uintptr_t addr,
                       size_t size, enum pagewalk_action *action);
    state (*pgd_entry)(struct pagewalk *walk, pgd_t *pgd, uintptr_t addr,
                       size_t size, enum pagewalk_action *action);

    /* not present page */
    state (*pte_hole)(struct pagewalk *walk, pte_t *pte, uintptr_t addr,
                      size_t size, enum pagewalk_action *action);
    state (*pmd_hole)(struct pagewalk *walk, pmd_t *pmd, uintptr_t addr,
                      size_t size, enum pagewalk_action *action);
    state (*pud_hole)(struct pagewalk *walk, pud_t *pud, uintptr_t addr,
                      size_t size, enum pagewalk_action *action);
    state (*p4d_hole)(struct pagewalk *walk, p4d_t *p4d, uintptr_t addr,
                      size_t size, enum pagewalk_action *action);
    state (*pgd_hole)(struct pagewalk *walk, pgd_t *pgd, uintptr_t addr,
                      size_t size, enum pagewalk_action *action);
};

extern state pagewalk_pte(struct pagewalk *walk, pmd_t *pmd, uintptr_t addr, size_t size);
extern state pagewalk_pmd(struct pagewalk *walk, pud_t *pud, uintptr_t addr, size_t size);
extern state pagewalk_pud(struct pagewalk *walk, p4d_t *p4d, uintptr_t addr, size_t size);
extern state pagewalk_p4d(struct pagewalk *walk, pgd_t *pgd, uintptr_t addr, size_t size);
extern state pagewalk_pgd(struct pagewalk *walk, struct memory *mem, uintptr_t addr, size_t size);

#endif /* _MM_PAGEWALK_H_ */
