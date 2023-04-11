/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

#define STACK_CACHE_NR CONFIG_STACK_CACHE_NR
static struct ncache *stack_cache;

void task_stack_magic(struct sched_task *task)
{
    uint32_t *magic = stack_end(task->stack);
    *magic = KMAGIC;
}
EXPORT_SYMBOL(task_stack_magic);

bool task_stack_check(struct sched_task *task)
{
    uint32_t *magic = task->stack;
    return *magic == KMAGIC;
}
EXPORT_SYMBOL(task_stack_check);

static void *task_stack_ncache_alloc(void *pdata)
{
#ifdef CONFIG_STACK_VIRTUAL
    return vmalloc(THREAD_SIZE);
#else
    return get_free_pages(THREAD_ORDER, GFP_ACCOUNT);
#endif
}

static void task_stack_ncache_free(void *stack, void *pdata)
{
#ifdef CONFIG_STACK_VIRTUAL
    vfree(stack);
#else
    free_page(stack);
#endif
}

void *task_stack_alloc(void)
{
    void *stack = ncache_get(stack_cache);

    if (unlikely(!stack))
        return NULL;

#ifndef STACK_CLEAR
    memset(stack, 0, THREAD_SIZE);
#endif

    return stack;
}
EXPORT_SYMBOL(task_stack_alloc);

void task_stack_free(void *stack)
{
    ncache_put(stack_cache, stack);
}
EXPORT_SYMBOL(task_stack_free);

static __init void clone_stack_init(void)
{
    stack_cache = ncache_create(
        task_stack_ncache_alloc, task_stack_ncache_free,
        STACK_CACHE_NR, NCACHE_PANIC, NULL
    );
}
