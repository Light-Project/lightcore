#include "parser.h"
#include "list.h"
#include "token.h"
#include <stdio.h>
#include <printk.h>
#include <stdlib.h>

static Node *primary(Token **rest, Token *tok, mpool_t *pool);
static Node *mul(Token **rest, Token *tok, mpool_t *pool);
static Node *expr(Token **rest, Token *tok, mpool_t *pool);


/*
    + - * / ! ~

    unary
    - +

    >= > <= <

    || &&

    | & ^

    shift
    << >>


    // do we need this?
    +, +=
    ++, --
    ? :
*/
/*

stmt  = ...| ident ":" stmt｜  "{" compound-stmt | expr-stmt

compound-stmt = (typedef | declaration | stmt)* "}"

expr_stmt = expr? ":"

expr = assign ("," expr)?

assign = conditional (assign-op assign)?

assign-op = "=" | "+=" ....

conditional = logor ("?" expr? ":" conditional)?

logor = logand ("||" logand)*

logand = bitor ("&&" bitor)*

bitor = bitxor ("|" bitxor)*

bitxor = bitand ("^" bitand)*

bitand = equality ("&" equality)*

equality = relational ("==" relational | "!=" relational)*

relational = shift ("<" shift | "<=" shift | ">" shift | ">=" shift)*

 shift = add ("<<" add | ">>" add)*

add = mul ("+" mul | "-" mul)*

mul = cast ("\*" cast | "/" cast | "%" cast)*

cast = "(" type-name ")" cast | unary

unary = ("+" | "-" | "*" | "&" | "!" | "~") cast | ("++" | "--") unary | "&&" ident | postfix

postfix  = "(" type-name ")" "{" initializer-list "}"

​		 	=  ident "(" func-args ")" postfix-tail* ｜ primary postfix-tail*

postfix-tail = "[" expr "]" | "(" func-args ")"| "." ident| "->" ident| "++"| "--"

primary  "(" "{" stmt+ "}" ")" | "(" expr ")"|| "sizeof" "(" type-name ")"|| "sizeof" unary|"_Alignof" "(" type-name ")"| "_Alignof" unary| ident| str| num



for example
x = 3;
conditional (assign-op assign) ==> primary assign-op primary



*/

// static void error(char *fmt, ...) {
//   va_list ap;
//   va_start(ap, fmt);
//   vfprintf(stderr, fmt, ap);
//   fprintf(stderr, "\n");
//   exit(1);
// }

// // Reports an error location and exit.
// static void verror_at(char *loc, char *fmt, va_list ap) {
//   int pos = loc - current_input;
//   fprintf(stderr, "%s\n", current_input);
//   fprintf(stderr, "%*s", pos, ""); // print pos spaces.
//   fprintf(stderr, "^ ");
//   vfprintf(stderr, fmt, ap);
//   fprintf(stderr, "\n");
//   exit(1);
// }

// static void error_at(char *loc, char *fmt, ...) {
//   va_list ap;
//   va_start(ap, fmt);
//   verror_at(loc, fmt, ap);
// }

// static void error_tok(Token *tok, char *fmt, ...) {
//   va_list ap;
//   va_start(ap, fmt);
//   verror_at(tok->loc, fmt, ap);
// }



// static void error_log(struct kshell_context *ctx, char *fmt, ...) {
//     va_list ap;
//     va_start(ap, fmt);

//     vprintk(fmt, ap);

//     exit(-1);
// }

static bool equal(Token *tok, char *op) {
    return memcmp(tok->loc, op, tok->len) == 0 && op[tok->len] == '\0';
}

static Token *skip(Token *tok, char *s) {
    if (!equal(tok, s)) {
        return NULL;
    }
    return list_next_entry(tok, list);
}


static Node *new_node(NodeKind kind, mpool_t *pool) {
    Node *node = mpool_alloc(pool, sizeof(Node));
    if (node == NULL) {
        return NULL;
    }

    node->kind = kind;
    return node;
}

static Node *new_num(int val, mpool_t *pool) {
    Node *node = new_node(ND_NUM, pool);
    node->val = val;
    return node;
}

static Node *new_unary(NodeKind kind, Node *expr, mpool_t *pool) {
    Node *node = new_node(kind, pool);
    node->lhs = expr;
    return node;
}

