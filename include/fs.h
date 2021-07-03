#ifndef _VFL_H_
#define _VFL_H_

#include <bits.h>
#include <types.h>
#include <state.h>
#include <list.h>

typedef enum fmode {
    FMODE_RDONLY    = BIT(0),
    FMODE_WRONLY    = BIT(1),
    FMODE_RW        = FMODE_RDONLY | FMODE_WRONLY,

    FMODE_LSEEK     = BIT(3),
    FMODE_PREAD     = BIT(4),
    FMODE_EXEC      = BIT(5),
} fmode_t;

struct file_ops {


    int (*read);
    int (*write);
    int (*sync);
    int (*readdir);
    int (*rename);
    int (*remove);
    int (*mkdir);
    int (*rmdir);
    int (*chmod);

};

struct super_ops {


    int (*read);
    int (*write);
    int (*sync);
    int (*readdir);
    int (*rename);
    int (*remove);
    int (*mkdir);
    int (*rmdir);
    int (*chmod);

};

state kernel_execve(const char *file, const char *const *argv, 
                const char *const *envp);


#endif
