/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <string.h>
#include <mhelper.h>
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

static struct kshell_env *interior_find(struct rb_root *head, const char *name)
{
    const struct rb_node *rb;
    rb = rb_find(head, name, env_rb_find);
    return rb ? env_to_kshell(rb) : NULL;
}

static const char *interior_getenv(struct rb_root *head, const char *name)
{
    struct kshell_env *env;

    if (strchr(name, '='))
        return NULL;

    env = interior_find(head, name);
    if (!env)
        return NULL;

    return env->val;
}

static state interior_setenv(struct rb_root *head, const char *name,
                             const char *val, bool overwrite)
{
    struct kshell_env *env;
    size_t nlen, vlen;

    env = interior_find(head, name);
    if (env && !overwrite)
        return -ENOERR;
    else if (env) {
        rb_delete(head, &env->node);
        kfree(env);
    }

    nlen = strlen(name) + 1;
    vlen = strlen(val) + 1;

    env = kmalloc(sizeof(*env) + nlen + vlen, GFP_KERNEL);
    if (!env)
        return -ENOMEM;

    env->val = env->name + nlen;
    strcpy(env->val, val);
    strcpy(env->name, name);
    rb_insert(head, &env->node, env_rb_cmp);

    return -ENOERR;
}

static state interior_putenv(struct rb_root *head, char *string)
{
    char *equal;
    state ret;

    equal = strchr(string, '=');
    if (string == equal)
        return -EINVAL;

    if (!equal)
        ret = interior_setenv(head, string, "", true);
    else {
        *equal = '\0';
        ret = interior_setenv(head, string, equal + 1, true);
        *equal = '=';
    }

    return ret;
}

static state interior_unsetenv(struct rb_root *head, const char *name)
{
    struct kshell_env *env;

    env = interior_find(head, name);
    if (!env)
        return -ENODATA;

    rb_delete(head, &env->node);
    kfree(env);

    return -ENOERR;
}

