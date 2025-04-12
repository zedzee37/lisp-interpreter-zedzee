#include "expr.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <parser.h>

// parse parses the entire string into expressions, whilst parseExpr parses a single expression

void freeExpr(Expr *expr) {
    ListExpr *list = (ListExpr *)expr;
    IdentifierExpr *identifier = (IdentifierExpr *)expr;
    LiteralExpr *literal = (LiteralExpr *)expr;

    switch (expr->type) {
        case LITERAL:
            switch (literal->type) {
                case STRING:
                    free(literal->data.string);
                    break;
                default:
                    break;
            }
            break;
        case LIST:
            // Free all exprs
            free(list->exprs);
            break;
        case IDENTIFIER:
            free(identifier->name);
            break;
    }
}

void consumeWhitespace(Parser *parser) {
    while (parser->current < parser->sourceLength) {
        char ch = parser->source[parser->current];

        switch (ch) {
            case ' ':
                break;
            case '\t':
                break;
            case '\r':
                break;
            case '\n':
                parser->line++;
                break;
            default:
                return;
        }

        parser->current++;
    }
}

ListExpr parseListExpr(Parser *parser) {
    parser->current++; // consume the left parenthesis
    consumeWhitespace(parser);

    // error
    if (parser->source[parser->current] == ')') {
        return (ListExpr){};
    }

    ListExpr expr;
    expr.base = (Expr){ .type = LIST };

    expr.exprsSize = 2;
    expr.exprsCount = 0;

    expr.exprs = calloc(expr.exprsSize, sizeof(Expr **));

    while (true) {
        Expr *expr = parseExpr(parser);

        if (!expr) {
        }

        // look for a comma, if theres a comma, continue, if not, look for a ')'
        char ch = parser->source[parser->current];
        if (ch == ',') {
            consumeWhitespace(parser);
            continue;
        }

        if (ch == ')') {
            break;
        }

        // error here
    }

    return expr;
}

LiteralExpr parseString(Parser *parser) {
}

LiteralExpr parseNumber(Parser *parser) {

}

IdentifierExpr parseIdentifier(Parser *parser) {

}

void addExpr(Parser *parser, Expr *expr) {
    if (parser->exprsCount >= parser->exprsSize) {
        parser->exprsSize *= 2;
        parser->exprs = reallocarray(parser->exprs, parser->exprsSize, sizeof(Expr *));
    }

    parser->exprs[parser->current++] = expr;
}

Expr *parseExpr(Parser *parser) {
    consumeWhitespace(parser);

    char ch = parser->source[parser->current];

    union {
        ListExpr list;
        LiteralExpr literal;
        IdentifierExpr identifier;
    } exp;

    Expr *expr = NULL;
    switch (ch) {
        case '(':
            exp.list = parseListExpr(parser);
            expr = (Expr *)&exp.list;
            break;
        case '"':
            exp.literal = parseString(parser);
            expr = (Expr *)&exp.literal;
        default:
            break;
    }

    return expr;
}

Expr **parse(const char *source, size_t *tokensSize) {
    size_t currentSize = 10;
    Expr **exprs = calloc(currentSize, sizeof(Expr *));

    Parser parser = {
        .source = source,
        .sourceLength = strlen(source),
        .current = 0,
        .line = 1,
        .exprs = exprs,
        .exprsSize = currentSize,
        .exprsCount = 0
    };

    while (parser.current < parser.sourceLength) {
        Expr *expr = parseExpr(&parser);

        // An error occured
        if (!expr) {
            return NULL;
        }

        addExpr(&parser, expr);
    }

    *tokensSize = parser.exprsCount;
    return exprs;
}
