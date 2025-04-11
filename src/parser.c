#include "expr.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <parser.h>

// parse parses the entire string into expressions, whilst parseExpr parses a single expression

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
    
    while (parser->source[parser->current] != ')') {
        Expr *expr = parseExpr(parser);
        
    }

    return expr;
}

Expr *parseString(Parser *parser) {

}

Expr *parseNumber(Parser *parser) {

}

Expr *parseIdentifier(Parser *parser) {

}

Expr *parseExpr(Parser *parser) {
	consumeWhitespace(parser);

	char ch = parser->source[parser->current];

	switch (ch) {
		case '(':
		return parseListExpr(parser);
		break;
		default:
		break;
	}

	// // All expressions should start with a left parenthesis
	// if (ch != '(') {
	// 	// TODO: error here
	// 	return NULL;
	// }

	// consumeWhitespace(parser);

	// // Empty expression
	// if (parser->source[parser->current] == ')') {
	// 	return NULL;
	// }

	// while (parser->source[parser->current] != ')') {
	// 	Expr *expr = parseExpr(parser);

	// 	consumeWhitespace(parser);
	// }
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

	*tokensSize = parser.exprsCount;
	return exprs;
}
