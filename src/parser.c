#include "expr.h"
#include <parser.h>
#include <stdlib.h>

Expr **parse(const char *source, size_t *tokensSize) {
	size_t currentSize = 10;
	*tokensSize = 0;
	Expr **exprs = calloc(currentSize, sizeof(Expr*));

	return exprs;
}
