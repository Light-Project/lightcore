/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>
#include <export.h>
#include <initcall.h>

static long env_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct kshell_env *enva = env_to_kshell(rba);
    const struct kshell_env *envb = env_to_kshell(rbb);
    return strcmp(enva->name, envb->name);
}

static long env_rb_find(const struct rb_node *rb, const void *name)
{
    const struct kshell_env *env = env_to_kshell(rb);
    return strcmp(name, env->name);
}

static struct kshell_env *kshell_env_find(struct kshell_context *ctx, const char *name)
{
    const struct rb_node *rb;
    rb = rb_find(&ctx->env, name, env_rb_find);
    return rb ? env_to_kshell(rb) : NULL;
}

static void kshell_env_release(struct kshell_context *ctx, struct kshell_env *env)
{
    rb_delete(&ctx->env, &env->node);
    kfree(env);
}

char *kshell_getenv(struct kshell_context *ctx, const char *name)
{
    struct kshell_env *env;

    if (strchr(name, '='))
        return NULL;

    env = kshell_env_find(ctx, name);
    if (!env)
        return NULL;

    return env->val;
}
EXPORT_SYMBOL(kshell_getenv);

state kshell_setenv(struct kshell_context *ctx, const char *name,
                    const char *val, bool overwrite)
{
    struct kshell_env *env;
    size_t nlen, vlen;

    env = kshell_env_find(ctx, name);
    if (env && !overwrite)
        return -ENOERR;
    else if (env)
        kshell_env_release(ctx, env);

    nlen = strlen(name) + 1;
    vlen = strlen(val) + 1;

    env = kmalloc(sizeof(*env) + nlen + vlen, GFP_KERNEL);
    if (!env)
        return -ENOMEM;

    env->val = env->name + nlen;
    strcpy(env->val, val);
    strcpy(env->name, name);
    rb_insert(&ctx->env, &env->node, env_rb_cmp);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_setenv);

state kshell_putenv(struct kshell_context *ctx, char *string)
{
    char *equal;
    state ret;

    equal = strchr(string, '=');
    if (!equal)
        return -EINVAL;

    *equal = '\0';
    ret = kshell_setenv(ctx, string, equal + 1, true);
    *equal = '=';

    return ret;
}
EXPORT_SYMBOL(kshell_putenv);

state kshell_unsetenv(struct kshell_context *ctx, const char *name)
{
    struct kshell_env *env;

    if (strchr(name, '='))
        return -EINVAL;

    env = kshell_env_find(ctx, name);
    if (!env)
        return -ENODATA;

    kshell_env_release(ctx, env);
    return -ENOERR;
}
EXPORT_SYMBOL(kshell_unsetenv);

void kshell_env_destory(struct kshell_context *ctx)
{
    struct kshell_env *env, *tmp;

    rb_post_for_each_entry_safe(env, tmp, &ctx->env, node)
        kfree(env);

    ctx->env = RB_INIT;
}

state kshell_env_clone(struct kshell_context *ctx, struct kshell_context *old)
{
    struct kshell_env *env;

    ctx->env = RB_INIT;
    rb_for_each_entry(env, &old->env, node) {
        if (kshell_setenv(ctx, env->name, env->val, true)) {
            kshell_env_destory(ctx);
            return -ENOMEM;
        }
    }

    return -ENOERR;
}

static state kshell_env_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_env *env;
    rb_for_each_entry(env, &ctx->env, node)
        kshell_printf(ctx, "\t%s=%s\n", env->name, env->val);
    return -ENOERR;
}

static struct kshell_command kshell_env_cmd = {
    .name = "env",
    .desc = "displays all environment variable",
    .exec = kshell_env_main,
    .prepare = kshell_env_clone,
    .release = kshell_env_destory,
};

static state env_init(void)
{
    return kshell_register(&kshell_env_cmd);
}
kshell_initcall(env_init);
