/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <initcall.h>
#include <kmalloc.h>
#include <export.h>
#include <printk.h>

LIST_HEAD(kshell_list);
SPIN_LOCK(kshell_lock);
static LIST_HEAD(kshell_env_list);
static SPIN_LOCK(kshell_env_lock);

struct kshell_env {
    struct list_head list;
    char *val, name[];
};

static struct kshell_env *kshell_env_find(const char *name)
{
    struct kshell_env *env, *find = NULL;

    spin_lock(&kshell_env_lock);

    list_for_each_entry(env, &kshell_env_list, list) {
        if (!strcmp(name, env->name)) {
            find = env;
            break;
        }
    }

    spin_unlock(&kshell_env_lock);
    return find;
}

static void kshell_env_release(struct kshell_env *env)
{
    spin_lock(&kshell_env_lock);
    list_del(&env->list);
    spin_unlock(&kshell_env_lock);
    kfree(env);
}

char *kshell_getenv(const char *name)
{
    struct kshell_env *env;

    if (strchr(name, '='))
        return NULL;

    env = kshell_env_find(name);
    if (!env)
        return NULL;

    return env->val;
}
EXPORT_SYMBOL(kshell_getenv);

state kshell_setenv(const char *name, const char *val, bool overwrite)
{
    struct kshell_env *env;
    size_t nlen, vlen;

    env = kshell_env_find(name);
    if (env && !overwrite)
        return -ENOERR;
    else if (env)
        kshell_env_release(env);

    nlen = strlen(name) + 1;
    vlen = strlen(val) + 1;

    env = kmalloc(sizeof(*env) + nlen + vlen, GFP_KERNEL);
    if (!env)
        return -ENOMEM;

    env->val = env->name + nlen;
    strcpy(env->val, val);
    strcpy(env->name, name);

    spin_lock(&kshell_env_lock);
    list_add(&kshell_env_list, &env->list);
    spin_unlock(&kshell_env_lock);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_setenv);

state kshell_putenv(char *string)
{
    char *equal;
    state ret;

    equal = strchr(string, '=');
    if (!equal)
        return -EINVAL;

    *equal = '\0';
    ret = kshell_setenv(string, equal + 1, true);
    *equal = '=';

    return ret;
}
EXPORT_SYMBOL(kshell_putenv);

state kshell_unsetenv(const char *name)
{
    struct kshell_env *env;

    if (strchr(name, '='))
        return -EINVAL;

    env = kshell_env_find(name);
    if (env)
        return -ENODATA;

    kshell_env_release(env);
    return -ENOERR;
}
EXPORT_SYMBOL(kshell_unsetenv);

static int kshell_sort(struct list_head *a, struct list_head *b, void *pdata)
{
    struct kshell_command *ca = list_to_kshell(a);
    struct kshell_command *cb = list_to_kshell(b);
    return *ca->name - *cb->name;
}

struct kshell_command *kshell_find(const char *name)
{
    struct kshell_command *cmd, *find = NULL;

    spin_lock(&kshell_lock);

    list_for_each_entry(cmd, &kshell_list, list) {
        if (!strcmp(name, cmd->name)) {
            find = cmd;
            break;
        }
    }

    spin_unlock(&kshell_lock);
    return find;
}
EXPORT_SYMBOL(kshell_find);

state kshell_register(struct kshell_command *cmd)
{
    if (!cmd->name || !cmd->desc)
        return -EINVAL;

    if (kshell_find(cmd->name)) {
        pr_err("cmd %s already registered", cmd->name);
        return -EINVAL;
    }

    spin_lock(&kshell_lock);
    list_add(&kshell_list, &cmd->list);
    spin_unlock(&kshell_lock);
    return -ENOERR;
}
EXPORT_SYMBOL(kshell_register);

void kshell_unregister(struct kshell_command *cmd)
{
    spin_lock(&kshell_lock);
    list_del(&cmd->list);
    spin_unlock(&kshell_lock);
}
EXPORT_SYMBOL(kshell_unregister);

static void __init ksh_initcall(void)
{
    initcall_entry_t *fn;
    initcall_t call;

    initcall_for_each_fn(fn, kshell_initcall) {
        call = initcall_from_entry(fn);
        if (call())
            pr_err("%s startup failed", fn->name);
    }
}

void ksh_init(void)
{
    printk("##########################\n");
    printk("Welcome to lightcore shell\n");

    ksh_initcall();
    list_sort(&kshell_list, kshell_sort, NULL);

    printk("Have a lot of fun..\n");
    kshell_main(0, NULL);
}
