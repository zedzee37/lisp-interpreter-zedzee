#include <expr.h>
#include <ctype.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h> 
#include <stdio.h>
#include <parser.h>

bool isSpecial(char ch) {
    return 
    ch == '+' || ch == '-' || ch == '*' ||
    ch == '=' || ch == '/' || ch == '%';
}

// Short for isalnum or is special
bool isRegular(char ch) {
    return isalnum(ch) || isSpecial(ch);
}

// parse parses the entire string into expressions, whilst parseExpr parses a single expression
void freeExpr(Expr *expr) {
    switch (expr->type) {
        case LITERAL:
            switch (expr->literal.type) {
                case STRING:
                    free(expr->literal.string);
                    break;
                case ARRAY:
                    for (int i = 0; i < expr->literal.array.count; i++) {
                        freeExpr(expr->literal.array.elements[i]);
                    }
                    free(expr->literal.array.elements);
                    break;
                default:
                    break;
            }
            break;
        case LIST:
            // Free all exprs
            for (int i = 0; i < expr->list.exprsCount; i++) {
                freeExpr(expr->list.exprs[i]);
            }
            free(expr->list.exprs);
            break;
        case IDENTIFIER:
            free(expr->identifier.name);
            break;
    }
    free(expr);
}

void consumeWhitespace(Parser *parser) {
    while (parser->current < parser->sourceLength) {
        char ch = parser->source[parser->current];

        // Ignore whitespace, besdies \n, in which case increment the line.
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

// Testing (5 + +)
ParserError parseListExpr(Parser *parser, Expr *expr) {
    ParserError maybeErr;
    parser->current++; // consume the left parenthesis
    consumeWhitespace(parser);

    if (parser->source[parser->current] == ')') {
        maybeErr.errorType = PARSER_UNEXPECTED_CHAR;
        maybeErr.line = parser->line;
        maybeErr.ch = ')';
        maybeErr.where = parser->current;
        return maybeErr;
    }

    ListExpr listExpr;

    listExpr.exprsSize = 2;
    listExpr.exprsCount = 0;

    listExpr.exprs = calloc(listExpr.exprsSize, sizeof(Expr *));

    expr->type = LIST;

    while (parser->current < parser->sourceLength) {
        consumeWhitespace(parser);

        Expr *e;
        ParserError err = parseExpr(parser, &e);

        if (err.errorType != PARSER_NONE) {
            expr->list = listExpr;
            return err;
        }

        if (listExpr.exprsCount >= listExpr.exprsSize) {
            listExpr.exprsSize += 2;
            listExpr.exprs = realloc(listExpr.exprs, listExpr.exprsSize * sizeof(Expr *));
        }

        listExpr.exprs[listExpr.exprsCount++] = e;

        if (listExpr.exprsCount >= 4) {
            printf("%lu\n", expr->list.exprsCount);
        }

        consumeWhitespace(parser);

        char ch = parser->source[parser->current]; 

        if (ch == ')') {
            parser->current++;

            expr->list = listExpr;

            maybeErr.errorType = PARSER_NONE;
            return maybeErr;
        }
    }

    maybeErr.errorType = PARSER_MISSING_CHAR;
    maybeErr.ch = ')';
    maybeErr.line = parser->line;
    maybeErr.where = parser->current;

    expr->list = listExpr;

    return maybeErr;
}

ParserError parseString(Parser *parser, Expr *expr) {
    ParserError maybeError;

    parser->current++; // Consume the opening "

    size_t strSize = 10;
    size_t charCount = 0;
    char *str = calloc(strSize, sizeof(char));

    while (true) {
        if (parser->current >= parser->sourceLength) {
            free(str);
            maybeError.errorType = PARSER_MISSING_CHAR;
            maybeError.line = parser->line;
            maybeError.ch = '"';
            maybeError.where = parser->current;
            return maybeError;
        }

        char ch = parser->source[parser->current];
        if (ch == '"') break;

        // Store character
        if (charCount >= strSize) {
            strSize *= 2;
            str = realloc(str, strSize * sizeof(char));
        }

        str[charCount++] = ch;
        parser->current++;
    }

    parser->current++; // Consume the closing "

    // Null-terminate the string
    if (charCount >= strSize) {
        str = realloc(str, (strSize + 1) * sizeof(char));
    }
    str[charCount] = '\0';

    LiteralExpr stringLiteral;
    stringLiteral.type = STRING;
    stringLiteral.string = str;

    expr->type = LITERAL;
    expr->literal = stringLiteral;

    maybeError.errorType = PARSER_NONE;
    return maybeError;
}

ParserError parseNumber(Parser *parser, Expr *expr) {
    ParserError maybeError;

    size_t strSize = 5;
    size_t digitCount = 0;
    char *number = calloc(strSize, sizeof(char));
    bool foundDecimal = false;

    while (
        parser->current < parser->sourceLength
    ) {
        char ch = parser->source[parser->current];

        if (!isdigit(ch)) {
            if (ch == '.') {
                if (foundDecimal) {
                    free(number);
                    maybeError.errorType = PARSER_UNEXPECTED_CHAR;
                    maybeError.where = parser->current;
                    maybeError.line = parser->line;
                    maybeError.ch = '.';
                    return maybeError;
                }

                foundDecimal = true;
            } else {
                break;
            }
        }

        parser->current++;

        if (digitCount >= strSize) {
            strSize *= 2;
            number = realloc(number, strSize * sizeof(char));
        }

        number[digitCount++] = ch;
    }

    // Make room for the null terminator.
    if (digitCount >= strSize) {
        strSize += 1;
        number = realloc(number, strSize * sizeof(char));
    }

    number[digitCount] = '\0';

    LiteralExpr literal;
    literal.type = NUMBER;
    literal.number = atof(number);
    free(number);

    expr->type = LITERAL;
    expr->literal = literal;

    maybeError.errorType = PARSER_NONE;
    return maybeError;
}

ParserError parseIdentifier(Parser *parser, Expr *expr) {
    ParserError maybeError;

    size_t strSize = 10;
    size_t charCount = 0;
    char *str = calloc(strSize, sizeof(char));

    while (
        parser->current < parser->sourceLength &&
        isRegular(parser->source[parser->current])
    ) {
        char ch = parser->source[parser->current];

        if (charCount >= strSize) {
            strSize *= 2;
            str = realloc(str, strSize * sizeof(char));
        }

        str[charCount++] = ch;
        parser->current++;
    }

    // Make room for the null terminator.
    if (charCount >= strSize) {
        strSize += 1;
        str = realloc(str, strSize * sizeof(char));
    }

    str[charCount] = '\0';

    IdentifierExpr identifier;
    identifier.name = str;

    expr->type = IDENTIFIER;
    expr->identifier = identifier;

    maybeError.errorType = PARSER_NONE;
    return maybeError;
}

ParserError parseArray(Parser *parser, Expr *expr) {
    ParserError maybeError;
    maybeError.errorType = PARSER_NONE;

    parser->current++; // consume the [
    
    LiteralExpr literal;
    literal.type = ARRAY;
    literal.array.size = ARRAY_DEFAULT_SIZE;
    literal.array.count = 0;
    literal.array.elements = calloc(literal.array.size, sizeof(Expr *)); 

    while (parser->current < parser->sourceLength) {
        consumeWhitespace(parser);

        Expr *e;
        ParserError err = parseExpr(parser, &e);
        
        if (err.errorType != PARSER_NONE) {
            maybeError = err;
            goto exit;
        }

        if (literal.array.count >= literal.array.size) {
            literal.array.size *= 2;
            literal.array.elements = realloc(literal.array.elements, literal.array.size * sizeof(Expr *));
        }

        literal.array.elements[literal.array.count++] = e;
        
        consumeWhitespace(parser);

        if (parser->source[parser->current] == ']') {
            parser->current++;
            goto exit;
        }
    }

    maybeError.errorType = PARSER_MISSING_CHAR;
    maybeError.ch = ']';
    maybeError.line = parser->line;
    maybeError.where = parser->current;

exit:
    expr->type = LITERAL;
    expr->literal = literal;

    return maybeError;
}

ParserError parseExpr(Parser *parser, Expr **expr) {
    consumeWhitespace(parser);

    ParserError err;

    if (parser->current >= parser->sourceLength) {
        *expr = NULL;
        err.errorType = PARSER_NONE;
        return err;
    }

    Expr *e = malloc(sizeof(Expr));
    char ch = parser->source[parser->current];

    ParserError maybeErr;
    *expr = NULL;
    switch (ch) {
        case '(':
            maybeErr = parseListExpr(parser, e);
            break;
        case '"':
            maybeErr = parseString(parser, e);
            break;
        case '[':
            maybeErr = parseArray(parser, e);
            break;
        default:
            if (isdigit(ch)) {
                maybeErr = parseNumber(parser, e);
            } else if (isalpha(ch) || isSpecial(ch)) {
                maybeErr = parseIdentifier(parser, e);
            } else {
                maybeErr.errorType = PARSER_UNEXPECTED_CHAR;
                maybeErr.line = parser->line;
                maybeErr.where = parser->current;
                maybeErr.ch = ch;
            }

            break;
    }

    *expr = e;

    if (maybeErr.errorType != PARSER_NONE) {
        return maybeErr;
    }

    err.errorType = PARSER_NONE;
    return err;
}

void addExpr(Parser *parser, Expr *expr) {
    if (parser->exprsCount >= parser->exprsSize) {
        parser->exprsSize *= 2;
        parser->exprs = realloc(parser->exprs, parser->exprsSize * sizeof(Expr **));
    }

    parser->exprs[parser->exprsCount++] = expr;
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

    consumeWhitespace(&parser);
    while (parser.current < parser.sourceLength) {
        Expr *expr;
        ParserError maybeErr = parseExpr(&parser, &expr);

        if (expr) {
            addExpr(&parser, expr);
        }

        if (maybeErr.errorType != PARSER_NONE) {
            *resultExprs = parser.exprs;
            *tokensSize = parser.exprsCount;
            return maybeErr;
        }
    }

    *resultExprs = parser.exprs;
    *tokensSize = parser.exprsCount;
    err.errorType = PARSER_NONE;
    return err;
}
