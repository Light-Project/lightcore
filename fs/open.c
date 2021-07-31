/* SPDX-License-Identifier: GPL-2.0-or-later */
#include <fs.h>
#include <kernel/syscall.h>

struct file *vfl_open(const char *name, int flags, umode_t mode)
{
    
}
EXPORT_SYMBOL(vfl_open);

asmlinkage long sys_open(const char *filename, int flags, umode_t mode)
{

}
