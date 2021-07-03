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


void *kmem_virt_alloc()
{

    
}

void *kmem_vmalloc(size_t size)
{



}

/**
 * External function
 */

void vmem_init()
{


}

state vmem_read(uint8_t *buff, uint8_t *addr, size_t size)
{
    if((buff == NULL)||(addr == NULL)||(size == 0))
        return ERROR;

    return OK;
}

state vmem_write(uint8_t *buff, uint8_t *addr, size_t size)
{
    if((buff == NULL)||(addr == NULL)||(size == 0))
        return ERROR;

    
    return OK;
}

void *vmalloc(size_t size)
{
    if(size == 0)
        return NULL;


}

state vfree(void *mem)
{
    if(mem == NULL)
        return ERROR;

}
