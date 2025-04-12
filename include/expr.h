#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	LITERAL,
	LIST,
	IDENTIFIER
} ExprType;

typedef enum {
	STRING,
	BOOLEAN,
	NUMBER,
} LiteralType;

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
	LiteralType type;
} LiteralExpr;

typedef struct {
	Expr base;
	Expr **exprs;
	size_t exprsSize;
    size_t exprsCount;
} ListExpr;

typedef struct {
	Expr base;
	char *name;
} IdentifierExpr;

void freeExpr(Expr *expr);
