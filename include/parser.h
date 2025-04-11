#pragma once

#include <stddef.h>
#include <expr.h>

typedef struct {
	const char *source;
	size_t sourceLength;
	size_t current;
	size_t line;
	Expr **exprs;
	size_t exprsSize;
	size_t exprsCount;
} Parser;

Expr **parse(const char *source, size_t *exprsSize);
