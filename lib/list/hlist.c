#include <hlist.h>
#include <types.h>
#include <stddef.h>

void hlist_head_init(hlist_head_t *head)
{
    head->first = NULL;
}

void hlist_node_init(hlist_node_t *node)
{
    node->pprev = NULL;
    node->next = NULL;
}

bool hlist_unhashed(hlist_node_t *node)
{
    return !node->pprev;
}

bool hlist_empty(hlist_head_t *head)
{
    return !head->first;
}

void hlist_head_add(hlist_head_t *head, hlist_node_t *node)
{
    node->next = head->first;
    if(head->first != NULL)
        head->first->pprev = &node->next;
    head->first = node;
    node->pprev = &head;
}

void hlist_prev_add(hlist_node_t *node, hlist_node_t *new)
{
    new->pprev = new->pprev;
    new->next = node;
    node->pprev = 
}

void hlist_next_add(hlist_node_t *node, hlist_node_t *new)
{
    new->next = node->next;
    node->next = new;
    new->pprev = &node->next;
    if(new->next != NULL)
        new->next->pprev = &node->next;
}

void hlist_del()
{


}