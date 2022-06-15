/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 Sanpe <sanpeqf@gmail.com>
 */

#include <json.h>
#include <ctype.h>
#include <string.h>
#include <kmalloc.h>
#include <export.h>

#define PASER_TEXT_DEF      64
#define PASER_NODE_DEPTH    32
#define PASER_STATE_DEPTH   36

enum json_state {
    JSON_STATE_NULL     = 0,
    JSON_STATE_ESC      = 1,
    JSON_STATE_BODY     = 2,
    JSON_STATE_COLON    = 3,
    JSON_STATE_WAIT     = 4,
    JSON_STATE_ARRAY    = 5,
    JSON_STATE_OBJECT   = 6,
    JSON_STATE_NAME     = 7,
    JSON_STATE_STRING   = 8,
    JSON_STATE_NUMBER   = 9,
    JSON_STATE_OTHER    = 10,
};

struct json_transition {
    enum json_state form, to;
    char code, ecode;
    int nstack, sstack;
    bool cross;
};

static const struct json_transition transition_table[] = {
    {JSON_STATE_OBJECT,   JSON_STATE_NAME,     '"',   '"',  + 1,  + 1,  false},
    {JSON_STATE_NAME,     JSON_STATE_COLON,    '"',   '"',    0,    0,  false},
    {JSON_STATE_COLON,    JSON_STATE_BODY,     ':',   ':',    0,    0,  false},

    {JSON_STATE_STRING,   JSON_STATE_ESC,     '\\',  '\\',    0,  + 1,  false},
    {JSON_STATE_NAME,     JSON_STATE_ESC,     '\\',  '\\',    0,  + 1,  false},
    {JSON_STATE_ESC,      JSON_STATE_NULL,    '\0',  '\0',    0,  - 1,   true},

    {JSON_STATE_BODY,     JSON_STATE_ARRAY,    '[',   '[',    0,    0,  false},
    {JSON_STATE_BODY,     JSON_STATE_OBJECT,   '{',   '{',    0,    0,  false},
    {JSON_STATE_BODY,     JSON_STATE_NUMBER,   '0',   '9',    0,    0,   true},
    {JSON_STATE_BODY,     JSON_STATE_STRING,   '"',   '"',    0,    0,  false},
    {JSON_STATE_BODY,     JSON_STATE_OTHER,   '\0',  '\0',    0,    0,   true},

    {JSON_STATE_ARRAY,    JSON_STATE_ARRAY,    '[',   '[',  + 1,  + 1,  false},
    {JSON_STATE_ARRAY,    JSON_STATE_OBJECT,   '{',   '{',  + 1,  + 1,  false},
    {JSON_STATE_ARRAY,    JSON_STATE_NUMBER,   '0',   '9',  + 1,  + 1,   true},
    {JSON_STATE_ARRAY,    JSON_STATE_STRING,   '"',   '"',  + 1,  + 1,  false},
    {JSON_STATE_ARRAY,    JSON_STATE_OTHER,   '\0',  '\0',  + 1,  + 1,   true},

    {JSON_STATE_ARRAY,    JSON_STATE_WAIT,     ']',   ']',  - 1,    0,  false},
    {JSON_STATE_OBJECT,   JSON_STATE_WAIT,     '}',   '}',  - 1,    0,  false},
    {JSON_STATE_STRING,   JSON_STATE_WAIT,     '"',   '"',  - 1,    0,  false},

    {JSON_STATE_NUMBER,   JSON_STATE_NULL,     ',',   ',',  - 1,  - 1,  false},
    {JSON_STATE_OTHER,    JSON_STATE_NULL,     ',',   ',',  - 1,  - 1,  false},
    {JSON_STATE_WAIT,     JSON_STATE_NULL,     ',',   ',',    0,  - 1,  false},

    {JSON_STATE_NUMBER,   JSON_STATE_WAIT,     ']',   ']',  - 2,  - 1,  false},
    {JSON_STATE_OTHER,    JSON_STATE_WAIT,     ']',   ']',  - 2,  - 1,  false},
    {JSON_STATE_WAIT,     JSON_STATE_WAIT,     ']',   ']',  - 1,  - 1,  false},
    {JSON_STATE_NUMBER,   JSON_STATE_WAIT,     '}',   '}',  - 2,  - 1,  false},
    {JSON_STATE_OTHER,    JSON_STATE_WAIT,     '}',   '}',  - 2,  - 1,  false},
    {JSON_STATE_WAIT,     JSON_STATE_WAIT,     '}',   '}',  - 1,  - 1,  false},
};

