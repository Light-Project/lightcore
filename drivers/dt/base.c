/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Equipment tree basic operation
 *
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 * 
 */

#include <driver/dt.h>
#include <driver/dt/libfdt.h>
#include <string.h>

struct dt_node *dt_root;
struct dt_node *dt_chosen;
struct dt_node *dt_stdout;

struct dt_node *dt_find_all(struct dt_node *prev)
{
    struct dt_node *node;
    if(!prev)
        node = dt_root;
    else if (!slist_check_empty(&prev->child))
        node = slist_first_entry(&prev->child, struct dt_node, sibling);
    else {
        node = prev;
        while (node->parent && slist_check_empty(&prev->sibling))
            node = node->parent;
        node = slist_next_entry(node, sibling);
    }
    return node;
}

struct dt_attribute *dt_attribute_find(const struct dt_node *node, 
                                       const char *name)
{
    struct dt_attribute *attribute;
    slist_for_each_entry(attribute, &node->attribute, list) {
        if (!strcmp(attribute->name, name))
            return attribute;
    }
    return NULL;
}

const void *dt_attribute_get(const struct dt_node *node, 
                             const char *name)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, name);
    return attribute ? attribute->value : NULL;
}

bool dt_attribute_match(const struct dt_node *node, 
                        const char *atname, const char *val)
{
    struct dt_attribute *attribute;
    attribute = dt_attribute_find(node, atname);
    return attribute && !strcmp(attribute->value, val);
}

bool dt_match_name(const struct dt_node *node, const char *name)
{   
    const char *node_name;
    int len;

    node_name = kbasename(node->path);
	len = strchrnul(name, '@') - node_name;
	return (strlen(node_name) == len) && 
           (!strncmp(node_name, name, len));
}

bool dt_match(const struct dt_device_id *id, const struct dt_node *node)
{
    if (dt_attribute_match(node, "compatible", id->compatible))
        return true;
    if (dt_attribute_match(node, "device_type", id->type))
        return true;
    if (dt_match_name(node, id->name))
        return true;
    return false;
}

int dt_address_nr(const struct dt_node *node)
{
    int count;

    for (count = 0; count < 32; count++) {


    }

    return count;
}

void *dt_address(const struct dt_node *node, int reg)
{


    return NULL;
}
