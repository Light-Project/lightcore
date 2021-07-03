#include <stddef.h>         /* For NULL */
#include <printk.h>         /* For printk */
#include <system.h>  /* For panic */
#include <fs.h>             /*  */
#include "private.h" 

#define init_max_args   2
#define init_max_envs   3

static const char *init_argv[init_max_args] = {
    "init", NULL,
};
static const char *init_envp[init_max_envs] = {
    "HOME=/", NULL,
};

static bool init_run(const char *file)
{
    pr_info("Run %s as init process\n\r", file);
    return kernel_execve(file, init_argv, init_envp);
}


void init_task(void)
{
    bool ret;

    ret = init_run("/bin/init");
    if(ret)
        return;
    
    panic("No init executable found.");
}
 
