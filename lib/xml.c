/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <xml.h>
#include <ctype.h>
#include <string.h>
#include <kmalloc.h>
#include <export.h>

#define PASER_TEXT_DEF      64
#define PASER_NODE_DEPTH    32

enum xml_state {
    XML_STATE_LABEL     = 0,
    XML_STATE_NAME      = 1,
    XML_STATE_ANAME     = 2,
    XML_STATE_AVALUE    = 3,
    XML_STATE_STRING    = 4,
    XML_STATE_AWAIT     = 5,
    XML_STATE_EQUAL     = 6,
    XML_STATE_BODY      = 7,
    XML_STATE_PARAPH    = 8,
    XML_STATE_EXIT      = 9,
    XML_STATE_IGNORE    = 10,
};

struct xml_transition {
    enum xml_state form, to;
    char code;
    int stack;
    bool cross;
    bool slack;
};

static const struct xml_transition transition_table[] = {
    {XML_STATE_BODY,    XML_STATE_LABEL,    '<',    0,  false,   true},
    {XML_STATE_LABEL,   XML_STATE_EXIT,     '/',    0,  false,   true},
    {XML_STATE_AWAIT,   XML_STATE_EXIT,     '/',    0,  false,   true},
    {XML_STATE_EXIT,    XML_STATE_BODY,     '>',  - 1,  false,   true},

    {XML_STATE_LABEL,   XML_STATE_IGNORE,   '!',    0,  false,   true},
    {XML_STATE_LABEL,   XML_STATE_IGNORE,   '?',    0,  false,   true},
    {XML_STATE_IGNORE,  XML_STATE_BODY,     '>',    0,  false,   true},

    {XML_STATE_AWAIT,   XML_STATE_ANAME,   '\0',  + 1,   true,   true},
    {XML_STATE_ANAME,   XML_STATE_EQUAL,    '=',    0,  false,   true},
    {XML_STATE_EQUAL,   XML_STATE_AVALUE,   '"',    0,  false,  false},
    {XML_STATE_AVALUE,  XML_STATE_AWAIT,    '"',  - 1,  false,   true},
    {XML_STATE_AWAIT,   XML_STATE_BODY,     '>',    0,  false,   true},

    {XML_STATE_LABEL,   XML_STATE_NAME,    '\0',  + 1,   true,  false},
    {XML_STATE_NAME,    XML_STATE_AWAIT,   '\1',    0,  false,   true},
    {XML_STATE_NAME,    XML_STATE_BODY,     '>',    0,  false,   true},

    {XML_STATE_BODY,    XML_STATE_STRING,  '\0',  + 1,   true,  false},
    {XML_STATE_STRING,  XML_STATE_BODY,    '\1',  - 1,  false,   true},
    {XML_STATE_STRING,  XML_STATE_LABEL,    '<',  - 1,  false,  false},
};

static inline bool is_record(enum xml_state state)
{
    return XML_STATE_NAME <= state && state <= XML_STATE_STRING;
}

static inline bool is_separator(const char ch)
{
    return !isprint(ch) || ch == ' ';
}

static inline const char *skip_lack(const char *string)
{
    while (*string && (!isprint(*string) || *string == ' '))
        string++;
    return string;
}

