/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 John Sanpe <sanpeqf@gmail.com>
 */

#include "kshell.h"
#include <ctype.h>
#include <string.h>
#include <mhelper.h>
#include <kmalloc.h>
#include <export.h>
#include <initcall.h>

static long kshenv_rb_cmp(const struct rb_node *rba, const struct rb_node *rbb)
{
    const struct kshell_env *enva = env_to_kshell(rba);
    const struct kshell_env *envb = env_to_kshell(rbb);
    return strcmp(enva->name, envb->name);
}

static long kshenv_rb_find(const struct rb_node *rb, const void *name)
{
    const struct kshell_env *env = env_to_kshell(rb);
    return strcmp(env->name, name);
}

static struct kshell_env *kshenv_find(struct rb_root *head, const char *name)
{
    const struct rb_node *rb;
    rb = rb_find(head, name, kshenv_rb_find);
    return rb ? env_to_kshell(rb) : NULL;
}

static state kshenv_setval(struct rb_root *head, const char *name,
                           const char *value, bool overwrite)
{
    struct kshell_env *env;

    env = kshenv_find(head, name);
    if (env && !overwrite)
        return -ENOERR;
    else if (env) {
        rb_delete(head, &env->node);
        kfree_const(env->value);
        kfree_const(env->name);
        kfree(env);
    }

    env = kmalloc(sizeof(*env), GFP_KERNEL);
    if (!env)
        return -ENOMEM;

    env->name = name;
    env->value = value;
    rb_insert(head, &env->node, kshenv_rb_cmp);

    return -ENOERR;
}

static size_t kshenv_checkname(const char *name)
{
    size_t index;

    /*
     * Rule 1: "Foo=bar"
     * Variable name must start with a letter.
     */
    if (!isalpha(*name))
        return 0;

    /*
     * Rule 2: "Foo_123=bar"
     * Variable name can only contain
     * alphanumeric characters and underscores.
     */
    for (index = 0; name[index]; ++index) {
        if (!isalnum(name[index]) && name[index] != '_')
            break;
    }

    return index;
}

static const char *generic_getenv(struct rb_root *head, bool check, const char *name)
{
    struct kshell_env *env;

    env = kshenv_find(head, name);
    if (!env)
        return NULL;

    return env->value;
}

static state generic_setenv(struct rb_root *head, bool check, const char *name,
                            const char *value, bool overwrite)
{
    const char *nname, *nvalue;
    state retval, namelen;

    if (check) {
        namelen = kshenv_checkname(name);
        if (!namelen)
            return -EINVAL;
        if (namelen != strlen(name))
            return -EINVAL;
    }

    nname = kstrdup_const(name, GFP_KERNEL);
    nvalue = kstrdup_const(value, GFP_KERNEL);

    if (!nname || !nvalue) {
        retval = -ENOMEM;
        goto failed;
    }

    retval = kshenv_setval(head, nname, nvalue, overwrite);
    if (retval)
        goto failed;

    return -ENOERR;

failed:
    kfree_const(nname);
    kfree_const(nvalue);
    return retval;
}

static state generic_putenv(struct rb_root *head, bool check, const char *string)
{
    const char *name, *value = NULL;
    size_t namelen;
    state retval;

    if (check)
        namelen = kshenv_checkname(string);
    else {
        name = strchr(string, '=');
        if (name)
            namelen = name - string;
        else
            namelen = strlen(string);
    }

    if (!namelen)
        return -EINVAL;

    name = kstrndup(string, namelen, GFP_KERNEL);
    if (!name)
        return -ENOMEM;

    if (!string[namelen]) {
        retval = kshenv_setval(head, name, "", true);
        if (retval) {
            retval = -ENOMEM;
            goto failed;
        }

        return -ENOERR;
    }

    if (string[namelen] == '=') {
        value = kstrdup_const(string + namelen + 1, GFP_KERNEL);
        if (!value) {
            retval = -ENOMEM;
            goto failed;
        }

        retval = kshenv_setval(head, name, value, true);
        if (retval) {
            retval = -ENOMEM;
            goto failed;
        }

        return -ENOERR;
    }

    if (string[namelen + 1] != '=') {
        retval = -EINVAL;
        goto failed;
    }

    value = kstrdup_const(string + namelen + 2, GFP_KERNEL);
    if (!value) {
        retval = -ENOMEM;
        goto failed;
    }

    switch (string[namelen]) {
        case '?':
            retval = kshenv_setval(head, name, value, false);
            if (retval)
                goto failed;
            break;

        case ':':
            if (!kshenv_find(head, name))
                break;
            retval = kshenv_setval(head, name, value, true);
            if (retval)
                goto failed;
            break;

        default:
            retval = -EINVAL;
            goto failed;
    }

    return -ENOERR;

failed:
    kfree(name);
    kfree_const(value);
    return retval;
}

static state generic_unsetenv(struct rb_root *head, bool check, const char *name)
{
    struct kshell_env *env;

    env = kshenv_find(head, name);
    if (!env)
        return -ENODATA;

    rb_delete(head, &env->node);
    kfree_const(env->value);
    kfree_const(env->name);
    kfree(env);

    return -ENOERR;
}

