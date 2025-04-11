#include "expr.h"
#include <parser.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

Expr *parseExpr(Parser *parser) {
	consumeWhitespace(parser);
	char ch = parser->source[parser->current];

	if (ch != '(') {
		// TODO: error here
		return NULL;
	}
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
