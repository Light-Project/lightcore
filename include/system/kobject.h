/*
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  include/system/kobject.h
 *  generic kernel object infrastructure 
 *  (C) 2020 wzl
 * 
 *  Change Logs:
 *  Date            Notes
 *  2021-01-20      first version 
 * 
 */

#ifndef _SYSTEM_KOBJECT_H_ 
#define _SYSTEM_KOBJECT_H_

#include <list.h>
#include <system/spinlock.h>
#include <system/kref.h>

typedef struct kset     kset_t;
typedef struct kobject  kobject_t;

typedef enum kobject_type{
    kobject_type_dir,
    kobject_type_reg,

} kobject_type_t;
 
struct kobject{
    const char      *name;      // Object name or directory name
    list_t          list;       // Object list
    kobject_t       *parent;    // Upper level directory structure
    kobject_type_t  type; 
    spinlock_t      lock;
    kset_t          *kset;
    kref_t          kref;

};

struct kset {
    list_t          list;
    spinlock_t      list_lock;
    kobject_t       kobj;
};

#endif
