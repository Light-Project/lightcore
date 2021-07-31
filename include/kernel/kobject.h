/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _KERNEL_KOBJECT_H_ 
#define _KERNEL_KOBJECT_H_

#include <list.h>
#include <kernel/spinlock.h>

enum kobject_type{
    kobject_type_dir,
    kobject_type_reg,

};
 
struct kobject{
    const char      *name;
    struct kobject  *parent;
    struct list_head list;


    enum kobject_type type;
    spinlock_t      lock;
};



#endif  /* _KERNEL_KOBJECT_H_ */