static inline bool is_struct(enum json_state state)
{
    return JSON_STATE_ARRAY <= state && state <= JSON_STATE_OBJECT;
}

static inline bool is_record(enum json_state state)
{
    return JSON_STATE_NAME <= state && state <= JSON_STATE_OTHER;
}

static inline const char *skip_lack(const char *string)
{
    while (!isprint(*string) || *string == ' ')
        string++;
    return string;
}

state json_parse(const char *buff, struct json_node **root, gfp_t gfp)
{
    enum json_state nstate, cstate = JSON_STATE_ARRAY;
    enum json_state sstack[PASER_STATE_DEPTH];
    struct json_node *nstack[PASER_NODE_DEPTH];
    struct json_node *parent, *node = NULL;
    unsigned int tpos = 0, tsize = PASER_TEXT_DEF;
    int nspos = 0, cspos = 0, nnpos = -1, cnpos = -1;
    char *tbuff, *nblock;
    state retval = -ENOERR;
    const char *walk;
    bool cross;

    tbuff = kmalloc(tsize, gfp);
    if (!tbuff)
        return -ENOMEM;

    for (walk = buff; (is_record(cstate) || (walk = skip_lack(walk))) && *walk; ++walk) {
        const struct json_transition *major, *minor = NULL;
        unsigned int count;

        for (count = 0; count < ARRAY_SIZE(transition_table); ++count) {
            major = &transition_table[count];
            if (major->form == cstate) {
                if (major->code <= *walk && *walk <= major->ecode) {
                    count = 0;
                    break;
                } else if (!major->code)
                    minor = major;
            }
        }

        if (count && minor)
            major = minor;
        else if (count)
            major = NULL;

        if (major) {
            nnpos += major->nstack;
            nspos += major->sstack;
            nstate = major->to;
            cross = major->cross;
        }

        if (nnpos >= PASER_NODE_DEPTH || nspos >= PASER_STATE_DEPTH) {
            retval = -EOVERFLOW;
            goto error;
        }

        if (nspos > cspos && cstate != JSON_STATE_NULL)
            sstack[cspos] = cstate;
        else if (nspos < cspos && nstate == JSON_STATE_NULL)
            nstate = sstack[nspos];

        if (nnpos < cnpos && nnpos < 0)
            break;
        else if (nnpos > cnpos) {
            parent = node;
            node = kzalloc(sizeof(*node), gfp);
            if (!node) {
                retval = -ENOMEM;
                goto error;
            }
            if (cnpos >= 0) {
                nstack[cnpos] = parent;
                list_add_prev(&nstack[cnpos]->child, &node->sibling);
            }
            node->parent = parent;
            list_head_init(&node->child);
        }

        switch (*walk) {
            case '[':
                json_set_array(node);
                break;

            case '{':
                json_set_object(node);
                break;

            default:
                break;
        }

        if (nstate != JSON_STATE_ESC) {
            if (is_record(cstate) && !is_record(nstate)) {
                tbuff[tpos] = '\0';
                switch (cstate) {
                    case JSON_STATE_NAME:
                        node->name = strdup(tbuff);
                        if (!node->name) {
                            retval = -ENOMEM;
                            goto error;
                        }
                        break;

                    case JSON_STATE_STRING:
                        node->string = strdup(tbuff);
                        if (!node->string) {
                            retval = -ENOMEM;
                            goto error;
                        }
                        json_set_string(node);
                        break;

                    case JSON_STATE_NUMBER:
                        node->number = atol(tbuff);
                        json_set_number(node);
                        break;

                    case JSON_STATE_OTHER:
                        if (!strcmp(tbuff, "null"))
                            json_set_null(node);
                        else if (!strcmp(tbuff, "true"))
                            json_set_true(node);
                        else if (!strcmp(tbuff, "false"))
                            json_set_false(node);
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
        }

        if (nnpos < cnpos)
            node = nstack[nnpos];

        cnpos = nnpos;
        cspos = nspos;
        cstate = nstate;
    }

error:
    if (cnpos > 0)
        node = *nstack;

    if (retval)
        json_release(node);
    else if (root)
        *root = node;

    return retval;
}
EXPORT_SYMBOL(json_parse);

void json_release(struct json_node *root)
{
    struct json_node *node, *tmp;

    if (unlikely(!root))
        return;

    list_for_each_entry_safe(node, tmp, &root->child, sibling) {
        if (json_test_array(node) || json_test_object(node))
            json_release(node);
        if (node->name)
            kfree(node->name);
        if (json_test_string(node))
            kfree(node->string);
        list_del(&node->sibling);
        kfree(node);
    }
}
EXPORT_SYMBOL(json_release);
