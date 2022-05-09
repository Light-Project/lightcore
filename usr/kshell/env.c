/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <kmalloc.h>
#include <export.h>

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

state kshell_envclone(struct kshell_context *ctx, struct kshell_context *new)
{
    struct kshell_env *env;

    rb_for_each_entry(env, &ctx->env, node) {
        if (kshell_setenv(new, env->name, env->val, true)) {
            kshell_envrelease(new);
            return -ENOMEM;
        }
    }

    return -ENOERR;
}

void kshell_envrelease(struct kshell_context *ctx)
{
    struct kshell_env *env, *tmp;
    rb_post_for_each_entry_safe(env, tmp, &ctx->env, node)
        kshell_env_release(ctx, env);
}
