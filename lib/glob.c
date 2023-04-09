/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <glob.h>
#include <export.h>

bool __pure glob_match(const char *glob, const char *string)
{
    const char *class, *bstring, *bglob = NULL;
    char glch, stch, tcha, tchb;
    bool match, inverted;

    for (;;) {
        glch = *glob++;
        stch = *string++;

        switch (glch) {
            case '?':
                if (stch == '\0')
                    return false;
                break;

            case '*':
                if (*glob == '\0')
                    return true;
                bglob = glob;
                bstring = --string;
                break;

            case '[':
                match = false;
                inverted = *glob == '!';
                class = glob + inverted;
                tcha = *class++;

                do {
                    if ((tchb = tcha) == '\0')
                        goto literal;

                    if (class[0] == '-' && class[1] != ']') {
                        tchb = class[1];
                        if (tchb == '\0')
                            goto literal;
                        class += 2;
                    }
                    match |= tcha <= stch && stch <= tchb;
                } while ((tcha = *class++) != ']');

                if (match == inverted)
                    goto backtrack;

                glob = class;
                break;

            case '\\':
                glch = *glob++;
                fallthrough;

            default: literal:
                if (glch == stch) {
                    if (glch == '\0')
                        return true;
                    break;
                }

            backtrack:
                if (stch == '\0' || !bglob)
                    return false;

                glob = bglob;
                string = ++bstring;
                break;
        }
    }
}
EXPORT_SYMBOL(glob_match);
