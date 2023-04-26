/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2022 John Sanpe <sanpeqf@gmail.com>
 */

#include <kshell.h>
#include <initcall.h>
#include <ctype.h>
#include <string.h>
#include <kmalloc.h>

static void usage(struct kshell_context *ctx)
{
    kshell_puts(ctx, "usage: math [option] \"grammar\" ...\n");
    kshell_puts(ctx, "\t-x  output to env in hexadecimal\n");
    kshell_puts(ctx, "\t-p  print calculation process\n");
    kshell_puts(ctx, "\t-o  printout results\n");
    kshell_puts(ctx, "\t-h  display this message\n");
}

static inline int is_value(int c)
{
    return isdigit(c) || isalpha(c);
}

static int get_value(struct kshell_context *ctx, char *end, char *varname)
{
    char tmp, *walk, *rvalue = NULL;
    bool neg = false, alnum = false;
    const char *result;
    int value;

    for (walk = varname; walk < end; ++walk) {
        if (!alnum && *walk == '-') {
            rvalue = walk + 1;
            neg = true;
        } else if (is_value(*walk))
            alnum = true;
        else
            break;
    }

    tmp = *walk;
    *walk = '\0';

    result = kshell_getenv(ctx, rvalue ?: varname);
    value = axtoi(result ?: rvalue ?: varname);

    *walk = tmp;
    return neg ? -value : value;
}

static void set_value(struct kshell_context *ctx, char *end, char *varname, int value)
{
    char *walk;
    char tmp, str[32];

    for (tmp = 0, walk = varname; walk <= end; ++walk) {
        if (!is_value(*walk) || walk == end) {
            tmp = *walk;
            *walk = '\0';
            break;
        }
    }

    if (kshell_getenv(ctx, varname)) {
        itoad(value, str);
        kshell_setenv(ctx, varname, str, true);
    }

    if (tmp)
        *walk = tmp;
}

static char *prev_number(struct kshell_context *ctx, char *start, char *end,
                         char *curr, int *number, char **region)
{
    bool alnum = false;
    char *varname;
    int result;

    for (result = 0; curr >= start; --curr) {
        if (alnum && !(is_value(*curr) || *curr == '-')) {
            result = get_value(ctx, end, varname);
            break;
        } else if (!alnum) {
            if (is_value(*curr))
                alnum = true;
            else if (!isspace(*curr))
                return NULL;
        }
        varname = curr;
    }

    if (alnum && curr < start)
        result = get_value(ctx, end, start);

    if (region)
        *region = curr;

    if (number)
        *number = result;

    return alnum ? curr : NULL;
}

static char *next_number(struct kshell_context *ctx, char *end,
                         char *curr, int *number, char **region)
{
    bool alnum = false;
    int result;

    for (result = 0; curr < end; ++curr) {
        if (alnum && !is_value(*curr))
            break;
        else if (!alnum) {
            if (is_value(*curr) || *curr == '-') {
                alnum = true;
                if (region)
                    *region = curr;
                result = get_value(ctx, end, curr);
            } else if (!isspace(*curr))
                return NULL;
        }
    }

    if (number)
        *number = result;

    return alnum ? curr : NULL;
}

static state string_extension(char **start, char **end, char **curr, size_t size, size_t ext)
{
    char *nblock, *ncurr;
    size_t nsize;

    if (size >= ext) {
        memset(*curr, ' ', size);
        return -ENOERR;
    }

    nsize = (*end - *start) + (ext - size);
    nblock = kzalloc(nsize, GFP_KERNEL);
    if (!nblock)
        return -ENOMEM;

    ncurr = nblock + (*curr - *start);
    memcpy(nblock, *start, *curr - *start);
    memcpy(ncurr + ext, *curr + size, (*end - *curr) - size);

    if (ksize(start))
        kfree(start);

    *start = nblock;
    *end = nblock + nsize;
    *curr = ncurr;

    return -ENOERR;
}

