/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <initcall.h>
#include <xml.h>
#include <selftest.h>

static const char xml_test[] = {
    "<!-- (C) 1999, 2000 WAP Forum Ltd.  All rights reserved -->\n"
    "<wml>\n"
    "<card id=\"card1\">\n"
    "<onevent type=\"onenterforward\">\n"
    "<go href=\"/vswap/run/result.eml\">\n"
    "        <postfield name=\"var\" value=\"&#36;test\"/>\n"
    "        <postfield name=\"v\" value=\"dark\"/>\n"
    "        <postfield name=\"ts\" value=\"0003\"/>\n"
    "        <postfield name=\"tp\" value=\"wml/state/variables/parsing/1\"/>\n"
    "        <postfield name=\"ti\" value=\"1\"/>\n"
    "        <postfield name=\"expected\" value=\"var:pass\"/>\n"
    "</go>\n"
    "</onevent>\n"
    "<p>If automatic testing failed, select <anchor>Failed<go\n"
    "href='/vswap/run/result.eml'>\n"
    "        <postfield name=\"SUBMIT\" value=\"No\"/><postfield name=\"v\"\n"
    "value=\"dark\"/>\n"
    "        <postfield name=\"ts\" value=\"0003\"/>\n"
    "        <postfield name=\"tp\" value=\"wml/state/variables/parsing/1\"/>\n"
    "        <postfield name=\"ti\" value=\"1\"/>\n"
    "        <postfield name=\"expected\" value=\"var:pass\"/></go></anchor>.</p>\n"
    "</card>\n"
    "</wml>\n"
};

static void xml_dumpinfo(struct kshell_context *ctx, struct xml_node *parent, unsigned int depth)
{
    struct xml_node *child;
    unsigned int count;

    kshell_printf(ctx, "object: %s {\n", parent->name);
    list_for_each_entry(child, &parent->child, sibling) {
        for (count = 0; count < depth; ++count)
            kshell_putc(ctx, '\t');
        if (xml_test_object(child)) {
            xml_dumpinfo(ctx, child, depth + 1);
            continue;
        }
        if (xml_test_attribute(child))
            kshell_printf(ctx, "attribute: %s=%s", child->attr_name, child->attr_value);
        else if (xml_test_string(child))
            kshell_printf(ctx, "string: %s", child->attr_name);
        kshell_putc(ctx, '\n');
    }

    for (count = 0; count < depth - 1; ++count)
        kshell_putc(ctx, '\t');
    kshell_puts(ctx, "}\n");
}

static state xml_test_testing(struct kshell_context *ctx, void *pdata)
{
    struct xml_node *xnode;
    state retval;
    char *buff;
    int length;

    retval = xml_parse(xml_test, &xnode, GFP_KERNEL);
    if (retval)
        return retval;

    kshell_puts(ctx, "pseudo expression:\n");
    xml_dumpinfo(ctx, xnode, 1);

    kshell_puts(ctx, "xml encode:\n");
    length = xml_encode(xnode, NULL, 0);

    buff = kmalloc(length, GFP_KERNEL);
    if (!buff) {
        retval = -ENOMEM;
        goto finish;
    }

    length = xml_encode(xnode, buff, length);
    kshell_write(ctx, buff, length);
    kfree(buff);

finish:
    xml_release(xnode);
    return retval;
}

static struct selftest_command xml_test_command = {
    .group = "lib",
    .name = "xml",
    .desc = "xml unit test",
    .testing = xml_test_testing,
};

static state selftest_xml_init(void)
{
    return selftest_register(&xml_test_command);
}
kshell_initcall(selftest_xml_init);
