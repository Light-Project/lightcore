
#include <klist.h>
#include <export.h>
#include <system/sched.h>
#include <system/spinlock.h>

/*
 * Use the lowest bit of n_klist to mark deleted nodes and exclude
 * dead ones from iteration.
 */
#define KNODE_DEAD		1LU
#define KNODE_KLIST_MASK	~KNODE_DEAD

static struct klist *knode_klist(struct klist_node *knode)
{
	return (struct klist *)
		((unsigned long)knode->n_klist & KNODE_KLIST_MASK);
}

static bool knode_dead(struct klist_node *knode)
{
	return (unsigned long)knode->n_klist & KNODE_DEAD;
}

static void knode_set_klist(struct klist_node *knode, struct klist *klist)
{
	knode->n_klist = klist;
	/* no knode deserves to start its life dead */
	WARN_ON(knode_dead(knode));
}

static void knode_kill(struct klist_node *knode)
{
	/* and no knode should die twice ever either, see we're very humane */
	WARN_ON(knode_dead(knode));
	*(unsigned long *)&knode->n_klist |= KNODE_DEAD;
}

/**
 * klist_init - Initialize a klist structure.
 * @k: The klist we're initializing.
 * @get: The get function for the embedding object (NULL if none)
 * @put: The put function for the embedding object (NULL if none)
 *
 * Initialises the klist structure.  If the klist_node structures are
 * going to be embedded in refcounted objects (necessary for safe
 * deletion) then the get/put arguments are used to initialise
 * functions that take and release references on the embedding
 * objects.
 */
void klist_init(struct klist *k, void (*get)(struct klist_node *),
		void (*put)(struct klist_node *))
{
	INIT_LIST_HEAD(&k->k_list);
	spin_lock_init(&k->k_lock);
	k->get = get;
	k->put = put;
}
EXPORT_SYMBOL_GPL(klist_init);

static void add_head(struct klist *k, struct klist_node *n)
{
	spin_lock(&k->k_lock);
	list_add(&n->n_node, &k->k_list);
	spin_unlock(&k->k_lock);
}

static void add_tail(struct klist *k, struct klist_node *n)
{
	spin_lock(&k->k_lock);
	list_add_tail(&n->n_node, &k->k_list);
	spin_unlock(&k->k_lock);
}

static void klist_node_init(struct klist *k, struct klist_node *n)
{
	INIT_LIST_HEAD(&n->n_node);
	kref_init(&n->n_ref);
	knode_set_klist(n, k);
	if (k->get)
		k->get(n);
}

/**
 * klist_add_head - Initialize a klist_node and add it to front.
 * @n: node we're adding.
 * @k: klist it's going on.
 */
void klist_add_head(struct klist_node *n, struct klist *k)
{
	klist_node_init(k, n);
	add_head(k, n);
}
EXPORT_SYMBOL_GPL(klist_add_head);

/**
 * klist_add_tail - Initialize a klist_node and add it to back.
 * @n: node we're adding.
 * @k: klist it's going on.
 */
void klist_add_tail(struct klist_node *n, struct klist *k)
{
	klist_node_init(k, n);
	add_tail(k, n);
}
EXPORT_SYMBOL_GPL(klist_add_tail);

/**
 * klist_add_behind - Init a klist_node and add it after an existing node
 * @n: node we're adding.
 * @pos: node to put @n after
 */
void klist_add_behind(struct klist_node *n, struct klist_node *pos)
{
	struct klist *k = knode_klist(pos);

	klist_node_init(k, n);
	spin_lock(&k->k_lock);
	list_add(&n->n_node, &pos->n_node);
	spin_unlock(&k->k_lock);
}
EXPORT_SYMBOL_GPL(klist_add_behind);

/**
 * klist_add_before - Init a klist_node and add it before an existing node
 * @n: node we're adding.
 * @pos: node to put @n after
 */
void klist_add_before(struct klist_node *n, struct klist_node *pos)
{
	struct klist *k = knode_klist(pos);

	klist_node_init(k, n);
	spin_lock(&k->k_lock);
	list_add_tail(&n->n_node, &pos->n_node);
	spin_unlock(&k->k_lock);
}
EXPORT_SYMBOL_GPL(klist_add_before);

struct klist_waiter {
	struct list_head list;
	struct klist_node *node;
	struct task_struct *process;
	int woken;
};

static DEFINE_SPINLOCK(klist_remove_lock);
static LIST_HEAD(klist_remove_waiters);

static void klist_release(struct kref *kref)
{
	struct klist_waiter *waiter, *tmp;
	struct klist_node *n = container_of(kref, struct klist_node, n_ref);

	WARN_ON(!knode_dead(n));
	list_del(&n->n_node);
	spin_lock(&klist_remove_lock);
	list_for_each_entry_safe(waiter, tmp, &klist_remove_waiters, list) {
		if (waiter->node != n)
			continue;

		list_del(&waiter->list);
		waiter->woken = 1;
		mb();
		wake_up_process(waiter->process);
	}
	spin_unlock(&klist_remove_lock);
	knode_set_klist(n, NULL);
}

static int klist_dec_and_del(struct klist_node *n)
{
	return kref_put(&n->n_ref, klist_release);
}

static void klist_put(struct klist_node *n, bool kill)
{
	struct klist *k = knode_klist(n);
	void (*put)(struct klist_node *) = k->put;

	spin_lock(&k->k_lock);
	if (kill)
		knode_kill(n);
	if (!klist_dec_and_del(n))
		put = NULL;
	spin_unlock(&k->k_lock);
	if (put)
		put(n);
}