#define GENERIC_ENV_NOPS(name, func, child, check, rtype, argnr, ...)       \
rtype kshell_##name                                                         \
(struct kshell_context *ctx, MMAP_DECLN(argnr, MARGFN_DECL, __VA_ARGS__))   \
{                                                                           \
    struct rb_root *head = child;                                           \
    return generic_##func(head, check,                                      \
           MMAP_DECLN(argnr, MARGFN_ARGS, __VA_ARGS__));                    \
}                                                                           \
EXPORT_SYMBOL(kshell_##name)

#define GENERIC_ENV_OPS(name, func, child, check, rtype, ...) \
    GENERIC_ENV_NOPS(name, func, child, check, rtype, COUNT_ARGS(__VA_ARGS__), __VA_ARGS__)

GENERIC_ENV_OPS(global_get, getenv, &ctx->env,
                true, const char *, const char *);
GENERIC_ENV_OPS(global_set, setenv, &ctx->env,
                true, state, const char *, const char *, bool);
GENERIC_ENV_OPS(global_put, putenv, &ctx->env,
                true, state, const char *);
GENERIC_ENV_OPS(global_unset, unsetenv, &ctx->env,
                true, state, const char *);

GENERIC_ENV_OPS(local_get, getenv, &list_first_entry(&ctx->local, struct kshell_stack, list)->env,
                true, const char *, const char *);
GENERIC_ENV_OPS(local_set, setenv, &list_first_entry(&ctx->local, struct kshell_stack, list)->env,
                true, state, const char *, const char *, bool);
GENERIC_ENV_OPS(local_put, putenv, &list_first_entry(&ctx->local, struct kshell_stack, list)->env,
                true, state, const char *);
GENERIC_ENV_OPS(local_unset, unsetenv, &list_first_entry(&ctx->local, struct kshell_stack, list)->env,
                true, state, const char *);

GENERIC_ENV_OPS(symbol_get, getenv, &list_first_entry(&ctx->symbol, struct kshell_stack, list)->env,
                false, const char *, const char *);
GENERIC_ENV_OPS(symbol_set, setenv, &list_first_entry(&ctx->symbol, struct kshell_stack, list)->env,
                false, state, const char *, const char *, bool);
GENERIC_ENV_OPS(symbol_put, putenv, &list_first_entry(&ctx->symbol, struct kshell_stack, list)->env,
                false, state, const char *);
GENERIC_ENV_OPS(symbol_unset, unsetenv, &list_first_entry(&ctx->symbol, struct kshell_stack, list)->env,
                false, state, const char *);

#define GENERIC_ENV_STACK(name, child)                                  \
state kshell_##name##_push(struct kshell_context *ctx)                  \
{                                                                       \
    struct kshell_stack *stack;                                         \
                                                                        \
    stack = kmalloc(sizeof(*stack), GFP_KERNEL);                        \
    if (unlikely(!stack))                                               \
        return -ENOMEM;                                                 \
                                                                        \
    stack->env = RB_INIT;                                               \
    list_add(&ctx->child, &stack->list);                                \
                                                                        \
    return -ENOERR;                                                     \
}                                                                       \
EXPORT_SYMBOL(kshell_##name##_push);                                    \
                                                                        \
state kshell_##name##_pop(struct kshell_context *ctx)                   \
{                                                                       \
    struct kshell_stack *stack;                                         \
    struct kshell_env *env, *tmp;                                       \
                                                                        \
    stack = list_first_entry(&ctx->child, struct kshell_stack, list);   \
    rb_post_for_each_entry_safe(env, tmp, &stack->env, node)            \
        kfree(env);                                                     \
                                                                        \
    list_del(&stack->list);                                             \
    return -ENOERR;                                                     \
}                                                                       \
EXPORT_SYMBOL(kshell_##name##_pop)

GENERIC_ENV_STACK(local, local);
GENERIC_ENV_STACK(symbol, symbol);

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

state kshell_putenv(struct kshell_context *ctx, const char *string)
{
    const char *name;
    size_t namelen;
    state retval;

    namelen = kshenv_checkname(string);
    if (!namelen)
        return -EINVAL;

    name = kstrndup(string, namelen, GFP_KERNEL);
    if (!name)
        return -ENOMEM;

    if (kshell_local_get(ctx, name))
        retval = kshell_local_put(ctx, string);
    else
        retval = kshell_global_put(ctx, string);

    kfree(name);
    return retval;
}
EXPORT_SYMBOL(kshell_putenv);

state kshell_unsetenv(struct kshell_context *ctx, const char *name)
{
    return kshell_local_unset(ctx, name) &&
           kshell_local_unset(ctx, name) ?
           -ENODATA : -ENOERR;
}
EXPORT_SYMBOL(kshell_unsetenv);

static void kshell_env_destory(struct kshell_context *ctx)
{
    struct kshell_env *env, *tmp;

    rb_post_for_each_entry_safe(env, tmp, &ctx->env, node) {
        kfree_const(env->value);
        kfree_const(env->name);
        kfree(env);
    }

    kshell_local_pop(ctx);
    kshell_symbol_pop(ctx);
}

static state kshell_env_clone(struct kshell_context *ctx, struct kshell_context *old)
{
    struct kshell_env *env;

    ctx->env = RB_INIT;
    list_head_init(&ctx->local);
    list_head_init(&ctx->symbol);
    kshell_local_push(ctx);
    kshell_symbol_push(ctx);

    rb_for_each_entry(env, &old->env, node) {
        if (kshell_setenv(ctx, env->name, env->value, true)) {
            kshell_local_pop(ctx);
            kshell_env_destory(ctx);
            return -ENOMEM;
        }
    }

    return -ENOERR;
}

static struct kshell_command kshell_env = {
    .prepare = kshell_env_clone,
    .release = kshell_env_destory,
};

static state env_init(void)
{
    return kshell_register(&kshell_env);
}
kshell_initcall(env_init);
