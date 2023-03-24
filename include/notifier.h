/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _NOTIFIER_H_
#define _NOTIFIER_H_

#include <ilist.h>
#include <mutex.h>
#include <spinlock.h>

struct notifier_node;
typedef enum notifier_return notifier_return_t;
typedef notifier_return_t (*notifier_entry_t)(struct notifier_node *node, void *arg);

enum notifier_return {
    NOTIFI_RET_DONE     = 0x00,
    NOTIFI_RET_WARN     = 0x01,
    NOTIFI_RET_REMOVE   = 0x02,
    NOTIFI_RET_STOP     = 0x04,
};

/**
 * struct notifier_node - describe a notification chain node.
 * @list: node for hanging notification chain.
 * @entry: callback function of notification chain.
 * @priority: priority of notification chain node.
 * @pdata: private data of notification chain node.
 */
struct notifier_node {
    struct ilist_node list;
    notifier_entry_t entry;
    int priority;
    void *pdata;
};

/**
 * struct notifier_head - generic notification chain header.
 * @node: node header for managing notification chain nodes.
 * @name: name of notification chain.
 */
struct notifier_head {
    struct ilist_head node;
    const char *name;
};

/**
 * struct notifier_raw_head - describe raw notification chain head.
 * @head: generic notification chain header.
 */
struct notifier_raw_head {
    struct notifier_head head;
};

/**
 * struct notifier_spin_head - describe atomic notification chain head.
 * @head: generic notification chain header.
 * @lock: spinlock of notification chain.
 */
struct notifier_spin_head {
    struct notifier_head head;
    spinlock_t lock;
};

#define ilist_to_notifier(ptr) \
    ilist_entry(ptr, struct notifier_node, list)

#define NOTIFIER_HEAD_STATIC(object) {              \
    .name = __stringify(object),                    \
    .node = ILIST_HEAD_STATIC((object).head.node),  \
}

#define NOTIFIER_RAW_HEAD_INIT(name)                \
(struct notifier_raw_head) {                        \
    .head = NOTIFIER_HEAD_STATIC(name),             \
}

#define NOTIFIER_SPIN_HEAD_INIT(name)               \
(struct notifier_spin_head) {                       \
    .head = NOTIFIER_HEAD_STATIC(name),             \
    .lock = SPIN_LOCK_STATIC((name).lock)           \
}

#define DEFINE_NOTIFIER_RAW_HEAD(name) \
    struct notifier_raw_head name = NOTIFIER_RAW_HEAD_INIT(name)

#define DEFINE_NOTIFIER_SPIN_HEAD(name) \
    struct notifier_spin_head name = NOTIFIER_SPIN_HEAD_INIT(name)

#define DECLARE_NOTIFIER_RAW_HEAD(name) \
    extern struct notifier_raw_head name

#define DECLARE_NOTIFIER_SPIN_HEAD(name) \
    extern struct notifier_spin_head name

extern state notifier_raw_chain_register(struct notifier_raw_head *head, struct notifier_node *node);
extern void notifier_raw_chain_unregister(struct notifier_raw_head *head, struct notifier_node *node);
extern notifier_return_t notifier_raw_chain_call(struct notifier_raw_head *head, void *pdata, int call_num, int *called_num);

extern state notifier_spin_chain_register(struct notifier_spin_head *head, struct notifier_node *node);
extern void notifier_spin_chain_unregister(struct notifier_spin_head *head, struct notifier_node *node);
extern notifier_return_t notifier_spin_chain_call(struct notifier_spin_head *head, void *pdata, int call_num, int *called_num);

#endif /* _NOTIFIER_H_ */