/**
 * klist_del - Decrement the reference count of node and try to remove.
 * @n: node we're deleting.
 */
void klist_del(struct klist_node *n)
{
	klist_put(n, true);
}
EXPORT_SYMBOL_GPL(klist_del);

/**
 * klist_remove - Decrement the refcount of node and wait for it to go away.
 * @n: node we're removing.
 */
void klist_remove(struct klist_node *n)
{
	struct klist_waiter waiter;

	waiter.node = n;
	waiter.process = current;
	waiter.woken = 0;
	spin_lock(&klist_remove_lock);
	list_add(&waiter.list, &klist_remove_waiters);
	spin_unlock(&klist_remove_lock);

	klist_del(n);

	for (;;) {
		set_current_state(TASK_UNINTERRUPTIBLE);
		if (waiter.woken)
			break;
		schedule();
	}
	__set_current_state(TASK_RUNNING);
}
EXPORT_SYMBOL_GPL(klist_remove);

/**
 * klist_node_attached - Say whether a node is bound to a list or not.
 * @n: Node that we're testing.
 */
int klist_node_attached(struct klist_node *n)
{
	return (n->n_klist != NULL);
}
EXPORT_SYMBOL_GPL(klist_node_attached);

/**
 * klist_iter_init_node - Initialize a klist_iter structure.
 * @k: klist we're iterating.
 * @i: klist_iter we're filling.
 * @n: node to start with.
 *
 * Similar to klist_iter_init(), but starts the action off with @n,
 * instead of with the list head.
 */
void klist_iter_init_node(struct klist *k, struct klist_iter *i,
			  struct klist_node *n)
{
	i->i_klist = k;
	i->i_cur = NULL;
	if (n && kref_get_unless_zero(&n->n_ref))
		i->i_cur = n;
}
EXPORT_SYMBOL_GPL(klist_iter_init_node);

/**
 * klist_iter_init - Iniitalize a klist_iter structure.
 * @k: klist we're iterating.
 * @i: klist_iter structure we're filling.
 *
 * Similar to klist_iter_init_node(), but start with the list head.
 */
void klist_iter_init(struct klist *k, struct klist_iter *i)
{
	klist_iter_init_node(k, i, NULL);
}
EXPORT_SYMBOL_GPL(klist_iter_init);

/**
 * klist_iter_exit - Finish a list iteration.
 * @i: Iterator structure.
 *
 * Must be called when done iterating over list, as it decrements the
 * refcount of the current node. Necessary in case iteration exited before
 * the end of the list was reached, and always good form.
 */
void klist_iter_exit(struct klist_iter *i)
{
	if (i->i_cur) {
		klist_put(i->i_cur, false);
		i->i_cur = NULL;
	}
}
EXPORT_SYMBOL_GPL(klist_iter_exit);

static struct klist_node *to_klist_node(struct list_head *n)
{
	return container_of(n, struct klist_node, n_node);
}

/**
 * klist_prev - Ante up prev node in list.
 * @i: Iterator structure.
 *
 * First grab list lock. Decrement the reference count of the previous
 * node, if there was one. Grab the prev node, increment its reference
 * count, drop the lock, and return that prev node.
 */
struct klist_node *klist_prev(struct klist_iter *i)
{
	void (*put)(struct klist_node *) = i->i_klist->put;
	struct klist_node *last = i->i_cur;
	struct klist_node *prev;
	unsigned long flags;

	spin_lock_irqsave(&i->i_klist->k_lock, flags);

	if (last) {
		prev = to_klist_node(last->n_node.prev);
		if (!klist_dec_and_del(last))
			put = NULL;
	} else
		prev = to_klist_node(i->i_klist->k_list.prev);

	i->i_cur = NULL;
	while (prev != to_klist_node(&i->i_klist->k_list)) {
		if (likely(!knode_dead(prev))) {
			kref_get(&prev->n_ref);
			i->i_cur = prev;
			break;
		}
		prev = to_klist_node(prev->n_node.prev);
	}

	spin_unlock_irqrestore(&i->i_klist->k_lock, flags);

	if (put && last)
		put(last);
	return i->i_cur;
}
EXPORT_SYMBOL_GPL(klist_prev);

/**
 * klist_next - Ante up next node in list.
 * @i: Iterator structure.
 *
 * First grab list lock. Decrement the reference count of the previous
 * node, if there was one. Grab the next node, increment its reference
 * count, drop the lock, and return that next node.
 */
struct klist_node *klist_next(struct klist_iter *i)
{
	void (*put)(struct klist_node *) = i->i_klist->put;
	struct klist_node *last = i->i_cur;
	struct klist_node *next;
	unsigned long flags;

	spin_lock_irqsave(&i->i_klist->k_lock, flags);

	if (last) {
		next = to_klist_node(last->n_node.next);
		if (!klist_dec_and_del(last))
			put = NULL;
	} else
		next = to_klist_node(i->i_klist->k_list.next);

	i->i_cur = NULL;
	while (next != to_klist_node(&i->i_klist->k_list)) {
		if (likely(!knode_dead(next))) {
			kref_get(&next->n_ref);
			i->i_cur = next;
			break;
		}
		next = to_klist_node(next->n_node.next);
	}

	spin_unlock_irqrestore(&i->i_klist->k_lock, flags);

	if (put && last)
		put(last);
	return i->i_cur;
}
EXPORT_SYMBOL_GPL(klist_next);