static Node *new_binary(NodeKind kind, Node *lhs, Node *rhs, mpool_t *pool) {
    Node *node = new_node(kind, pool);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static Node *new_add(Node *lhs, Node *rhs, mpool_t *pool) {
    return new_binary(ND_ADD, lhs, rhs, pool);
}

static Node *new_sub(Node *lhs, Node *rhs, mpool_t *pool) {
    return new_binary(ND_SUB, lhs, rhs, pool);
}

static Node *new_mul(Node *lhs, Node *rhs, mpool_t *pool) {
    return new_binary(ND_MUL, lhs, rhs, pool);
}

static Node *new_div(Node *lhs, Node *rhs, mpool_t *pool) {
    return new_binary(ND_DIV, lhs, rhs, pool);
}

static Node *new_mod(Node *lhs, Node *rhs, mpool_t *pool) {
    return new_binary(ND_MOD, lhs, rhs, pool);
}

//primary is the highest priority
static Node *primary(Token **rest, Token *tok, mpool_t *pool) {
    if (equal(tok, "(")) {
        Node *node = expr(&tok, list_next_entry(tok, list), pool);
        *rest = skip(tok, ")");
        return node;
    }

    if (tok->kind == TK_NUM) {
        Node *node = new_num(tok->val, pool);

        *rest = list_next_entry(tok, list);
        return node;
    }

    pr_warn("expected an expression\n");
    return NULL;
}

static Node *unary(Token **rest, Token *tok, mpool_t *pool) {
    Node *node;
    if (equal(tok, "+")) {
        return mul(rest, tok, pool);
    }

    if (equal(tok, "-")) {
        node = mul(rest, list_next_entry(tok, list), pool);
        return new_unary(ND_NEG, node, pool);
    }

    if (equal(tok, "!")) {
        node = mul(rest, list_next_entry(tok, list), pool);
        return new_unary(ND_NOT, node, pool);
    }

    if (equal(tok, "~")) {
        node = mul(rest, list_next_entry(tok, list), pool);
        return new_unary(ND_BITNOT, node, pool);
    }

    return primary(rest, tok, pool);
}

static Node *mul(Token **rest, Token *tok, mpool_t *pool) {
    Node *node = unary(&tok, tok, pool);

    for (;;) {
        if (equal(tok, "*")) {
            node = new_mul(node, unary(&tok, list_next_entry(tok, list), pool), pool);
            continue;
        }

        if (equal(tok, "/")) {
            node = new_div(node, unary(&tok, list_next_entry(tok, list), pool), pool);
            continue;
        }

        if (equal(tok, "%")) {
            node = new_mod(node, unary(&tok, list_next_entry(tok, list), pool), pool);
            continue;
        }

        *rest = tok;
        return node;
    }
}

static Node *add(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;

    node = mul(&tok, tok, pool);

    for (;;) {

        if (equal(tok, "+")) {
            rnode = mul(&tok, list_next_entry(tok, list), pool);

            node = new_add(node, rnode, pool);
            continue;
        }

        if (equal(tok, "-")) {
            rnode = mul(&tok, list_next_entry(tok, list), pool);
            node = new_sub(node, rnode, pool);
            continue;
        }

        *rest = tok;
        return node;
    }
}

static Node *shift(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = add(&tok, tok, pool);

    for (;;) {

        if (equal(tok, "<<")) {
            rnode = add(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_SHL, node, rnode, pool);
        }

        if (equal(tok, ">>")) {
            rnode = add(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_SHR, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }
}

static Node *relational(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = shift(&tok, tok, pool);

    for (;;) {

        if (equal(tok, "<")) {
            rnode = shift(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_LT, node, rnode, pool);
        }

        if (equal(tok, "<=")) {
            rnode = shift(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_LE, node, rnode, pool);
        }

        if (equal(tok, ">")) {
            rnode = shift(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_LT, rnode, node, pool);
        }

        if (equal(tok, ">=")) {
            rnode = shift(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_LE, rnode, node, pool);
        }

        *rest = tok;
        return node;
    }
}

static Node *equality(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = relational(&tok, tok, pool);

    for (;;) {

        if (equal(tok, "==")) {
            rnode = relational(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_EQ, node, rnode, pool);
        }

        if (equal(tok, "!=")) {
            rnode = relational(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_NE, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }
}


static Node *bitand(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = equality(&tok, tok, pool);


    for (;;) {
        if (equal(tok, "&")) {
            rnode = equality(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_BITAND, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }
}

static Node *bitxor(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = bitand(&tok, tok, pool);


    for (;;) {
        if (equal(tok, "^")) {
            rnode = bitand(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_BITXOR, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }
}


static Node *bitor(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = bitxor(&tok, tok, pool);


    for (;;) {
        if (equal(tok, "|")) {
            rnode = bitxor(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_BITOR, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }
}

static Node *logand(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = bitor(&tok, tok, pool);

    for (;;) {
        if (equal(tok, "&&")) {
            rnode = bitor(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_LOGAND, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }
}

static Node *logor(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = logand(&tok, tok, pool);

    for (;;) {
        if (equal(tok, "||")) {
            rnode = logand(&tok, list_next_entry(tok, list), pool);

            node = new_binary(ND_LOGOR, node, rnode, pool);
        }

        *rest = tok;
        return node;
    }

}

static Node *conditional(Token **rest, Token *tok, mpool_t *pool) {
    Node *cond, *node;
    cond = logor(&tok, tok, pool);

    if (!equal(tok, "?")) {
        *rest = tok;
        return cond;
    }

    node = new_node(ND_COND, pool);

    node->cond = cond;
    node->then = expr(&tok, list_next_entry(tok, list), pool);

    tok = skip(tok, ":");
    node->els = conditional(rest, tok, pool);

    return node;

}

static Node *expr(Token **rest, Token *tok, mpool_t *pool) {
    Node *node, *rnode;
    node = conditional(&tok, tok, pool);

    if (equal(tok, ",")) {
        rnode = expr(rest, list_next_entry(tok, list), pool);

        return new_binary(ND_COMMA, node, rnode, pool);
    }

    *rest = tok;
    return node;
}


Node* parser(Token *tok, mpool_t *pool) {
    Node *node;
    node =  expr(&tok, tok, pool);

    if (tok->kind != TK_EOF) {
        pr_warn("expected an eof token\n");
        return NULL;
    }


    return node;
}


int compute(Node *node) {

    if (node->kind == ND_NUM) {
        return node->val;
    }

    if (node->kind == ND_NEG) {
        return -compute(node->lhs);
    }

    if (node->kind == ND_NOT) {
        return !compute(node->lhs);
    }

    if (node->kind == ND_BITNOT) {
        return ~compute(node->lhs);
    }

    if (node->kind == ND_EQ) {
        return compute(node->lhs) == compute(node->rhs);
    }

    if (node->kind == ND_NE) {
        return compute(node->lhs) != compute(node->rhs);
    }

    if (node->kind == ND_LE) {
        return compute(node->lhs) <= compute(node->rhs);
    }

    if (node->kind == ND_LT) {
        return compute(node->lhs) < compute(node->rhs);
    }

    if (node->kind == ND_SHL) {
        return compute(node->lhs) << compute(node->rhs);
    }

    if (node->kind == ND_SHR) {
        return compute(node->lhs) >> compute(node->rhs);
    }

    if (node->kind == ND_MUL) {
        return compute(node->lhs) * compute(node->rhs);
    }

    if (node->kind == ND_DIV) {
        return compute(node->lhs) / compute(node->rhs);
    }

    if (node->kind == ND_MOD) {
        return compute(node->lhs) % compute(node->rhs);
    }

    if (node->kind == ND_ADD) {
        return compute(node->lhs) + compute(node->rhs);
    }

    if (node->kind == ND_SUB) {
        return compute(node->lhs) - compute(node->rhs);
    }

    if (node->kind == ND_BITAND) {
        return compute(node->lhs) & compute(node->rhs);
    }

    if (node->kind == ND_BITXOR) {
        return compute(node->lhs) ^ compute(node->rhs);
    }

    if (node->kind == ND_BITOR) {
        return compute(node->lhs) | compute(node->rhs);
    }

    if (node->kind == ND_LOGAND) {
        return compute(node->lhs) && compute(node->rhs);
    }

    if (node->kind == ND_LOGOR) {
        return compute(node->lhs) || compute(node->rhs);
    }

    if (node->kind == ND_COMMA) {
        return compute(node->rhs);
    }

    if (node->kind == ND_COND) {
        if (compute(node->cond)) {
            return compute(node->then);
        }
        return compute(node->els);
    }

    return 0;
}
