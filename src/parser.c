#include "expr.h"
#include <parser.h>
#include <stddef.h>
#include <stdlib.h>

Expr **parse(const char *source, size_t *tokensSize) {
	size_t currentSize = 10;
	*tokensSize = 0;
	Expr **exprs = calloc(currentSize, sizeof(Expr*));

	// Example
	// size_t current = 0;
	// size_t exprSize;
	// Expr *expr = parseExpr(source + current, &exprSize);
	// current += exprSize;

	return exprs;
}
