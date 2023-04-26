#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <printk.h>

static bool startswith(const char *p, const char *q) {
  return strncmp(p, q, strlen(q)) == 0;
}

#if 0
char *token_read_file(char *path) {
    FILE *fp;

    if (strcmp(path, "-") == 0) {
        // By convention, read from stdin if a given filename is "-".
        fp = stdin;
    } else {
        fp = fopen(path, "r");
        if (!fp)
        return NULL;
    }

    char *buf;
    size_t buflen;
    FILE *out = open_memstream(&buf, &buflen);

    // Read the entire file.
    for (;;) {
        char buf2[4096];
        int n = fread(buf2, 1, sizeof(buf2), fp);
        if (n == 0) {
            break;
        }

        fwrite(buf2, 1, n, out);
    }

    if (fp != stdin)
        fclose(fp);

    // Make sure that the last line is properly terminated with '\n'.
    fflush(out);
    if (buflen == 0 || buf[buflen - 1] != '\n')
        fputc('\n', out);
    fputc('\0', out);
    fclose(out);
    return buf;
}
#endif

Token *new_token(tokentype_t type, const char *start, const char *end, mpool_t *pool) {
    Token *t;
    t = mpool_alloc(pool, sizeof(Token));
    if (t == NULL) {
        return NULL;
    }
    t->kind = type;
    t->loc = start;
    t->end = end;
    t->len = end - start;

    return t;
}

Token *new_token_num(const char *start, const char *end, mpool_t *pool) {
    Token *t;
    t = new_token(TK_NUM, start, end, pool);
    if (t == NULL) {
        return NULL;
    }
    //now we assume is int number

    t->val = atoi(start);

    return t;
}

Token *new_token_eof(const char *end, mpool_t *pool) {
    Token *t;
    t = new_token(TK_EOF, end , end, pool);
    if (t == NULL) {
        return NULL;
    }
    return t;
}

Token * new_token_punct(const char *start, const char *end, mpool_t *pool) {
    Token *t;
    t = new_token(TK_PUNCT, start , end, pool);
    if (t == NULL) {
        return NULL;
    }

    return t;
}

static int read_punct(const char *p) {
    static char *kw[] = {
        "<<=", ">>=", "...", "==", "!=", "<=", ">=", "->", "+=",
        "-=", "*=", "/=", "++", "--", "%=", "&=", "|=", "^=", "&&",
        "||", "<<", ">>", "##",
    };

    for (int i = 0; i < sizeof(kw) / sizeof(*kw); i++) {
        if (startswith(p, kw[i])) {
            return strlen(kw[i]);
        }
    }

    return ispunct(*p) ? 1 : 0;

}

Token *tokenize(const char *p, mpool_t *pool) {
    Token *head, *t;
    int punct_len;
    head = new_token(0, NULL, NULL, pool);
    if (head == NULL) {
        return NULL;
    }

    list_head_init(&head->list);


    for (; *p ;) {

        if (startswith(p, "//")) {
            p+=2;
            while (*p != '\n') {
                p++;
            }
            continue;
        }

        // Skip block comments.
        if (startswith(p, "/*")) {
            char *q = strstr(p + 2, "*/");
            if (!q) {
                pr_warn("unclosed block comment\n");
                goto err;
            }
            p = q + 2;
            continue;
        }

        if (*p == '\n') {
            p++;
            continue;
        }

        if (isspace(*p)) {
            p++;
            continue;
        }

        // Numeric literal
        if (isdigit(*p) || (*p == '.' && isdigit(p[1]))) {
            const char *q = p++;

            for (;;) {

                if (p[0] && p[1] && strchr("eEpP", p[0]) && strchr("+-", p[1])) {
                    p += 2;
                }

                else if (isalnum(*p) || *p == '.') {
                    p++;
                }else {
                    break;
                }
            }

            t = new_token_num(q, p, pool);
            if (t == NULL) {
                goto err;
            }

            list_add_prev(&head->list, &t->list);

            continue;
        }

        //keyword

        //Punctuators
        punct_len = read_punct(p);
        if (punct_len) {
            t = new_token_punct(p, p + punct_len, pool);
            if (t == NULL) {
                goto err;
            }

            p += punct_len;
            list_add_prev(&head->list, &t->list);

            continue;
        }

        pr_warn("invalid token\n");
        goto err;
    }

    t = new_token_eof(p, pool);
    if (t == NULL) {
        goto err;
    }
    list_add_prev(&head->list, &t->list);

    return head;
err:

    return NULL;
}

// Token *tokenize_file(char *file, mpool_t *pool) {
//     char *p = token_read_file(file);
//     if (p == NULL) {
//         return NULL;
//     }

//     return tokenize(p, pool);
// }
