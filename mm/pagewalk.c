/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#include <mm/pagewalk.h>
#include <crash.h>
#include <export.h>

static bool level_subtree(struct pagewalk *walk, unsigned int level)
{
    struct pagewalk_ops *ops = walk->ops;

    switch (level) {
        case 0:
            if (ops->p4d_entry || ops->p4d_hole)
                return true;
            fallthrough;

        case 1:
            if (ops->pud_entry || ops->pud_hole)
                return true;
            fallthrough;

        case 2:
            if (ops->pmd_entry || ops->pmd_hole)
                return true;
            fallthrough;

        case 3:
            if (ops->pte_entry || ops->pte_hole)
                return true;
            fallthrough;

        default:
            BUG();
    }

    return false;
}

state pagewalk_pte(struct pagewalk *walk, pmd_t *pmd, uintptr_t addr, size_t size)
{
    struct pagewalk_ops *ops = walk->ops;
    enum pagewalk_action action;
    pte_t *pte;
    state retval = -ENOERR;

    pte = pte_offset(pmd, addr);
    for (; size; addr += PAGE_SIZE, size -= PAGE_SIZE) {

again:
        action = PAGEWALK_ACT_CONTINUE;
        if (pte_none(pte)) {
            if (ops->pte_hole)
                retval = ops->pte_hole(walk, pte, addr, size, &action);
        } else if (ops->pte_entry)
            retval = ops->pte_entry(walk, pte, addr, size, &action);

        if (unlikely(retval))
            break;

        switch (action) {
            case PAGEWALK_ACT_CONTINUE:
                continue;

            case PAGEWALK_ACT_AGAIN:
                goto again;

            default:
                retval = -EINVAL;
                goto finish;
        }

        pte++;
    }

finish:
    return retval;
}
EXPORT_SYMBOL(pagewalk_pte);

#define GENERIC_PAGEWALK_OPS(type, upper, uptype, upchild, lower, level)        \
state pagewalk_##type##_generic                                                 \
(struct pagewalk *walk, uptype *upper, uintptr_t addr, size_t size)             \
{                                                                               \
    struct pagewalk_ops *ops = walk->ops;                                       \
    enum pagewalk_action action;                                                \
    size_t bound;                                                               \
    type##_t *type;                                                             \
    state retval = -ENOERR;                                                     \
                                                                                \
    type = type##_offset(upchild, addr);                                        \
    for (; size; addr += bound, size -= bound) {                                \
        bound = type##_bound_size(addr, size);                                  \
                                                                                \
again:                                                                          \
        action = PAGEWALK_ACT_SUBTREE;                                          \
        if (type##_none(type)) {                                                \
            if (ops->type##_hole)                                               \
                retval = ops->type##_hole(walk, type, addr, size, &action);     \
        } else if (ops->type##_entry)                                           \
            retval = ops->type##_entry(walk, type, addr, size, &action);        \
                                                                                \
        if (unlikely(retval))                                                   \
            break;                                                              \
                                                                                \
        switch (action) {                                                       \
            case PAGEWALK_ACT_SUBTREE:                                          \
                if (!type##_get_huge(type) && type##_present(type) &&           \
                    level_subtree(walk, level))                                 \
                    break;                                                      \
                fallthrough;                                                    \
                                                                                \
            case PAGEWALK_ACT_CONTINUE:                                         \
                continue;                                                       \
                                                                                \
            case PAGEWALK_ACT_AGAIN:                                            \
                goto again;                                                     \
                                                                                \
            default:                                                            \
                retval = -EINVAL;                                               \
                goto finish;                                                    \
        }                                                                       \
                                                                                \
        retval = pagewalk_##lower(walk, type, addr, size);                      \
        if (unlikely(retval))                                                   \
            break;                                                              \
                                                                                \
        type++;                                                                 \
    }                                                                           \
                                                                                \
finish:                                                                         \
    return retval;                                                              \
}

#define pagewalk_pmd pagewalk_pmd_generic
#define pagewalk_pud pagewalk_pud_generic
#define pagewalk_p4d pagewalk_p4d_generic
#define pagewalk_pgd pagewalk_pgd_generic

GENERIC_PAGEWALK_OPS(pmd, pud, pud_t, pud, pte, 3)
GENERIC_PAGEWALK_OPS(pud, p4d, p4d_t, p4d, pmd, 2)
GENERIC_PAGEWALK_OPS(p4d, pgd, pgd_t, pgd, pud, 1)
GENERIC_PAGEWALK_OPS(pgd, mem, struct memory, mem->pgdir, p4d, 0)

EXPORT_SYMBOL(pagewalk_pmd);
EXPORT_SYMBOL(pagewalk_pud);
EXPORT_SYMBOL(pagewalk_p4d);
EXPORT_SYMBOL(pagewalk_pgd);
