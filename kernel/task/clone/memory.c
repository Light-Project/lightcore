/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2023 John Sanpe <sanpeqf@gmail.com>
 */

static struct kcache *memory_cache;

static struct memory *task_memory_init(struct memory *mm)
{
    return mm;
}

static void task_memory_remove(struct memory *mm)
{

}

struct memory *task_memory_alloc(void)
{
    struct memory *mem;

    mem = kcache_zalloc(memory_cache, GFP_KERNEL);
    if (unlikely(!mem))
        return NULL;

    return task_memory_init(mem);
}
EXPORT_SYMBOL(task_memory_alloc);

void task_memory_free(struct memory *mem)
{
    if (memory_put(mem))
        task_memory_remove(mem);
}
EXPORT_SYMBOL(task_memory_free);

static state task_copy_memory(struct task_clone_args *args, struct sched_task *child)
{
    enum clone_flags flags = args->flags;
    struct memory *mem, *omem;

    /* clone a kernel thread */
    if (!(omem = current->mem)) {
        child->mem = NULL;
        child->active_mem = NULL;
        return -ENOERR;
    }

    if (flags & CLONE_VM) {
        memory_get(omem);
        mem = omem;
    } else {
        mem = kcache_zalloc(memory_cache, GFP_KERNEL);
        if (!mem)
            return -ENOMEM;

        memcpy(mem, omem, sizeof(*mem));
    }

    child->mem = mem;
    child->active_mem = mem;

    return -ENOERR;
}

static __init void clone_memory_init(void)
{
    memory_cache = KCACHE_CREATE(struct memory, KCACHE_PANIC);
}
