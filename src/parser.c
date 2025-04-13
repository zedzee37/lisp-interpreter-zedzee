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

ParserError parseListExpr(Parser *parser, ListExpr *expr) {
    ParserError maybeErr;
    parser->current++; // consume the left parenthesis
    consumeWhitespace(parser);

    if (parser->source[parser->current] == ')') {
        maybeErr.errorType = UNEXPECTED_CHAR;
        maybeErr.line = parser->line;
        maybeErr.ch = ')';
        maybeErr.where = parser->current;
        return maybeErr;
    }

    ListExpr listExpr;
    listExpr.base = (Expr){ .type = LIST };

    listExpr.exprsSize = 2;
    listExpr.exprsCount = 0;

    listExpr.exprs = calloc(listExpr.exprsSize, sizeof(Expr **));

    while (true) {
        Expr *e;
        ParserError err = parseExpr(parser, &e);

        if (err.errorType != NONE) {
            return err;
        }

        if (listExpr.exprsCount >= listExpr.exprsSize) {
            listExpr.exprsSize += 2; // less aggressive growth
            listExpr.exprs = realloc(listExpr.exprs, listExpr.exprsSize * sizeof(Expr *));
        }

        listExpr.exprs[listExpr.exprsCount++] = e;

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
        maybeErr.errorType = UNEXPECTED_CHAR;
        maybeErr.line = parser->line;
        maybeErr.ch = parser->source[parser->current];
        maybeErr.where = parser->current;
        return maybeErr;
    }

    maybeErr.errorType = NONE;
    return maybeErr;
}

ParserError parseString(Parser *parser, LiteralExpr *expr) {
    ParserError maybeError;

    parser->current++; // Consume the "
    
    size_t strSize = 10;
    size_t charCount = 0;
    char *str = calloc(strSize, sizeof(char));
    
    while (parser->source[parser->current] != '"') {
        char ch = parser->source[parser->current++];
        
        if (charCount >= strSize) {
            strSize *= 2;
            str = reallocarray(str, strSize, sizeof(char));
        }

        str[charCount++] = ch;

        parser->current++;

        if (parser->current >= parser->sourceLength) {
            free(str);
            maybeError.errorType = MISSING_CHAR;
            maybeError.line = parser->line;
            maybeError.ch = '"';
            maybeError.where = parser->current;
            return maybeError;
        }
    }

    // Make room for the null terminator.
    if (charCount >= strSize) {
        strSize += 1;
        str = reallocarray(str, strSize, sizeof(char));
    }

    str[charCount] = '\0'; 
    
    // Return the captured string'
    LiteralExpr stringLiteral;
    stringLiteral.type = STRING;
    stringLiteral.base = (Expr){ .type = LITERAL };
    stringLiteral.string = str;
    *expr = stringLiteral;

    maybeError.errorType = NONE;
    return maybeError;
}

ParserError parseNumber(Parser *parser, LiteralExpr *expr) {
    ParserError maybeError;

    maybeError.errorType = NONE;
    return maybeError;
}

ParserError parseIdentifier(Parser *parser, LiteralExpr *expr) {
    ParserError maybeError;

    maybeError.errorType = NONE;
    return maybeError;
}

void addExpr(Parser *parser, Expr *expr) {
    if (parser->exprsCount >= parser->exprsSize) {
        parser->exprsSize *= 2;
        parser->exprs = realloc(parser->exprs, parser->exprsSize * sizeof(Expr *));
    }

    parser->exprs[parser->current++] = expr;
}

ParserError parseExpr(Parser *parser, Expr **expr) {
    ParserError err;
    consumeWhitespace(parser);

    char ch = parser->source[parser->current];

    union {
        ListExpr list;
        LiteralExpr literal;
        IdentifierExpr identifier;
    } exp;

    ParserError maybeErr;
    *expr = NULL;
    switch (ch) {
        case '(':
            maybeErr = parseListExpr(parser, &exp.list);
            *expr = (Expr *)&exp.list;
            break;
        case '"':
            maybeErr = parseString(parser, &exp.literal);
            *expr = (Expr *)&exp.literal;
            break;
        default:
            maybeErr.errorType = UNEXPECTED_CHAR;
            maybeErr.line = parser->line;
            maybeErr.where = parser->current;
            maybeErr.ch = ch;
            break;
    }

    if (maybeErr.errorType != NONE) {
        return maybeErr;
    }

    err.errorType = NONE;
    return err;
}

ParserError parse(const char *source, size_t *tokensSize, Expr ***resultExprs) {
    ParserError err;
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
        Expr *expr;
        ParserError maybeErr = parseExpr(&parser, &expr);

        if (maybeErr.errorType != NONE) {
            return maybeErr;
        }

        addExpr(&parser, expr);
    }

    *resultExprs = parser.exprs;
    *tokensSize = parser.exprsCount;
    err.errorType = NONE;
    return err;
}
