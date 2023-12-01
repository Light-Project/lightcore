#ifndef _LOCAL_PARSER_H_
#define _LOCAL_PARSER_H_


#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "token.h"
typedef struct ast_node_s Node;

typedef enum {
    ND_NUM,
    ND_NEG,       // unary -
    ND_NOT,       // unary !
    ND_EQ,        // ==
    ND_NE,        // !=
    ND_LT,        // <
    ND_LE,        // <=
    ND_SHL,       // <<
    ND_SHR,       // >>
    ND_BITNOT,     // unary ~
    ND_MUL,       // *
    ND_DIV,       // /
    ND_MOD,       // %
    ND_ADD,       // +
    ND_SUB,       // -
    ND_BITAND,    // &
    ND_BITXOR,    // ^
    ND_BITOR,     // |
    ND_LOGAND,    // &&
    ND_LOGOR,     // ||
    ND_COMMA,     // ,
    ND_COND,      // ?:
}NodeKind;

struct ast_node_s {
    NodeKind kind;
    union {
        struct {
            Node *lhs;
            Node *rhs;
        };
        struct {
            Node *cond;
            Node *then;
            Node *els;
        };
    };
    long val;
};

Node* parser(Token *tok, mpool_t *pool);

int compute(Node *node);

#endif