#define GENERIC_ENV_NOPS(name, func, child, rtype, argnr, ...)                  \
rtype kshell_##name                                                             \
(struct kshell_context *ctx, MMAP_DECLN(argnr, MARGFN_DECL, __VA_ARGS__))       \
{                                                                               \
    struct rb_root *head = child;                                               \
    return interior_##func(head, MMAP_DECLN(argnr, MARGFN_ARGS, __VA_ARGS__));  \
}                                                                               \
EXPORT_SYMBOL(kshell_##name)

#define GENERIC_ENV_OPS(name, func, child, rtype, ...) \
    GENERIC_ENV_NOPS(name, func, child, rtype, COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

GENERIC_ENV_OPS(global_get, getenv, &ctx->env, const char *, const char *);
GENERIC_ENV_OPS(global_set, setenv, &ctx->env, state, const char *, const char *, bool);
GENERIC_ENV_OPS(global_put, putenv, &ctx->env, state, char *);
GENERIC_ENV_OPS(global_unset, unsetenv, &ctx->env, state, const char *);

GENERIC_ENV_OPS(local_get, getenv, &list_first_entry(&ctx->local, struct kshell_local, list)->env, const char *, const char *);
GENERIC_ENV_OPS(local_set, setenv, &list_first_entry(&ctx->local, struct kshell_local, list)->env, state, const char *, const char *, bool);
GENERIC_ENV_OPS(local_put, putenv, &list_first_entry(&ctx->local, struct kshell_local, list)->env, state, char *);
GENERIC_ENV_OPS(local_unset, unsetenv, &list_first_entry(&ctx->local, struct kshell_local, list)->env, state, const char *);

const char *kshell_getenv(struct kshell_context *ctx, const char *name)
{
    return kshell_local_get(ctx, name) ?: kshell_global_get(ctx, name);
}
EXPORT_SYMBOL(kshell_getenv);

state kshell_setenv(struct kshell_context *ctx, const char *name, const char *val, bool overwrite)
{
    if (kshell_local_get(ctx, name))
        return kshell_local_set(ctx, name, val, overwrite);
    return kshell_global_set(ctx, name, val, overwrite);
}
EXPORT_SYMBOL(kshell_setenv);

state kshell_putenv(struct kshell_context *ctx, char *string)
{
    char *equal;
    state ret;

    equal = strchr(string, '=');
    if (string == equal)
        return -EINVAL;

    if (!equal)
        ret = kshell_setenv(ctx, string, "", true);
    else {
        *equal = '\0';
        ret = kshell_setenv(ctx, string, equal + 1, true);
        *equal = '=';
    }

    return ret;
}
EXPORT_SYMBOL(kshell_putenv);

state kshell_unsetenv(struct kshell_context *ctx, const char *name)
{
    return kshell_local_unset(ctx, name) &&
           kshell_local_unset(ctx, name) ?
           -ENODATA : -ENOERR;
}
EXPORT_SYMBOL(kshell_unsetenv);

state kshell_local_push(struct kshell_context *ctx)
{
    struct kshell_local *local;

    local = kmalloc(sizeof(*local), GFP_KERNEL);
    if (unlikely(!local))
        return -ENOMEM;

    local->env = RB_INIT;
    list_add(&ctx->local, &local->list);

    return -ENOERR;
}
EXPORT_SYMBOL(kshell_local_push);

state kshell_local_pop(struct kshell_context *ctx)
{
    struct kshell_local *local = list_first_entry(&ctx->local, struct kshell_local, list);
    struct kshell_env *env, *tmp;

    rb_post_for_each_entry_safe(env, tmp, &local->env, node)
        kfree(env);

    list_del(&local->list);
    return -ENOERR;
}
EXPORT_SYMBOL(kshell_local_pop);

static void kshell_env_destory(struct kshell_context *ctx)
{
    struct kshell_env *env, *tmp;

    kshell_local_pop(ctx);
    rb_post_for_each_entry_safe(env, tmp, &ctx->env, node)
        kfree(env);
}

static state kshell_env_clone(struct kshell_context *ctx, struct kshell_context *old)
{
    struct kshell_env *env;

    ctx->env = RB_INIT;
    list_head_init(&ctx->local);
    kshell_local_push(ctx);

    rb_for_each_entry(env, &old->env, node) {
        if (kshell_setenv(ctx, env->name, env->val, true)) {
            kshell_local_pop(ctx);
            kshell_env_destory(ctx);
            return -ENOMEM;
        }
    }

    return -ENOERR;
}

static void usage(struct kshell_context *ctx, const char *cmd)
{
    kshell_printf(ctx, "usage %s [option] name[=value]...\n", cmd);
    kshell_printf(ctx, "\t-u  remove variable from the environment\n");
    kshell_printf(ctx, "\t-h  display this message\n");
}

static state kshell_local_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_local *local = list_first_entry(&ctx->local, struct kshell_local, list);
    struct kshell_env *env = NULL;
    unsigned int count;
    state retval = -ENOERR;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'l':
                env = (void *)true;
                break;

            case 'u':
                if ((++count) >= argc)
                    goto usage;
                kshell_local_unset(ctx, argv[count]);
                break;

            case 'h': default:
                goto usage;
        }
    }

    while (count < argc) {
        retval = kshell_local_put(ctx, argv[count++]);
        if (retval)
            break;
    }

    if (count == 1 || (env && !retval)) {
        rb_for_each_entry(env, &local->env, node)
            kshell_printf(ctx, "\t%s=%s\n", env->name, env->val);
    }

    return retval;

usage:
    usage(ctx, argv[0]);
    return -EINVAL;
}

static state kshell_env_main(struct kshell_context *ctx, int argc, char *argv[])
{
    struct kshell_env *env = NULL;
    unsigned int count;
    state retval = -ENOERR;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'l':
                env = (void *)true;
                break;

            case 'u':
                if ((++count) >= argc)
                    goto usage;
                kshell_local_unset(ctx, argv[count]);
                break;

            case 'h': default:
                goto usage;
        }
    }

    while (count < argc) {
        retval = kshell_local_put(ctx, argv[count++]);
        if (retval)
            break;
    }

    if (count == 1 || (env && !retval)) {
        rb_for_each_entry(env, &ctx->env, node)
            kshell_printf(ctx, "\t%s=%s\n", env->name, env->val);
    }

    return retval;

usage:
    usage(ctx, argv[0]);
    return -EINVAL;
}

static struct kshell_command kshell_local_cmd = {
    .name = "local",
    .desc = "setting local variables",
    .exec = kshell_local_main,
};

static struct kshell_command kshell_env_cmd = {
    .name = "env",
    .desc = "setting environment variables",
    .exec = kshell_env_main,
    .prepare = kshell_env_clone,
    .release = kshell_env_destory,
};

static state env_init(void)
{
    state retval;

    retval = kshell_register(&kshell_env_cmd);
    if (retval)
        return retval;

    return kshell_register(&kshell_local_cmd);
}
kshell_initcall(env_init);
