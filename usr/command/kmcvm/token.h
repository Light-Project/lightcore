#ifndef _LOCAL_TOKEN_H_
#define _LOCAL_TOKEN_H_

#include <list.h>
#include <kmalloc.h>
#include <stddef.h>
#include "mpool.h"

typedef struct token Token;

typedef enum tokentype{
    TK_NUM,
    TK_NUM_FLOAT,
    TK_PUNCT,
    TK_KEYWORD,
    TK_EOF,
}tokentype_t;

struct token {
    tokentype_t kind;
    struct list_head list;
    const char *loc;
    const char *end;
    size_t len;
    int val;
    long double fval;
};



Token *tokenize(const char *p, mpool_t *pool);
// Token *tokenize_file(char *file, mpool_t *pool);

#endif