state xml_parse(const char *buff, struct xml_node **root, gfp_t gfp)
{
    enum xml_state nstate, cstate = XML_STATE_BODY;
    struct xml_node *nstack[PASER_NODE_DEPTH];
    struct xml_node *parent, *node;
    unsigned int tpos = 0, tsize = PASER_TEXT_DEF;
    char *tbuff, *nblock;
    int nnpos = 0, cnpos = 0;
    state retval = -ENOERR;
    const char *walk;
    bool cross, slack = true;

    tbuff = kmalloc(tsize, gfp);
    if (!tbuff)
        return -ENOMEM;

    node = kzalloc(sizeof(*node), gfp);
    if (!node) {
        kfree(tbuff);
        return -ENOMEM;
    }

    node->name = kstrdup("root", gfp);
    list_head_init(&node->child);

    for (walk = buff; (!slack || (walk = skip_lack(walk))) && *walk; ++walk) {
        const struct xml_transition *major, *minor = NULL;
        unsigned int count;

        for (count = 0; count < ARRAY_SIZE(transition_table); ++count) {
            major = &transition_table[count];
            if (major->form == cstate) {
                if (major->code == *walk) {
                    count = 0;
                    break;
                } else if (major->code == '\1') {
                    if (is_separator(*walk))
                        minor = major;
                } else if (!major->code)
                    minor = major;
            }
        }

        if (count && minor)
            major = minor;
        else if (count)
            major = NULL;

        if (major) {
            nnpos += major->stack;
            nstate = major->to;
            cross = major->cross;
            slack = major->slack;
        }

        if (nnpos >= PASER_NODE_DEPTH) {
            retval = -EOVERFLOW;
            goto error;
        }

        if (nnpos < cnpos && nnpos < 0)
            break;
        else if (nnpos > cnpos) {
            parent = node;
            node = kzalloc(sizeof(*node), gfp);
            if (!node) {
                retval = -ENOMEM;
                goto error;
            }
            nstack[cnpos] = parent;
            node->parent = parent;
            list_add_prev(&nstack[cnpos]->child, &node->sibling);
            list_head_init(&node->child);
        }

        if (is_record(cstate) && !is_record(nstate)) {
            tbuff[tpos] = '\0';
            switch (cstate) {
                case XML_STATE_NAME:
                    node->name = kstrdup(tbuff, gfp);
                    if (!node->name) {
                        retval = -ENOMEM;
                        goto error;
                    }
                    xml_set_object(node);
                    break;

                case XML_STATE_ANAME:
                    node->attr_name = kstrdup(tbuff, gfp);
                    if (!node->string) {
                        retval = -ENOMEM;
                        goto error;
                    }
                    break;

                case XML_STATE_AVALUE:
                    node->attr_value = kstrdup(tbuff, gfp);
                    if (!node->attr_value) {
                        retval = -ENOMEM;
                        goto error;
                    }
                    xml_set_attribute(node);
                    break;

                case XML_STATE_STRING:
                    node->string = kstrdup(tbuff, gfp);
                    if (!node->string) {
                        retval = -ENOMEM;
                        goto error;
                    }
                    xml_set_string(node);
                    break;

                default:
                    break;
            }
            tpos = 0;
        } else if (cross || is_record(cstate)) {
            if (unlikely(tpos >= tsize)) {
                tsize *= 2;
                nblock = krealloc(tbuff, tsize, gfp);
                if (!nblock) {
                    kfree(tbuff);
                    retval = -ENOMEM;
                    goto error;
                }
                tbuff = nblock;
            }
            tbuff[tpos++] = *walk;
            cross = false;
        }

        if (nnpos < cnpos)
            node = nstack[nnpos];

        cnpos = nnpos;
        cstate = nstate;
    }

error:
    kfree(tbuff);
    if (cnpos > 0)
        node = *nstack;

    if (retval)
        xml_release(node);
    else if (root)
        *root = node;

    return retval;
}
EXPORT_SYMBOL(xml_parse);

static int encode_depth(struct xml_node *parent, char *buff, int size, int len, unsigned int depth)
{
    #define xml_sprintf(fmt, ...) len += snprintf(buff + len, max(0, size - len), fmt, ##__VA_ARGS__)
    struct xml_node *child, *attr;
    unsigned int count;
    int save;

    list_for_each_entry(child, &parent->child, sibling) {
        if (xml_test_attribute(child))
            continue;
        for (count = 0; count < depth; ++count)
            xml_sprintf("\t");
        if (xml_test_string(child))
            xml_sprintf("%s\n", child->attr_name);
        else if (xml_test_object(child)) {
            xml_sprintf("<%s", child->name);
            list_for_each_entry(attr, &child->child, sibling) {
                if (xml_test_attribute(attr)) {
                    xml_sprintf("\n");
                    for (count = 0; count < depth; ++count)
                        xml_sprintf("\t");
                    xml_sprintf(" %s=\"%s\"", attr->attr_name, attr->attr_value);
                }
            }
            xml_sprintf(">\n");

            save = len;
            len = encode_depth(child, buff, size, len, depth + 1);

            if (len == save) {
                len -= 2;
                xml_sprintf("/>\n");
            } else {
                for (count = 0; count < depth; ++count)
                    xml_sprintf("\t");
                xml_sprintf("</%s>\n", child->name);
            }
        }
    }

    return len;
}

int xml_encode(struct xml_node *root, char *buff, int size)
{
    return encode_depth(root, buff, size, 0, 0) + 1;
}
EXPORT_SYMBOL(xml_encode);

void xml_release(struct xml_node *root)
{
    struct xml_node *node, *tmp;

    if (unlikely(!root))
        return;

    list_for_each_entry_safe(node, tmp, &root->child, sibling) {
        list_del(&node->sibling);
        if (xml_test_object(node)) {
            xml_release(node);
            continue;
        }
        if (xml_test_string(node))
            kfree(node->string);
        else if (xml_test_attribute(node)) {
            kfree(node->attr_name);
            kfree(node->attr_value);
        }
        kfree(node);
    };

    kfree(root->name);
    kfree(root);
}
EXPORT_SYMBOL(xml_release);
