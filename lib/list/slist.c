#include <slist.h>
#include <stddef.h>
#include <types.h>

void slist_head_init(struct slist_head *list)
{
    list->next = list;
}

void slist_add(struct slist_head *node, struct slist_head *new)
{
    new->next = node->next;
    node->next = new;
}

void slist_del(struct slist_head *head, struct slist_head *node)
{
    struct slist_head *walk = head;
    
    while(walk->next != node)
        walk = walk->next;
    
    walk->next = node->next;
}

void slist_replace(struct slist_head *head, struct slist_head *old, struct slist_head *new)
{
    struct slist_head *walk = head;
    
    while(walk->next != old)
        walk = walk->next;
    
    walk->next = new;
    new->next = old->next;
}

bool slist_check_first(struct slist_head *head, struct slist_head *node)
{
    return head->next == node;
}

bool slist_check_end(struct slist_head *node)
{
    return node->next == NULL;
}

bool slist_check_empty(struct slist_head *head)
{
    return head->next == NULL;
}
