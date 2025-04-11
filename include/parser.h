#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	LITERAL,
	GROUPING,
	LIST
} ExprType;

typedef struct {
	ExprType type;
} Expr;

typedef struct {
	Expr base;
	union {
		char *string;
		bool boolean;
		double number;
	} data;
} LiteralExpr;

typedef struct {
	Expr base;
	Expr *expr;
} GroupingExpr;

typedef struct {
	Expr base;
	Expr *left;
	Expr **right;
	size_t rightSize;
} ListExpr;
