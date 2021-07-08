/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  lightcore/mm/vmem.c
 *  light core virt mem manager
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-20      first version 
 * 
 */

#include <mm.h>
#include <types.h>
#include <stddef.h>
#include <printk.h>
#include <list.h>
#include <mm/vmap.h>
#include <system/spinlock.h>
#include <state.h>
#include <system.h>
#include <export.h>
#include <debug.h>

void *vmalloc(size_t size)
{
    if(size == 0)
        return NULL;


}

void vfree(void *mem)
{
    if(mem == NULL)
        return ERROR;

}
