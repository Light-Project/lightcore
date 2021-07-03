#include <system/sched.h>

#include <asm/proc.h>

/**
 * context_switch - switch context form prev to next
 * @prev: switch form
 * @next: switch to
 * 
 */
void context_switch(struct task *prev, struct task *next)
{


    /* yep, kernel thread has no alone mem space. */
    if(!next->mm) {     /* switch to kernel */

    } else {            /* switch to user */


    }

    /* it's ready to switch now. */
    arch_switch(prev, next);


}

void sched_relax(void)
{

    
}

