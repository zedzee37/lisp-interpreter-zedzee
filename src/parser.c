#include "expr.h"
#include <parser.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Expr **parse(const char *source, size_t *tokensSize) {
	size_t currentSize = 10;
	*tokensSize = 0;
	Expr **exprs = calloc(currentSize, sizeof(Expr*));

	// Example
	// size_t current = 0;
	// size_t exprSize;
	// Expr *expr = parseExpr(source + current, &exprSize);
	// current += exprSize;

	size_t current = 0;
	size_t sourceLen = strlen(source);

	while (current < sourceLen) {
		char ch = source[current];

		switch (ch) {
			case '(':
			break;

			case ' ':
			break;
			case '\n':
			break;
			case '\t':
			break;
			case '\r':
			break;

			default:
			// An error occurred, all expressions must start with parenthesis
			// TODO: better error reporting
			printf("Error\n");
			return NULL;
			break;
		}

		current++;
	}

	return exprs;
}

Expr *parseExpr(const char *source, size_t *exprLen) {
	(*exprLen) = 1; // 1 to consume the parenthesis
}