static int math_parser(struct kshell_context *ctx, char *start, char *end, bool proc)
{
    char *curr, *error = NULL;
    int plen, result;


    /* Principal priority */
    for (curr = start; (curr = strchr(curr, '(')); ++curr) {
        char *walk = curr + 1;
        unsigned int stack;

        for (stack = 1; *walk; ++walk) {
            if (*walk == '(')
                ++stack;
            else if (*walk == ')') {
                if (!--stack)
                    break;
            }
        }

        if (stack) {
            error = curr;
            goto error;
        }

        result = math_parser(ctx, curr + 1, walk, proc);
        plen = snprintf(NULL, 0, " %d", result);
        string_extension(&start, &end, &curr, walk - curr + 1, plen + 1);
        snprintf(curr, plen + 1, " %d", result);
        curr[plen] = ' ';
    }

    if (proc)
        kshell_printf(ctx, "P01: %.*s\n", end - start, start);

    /* Secondary priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;

        if (!strncmp(curr, "++", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, &right))) {
            /* variable++ */
            left++;
            set_value(ctx, end, left, result + 1);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = (curr + 2) - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "--", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, &right))) {
            /* variable-- */
            left++;
            set_value(ctx, end, left, result - 1);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = (curr + 2) - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "++", 2) && (right = next_number(ctx, end, curr + 2, &result, &left))) {
            /* ++variable */
            result++;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &curr, (size = right - curr), (ext = plen + 1));
            snprintf(curr, plen + 1, "%d", result);
            curr[plen] = ' ';
            curr += max(size, ext) - 1;

        } else if (!strncmp(curr, "--", 2) && (right = next_number(ctx, end, curr + 2, &result, &left))) {
            /* --variable */
            result--;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &curr, (size = right - curr), (ext = plen + 1));
            snprintf(curr, plen + 1, "%d", result);
            curr[plen] = ' ';
            curr += max(size, ext) - 1;

        } else if (*curr == '~' && (right = next_number(ctx, end, curr + 1, &result, NULL))) {
            /* ~variable */
            plen = snprintf(NULL, 0, "%d", ~result);
            string_extension(&start, &end, &curr, (size = right - curr), (ext = plen + 1));
            snprintf(curr, plen + 1, "%d", ~result);
            curr[plen] = ' ';
            curr += max(size, ext) - 1;

        } else if (*curr == '!' && (right = next_number(ctx, end, curr + 1, &result, NULL))) {
            /* !variable */
            plen = snprintf(NULL, 0, "%d", !result);
            string_extension(&start, &end, &curr, (size = right - curr), (ext = plen + 1));
            snprintf(curr, plen + 1, "%d", !result);
            curr[plen] = ' ';
            curr += max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P02: %.*s\n", end - start, start);

    /* Tertiary priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (*curr == '*' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                         && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable * variable */
            left++;
            result *= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (*curr == '/' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable / variable */
            if (!rresult) {
                error = curr;
                goto error;
            }
            left++;
            result /= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (*curr == '%' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable % variable */
            if (!rresult) {
                error = curr;
                goto error;
            }
            left++;
            result %= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P03: %.*s\n", end - start, start);

    /* Quartus priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (*curr == '+' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                         && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable + variable */
            left++;
            result += rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (*curr == '-' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable - variable */
            left++;
            result -= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P04: %.*s\n", end - start, start);

    /* Fifth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (!strncmp(curr, "<<", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                    && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable << variable */
            left++;
            result <<= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, ">>", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable >> variable */
            left++;
            result >>= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P05: %.*s\n", end - start, start);

    /* Sixth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (!strncmp(curr, ">=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                    && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable >= variable */
            left++;
            result = result >= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "<=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable <= variable */
            left++;
            result = result <= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (*curr == '>' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable < variable */
            left++;
            result = result > rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (*curr == '<' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable < variable */
            left++;
            result = result < rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P06: %.*s\n", end - start, start);

    /* Seventh priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (!strncmp(curr, "==", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                    && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable == variable */
            left++;
            result = result == rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "!=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable != variable */
            left++;
            result = result != rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P07: %.*s\n", end - start, start);

    /* Eighth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (*curr == '&' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                         && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable & variable */
            left++;
            result &= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P08: %.*s\n", end - start, start);

    /* Ninth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (*curr == '^' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                         && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable < variable */
            left++;
            result ^= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P09: %.*s\n", end - start, start);

    /* Tenth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (*curr == '|' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                         && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable | variable */
            left++;
            result |= rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P10: %.*s\n", end - start, start);

    /* Eleventh priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (!strncmp(curr, "&&", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                    && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable && variable */
            left++;
            result = result && rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P11: %.*s\n", end - start, start);

    /* Twelfth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (!strncmp(curr, "||", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                    && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable || variable */
            left++;
            result = result || rresult;
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P12: %.*s\n", end - start, start);

    /* Thirteenth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        uintptr_t sindex, eindex;
        size_t size, ext;
        int mresult, rresult;

        if (*curr == '?' && (left = prev_number(ctx, start, end, curr - 1, NULL, NULL))) {

            sindex = curr - start;
            curr = strchr(curr, ':');
            if (!curr) {
                error = start + sindex;
                goto error;
            }
            eindex = curr - start;

            do {
                if (*curr == '?' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))) {
                    /* variable ? variable : variable */
                    right = strchr(curr, ':');
                    if (!right) {
                        error = curr;
                        goto error;
                    }

                    right++;
                    if (!(right = next_number(ctx, end, right, &rresult, NULL))) {
                        error = curr;
                        goto error;
                    }

                    if (!next_number(ctx, end, curr + 1, &mresult, NULL))
                        mresult = result;

                    left++;
                    result = result ? mresult : rresult;
                    plen = snprintf(NULL, 0, "%d", result);
                    string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
                    snprintf(left, plen + 1, "%d", result);
                    left[plen] = ' ';
                    curr = left;
                }
            } while (curr-- > start + sindex);

            curr = start + eindex;
        }
    }

    if (proc)
        kshell_printf(ctx, "P13: %.*s\n", end - start, start);

    /* Fourteenth priority */
    for (curr = start; curr < end; ++curr) {
        char *left, *right;
        size_t size, ext;
        int rresult;

        if (!strncmp(curr, "+=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                    && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable += variable */
            left++;
            result += rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "-=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable -= variable */
            left++;
            result -= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "*=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable *= variable */
            left++;
            result *= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "/=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable /= variable */
            if (!rresult) {
                error = curr;
                goto error;
            }
            left++;
            result /= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "%=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable %= variable */
            if (!rresult) {
                error = curr;
                goto error;
            }
            left++;
            result %= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "&=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable &= variable */
            left++;
            result &= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "^=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable ^= variable */
            left++;
            result ^= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "|=", 2) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                           && (right = next_number(ctx, end, curr + 2, &rresult, NULL))) {
            /* variable |= variable */
            left++;
            result |= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, "<<=", 3) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                            && (right = next_number(ctx, end, curr + 3, &rresult, NULL))) {
            /* variable <<= variable */
            left++;
            result <<= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (!strncmp(curr, ">>=", 3) && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                            && (right = next_number(ctx, end, curr + 3, &rresult, NULL))) {
            /* variable >>= variable */
            left++;
            result >>= rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;

        } else if (*curr == '=' && (left = prev_number(ctx, start, end, curr - 1, &result, NULL))
                                && (right = next_number(ctx, end, curr + 1, &rresult, NULL))) {
            /* variable = variable */
            left++;
            result = rresult;
            set_value(ctx, end, left, result);
            plen = snprintf(NULL, 0, "%d", result);
            string_extension(&start, &end, &left, (size = right - left), (ext = plen + 1));
            snprintf(left, plen + 1, "%d", result);
            left[plen] = ' ';
            curr = left + max(size, ext) - 1;
        }
    }

    if (proc)
        kshell_printf(ctx, "P14: %.*s\n", end - start, start);

    if (!(curr = next_number(ctx, end, start, &result, NULL))) {
        error = start;
        goto error;
    }

    if (*((error = skip_spaces(curr)) == end ? "" : error)) {
        error = skip_spaces(error);
        goto error;
    }

    if (ksize(start))
        kfree(start);

    return result;

error:
    kshell_printf(ctx, "math: parse error near '%s'\n", error);
    errno = -EINVAL;
    return 0;
}

static state math_main(struct kshell_context *ctx, int argc, char *argv[])
{
    unsigned int count;
    bool pflag = false;
    bool oflag = false;
    int result = 0;

    for (count = 1; count < argc; ++count) {
        char *para = argv[count];

        if (para[0] != '-')
            break;

        switch (para[1]) {
            case 'p':
                pflag = true;
                break;

            case 'o':
                oflag = true;
                break;

            case 'h':
                goto usage;

            default:
                goto finish;
        }
    }

    if (argc == count)
        goto usage;

finish:
    for (; count < argc; ++count) {
        unsigned int len = strlen(argv[count]);
        result = math_parser(ctx, argv[count], argv[count] + len, pflag);
    }

    if (!oflag)
        return result;

    kshell_printf(ctx, "%d\n", result);
    return errno;

usage:
    usage(ctx);
    return -EINVAL;
}

static struct kshell_command math_cmd = {
    .name = "math",
    .desc = "environment variable calculation",
    .exec = math_main,
};

static state math_init(void)
{
    return kshell_register(&math_cmd);
}
kshell_initcall(math_init);
