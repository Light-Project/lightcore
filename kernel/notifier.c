/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#define MODULE_NAME "notifier"
#define pr_fmt(fmt) MODULE_NAME ": " fmt

#include <notifier.h>
#include <printk.h>
#include <panic.h>
#include <export.h>

static long notifier_chain_cmp(struct ilist_node *nodea, struct ilist_node *nodeb, const void *pdata)
{
    struct notifier_node *nnodea = ilist_to_notifier(nodea);
    struct notifier_node *nnodeb = ilist_to_notifier(nodeb);
    if (nnodea->priority == nnodeb->priority) return 0;
    return nnodea->priority < nnodeb->priority ? -1 : 1;
}

/**
 * notifier_chain_register - register a node to the notification chain.
 * @head: header to be registered.
 * @node: notification chain node to register.
 */
static state notifier_chain_register(struct notifier_head *head, struct notifier_node *node)
{
    if (!node->entry)
        return -EINVAL;

    ilist_node_init(&node->list);
    ilist_add(&head->node, &node->list, notifier_chain_cmp, NULL);
    pr_debug("chain '%s' register (%p)\n", head->name, node);

    return -ENOERR;
}

/**
 * notifier_chain_unregister - unregister a node from the notification chain.
 * @head: raw header to be unregistered.
 * @node: notification chain node to unregister.
 */
static void notifier_chain_unregister(struct notifier_head *head, struct notifier_node *node)
{
    ilist_del(&head->node, &node->list);
    pr_debug("chain '%s' unregister (%p)\n", head->name, node);
}

/**
 * notifier_chain_call - call the callback function of node on notification chain.
 * @head: chain header to be notified.
 * @arg: parameters to be passed to the node.
 * @call_num: number of nodes to be notified.
 * @called_num: number of nodes actually notified.
 */
static notifier_return_t
notifier_chain_call(struct notifier_head *head, void *arg, int call_num, int *called_num)
{
    struct notifier_node *node, *tmp;
    notifier_return_t retval = NOTIFI_RET_DONE;

    ilist_for_each_entry_safe(node, tmp, &head->node, list) {
        if (!call_num--)
            break;

        pr_debug("chain '%s' calling (%p)\n", head->name, node);
        retval = node->entry(node, arg);

        if (called_num)
            (*called_num)++;

        if (retval & NOTIFI_RET_REMOVE) {
            ilist_del(&head->node, &node->list);
            retval &= ~NOTIFI_RET_REMOVE;
        }

        if (retval & NOTIFI_RET_STOP)
            break;
    }

    return retval;
}

/**
 * notifier_raw_chain_register - register a node to the raw notification chain.
 * @head: raw header to be registered.
 * @node: notification chain node to register.
 */
state notifier_raw_chain_register(struct notifier_raw_head *head, struct notifier_node *node)
{
    return notifier_chain_register(&head->head, node);
}
EXPORT_SYMBOL(notifier_raw_chain_register);

/**
 * notifier_raw_chain_unregister - unregister a node from the raw notification chain.
 * @head: raw header to be unregistered.
 * @node: notification chain node to unregister.
 */
void notifier_raw_chain_unregister(struct notifier_raw_head *head, struct notifier_node *node)
{
    notifier_chain_unregister(&head->head, node);
}
EXPORT_SYMBOL(notifier_raw_chain_unregister);

/**
 * notifier_raw_chain_call - call the callback function of node on raw notification chain.
 * @head: raw chain header to be notified.
 * @arg: parameters to be passed to the node.
 * @call_num: number of nodes to be notified.
 * @called_num: number of nodes actually notified.
 */
notifier_return_t notifier_raw_chain_call(struct notifier_raw_head *head, void *arg,
                                          int call_num, int *called_num)
{
    return notifier_chain_call(&head->head, arg, call_num, called_num);
}
EXPORT_SYMBOL(notifier_raw_chain_call);

/**
 * notifier_spin_chain_register - register a node to the atomic notification chain.
 * @head: atomic header to be registered.
 * @node: notification chain node to register.
 */
state notifier_spin_chain_register(struct notifier_spin_head *head, struct notifier_node *node)
{
    state retval;
    spin_lock(&head->lock);
    retval = notifier_chain_register(&head->head, node);
    spin_unlock(&head->lock);
    return retval;
}
EXPORT_SYMBOL(notifier_spin_chain_register);

/**
 * notifier_spin_chain_unregister - unregister a node from the atomic notification chain.
 * @head: atomic header to be unregistered.
 * @node: notification chain node to unregister.
 */
void notifier_spin_chain_unregister(struct notifier_spin_head *head, struct notifier_node *node)
{
    spin_lock(&head->lock);
    notifier_chain_unregister(&head->head, node);
    spin_unlock(&head->lock);
}
EXPORT_SYMBOL(notifier_spin_chain_unregister);

/**
 * notifier_spin_chain_call - call the callback function of node on atomic notification chain.
 * @head: atomic chain header to be notified.
 * @arg: parameters to be passed to the node.
 * @call_num: number of nodes to be notified.
 * @called_num: number of nodes actually notified.
 */
notifier_return_t notifier_spin_chain_call(struct notifier_spin_head *head, void *arg,
                                           int call_num, int *called_num)
{
    notifier_return_t retval;
    spin_lock(&head->lock);
    retval = notifier_chain_call(&head->head, arg, call_num, called_num);
    spin_unlock(&head->lock);
    return retval;
}
EXPORT_SYMBOL(notifier_spin_chain_call);
