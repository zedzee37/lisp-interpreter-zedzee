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

typedef struct Expr Expr;

typedef struct {
	union {
		char *string;
		bool boolean;
		double number;
	};
	LiteralType type;
} LiteralExpr;

typedef struct {
	Expr **exprs;
	size_t exprsSize;
    size_t exprsCount;
} ListExpr;

typedef struct {
	char *name;
} IdentifierExpr;

typedef struct Expr {
	ExprType type;
    union {
        LiteralExpr literal;
        ListExpr list;
        IdentifierExpr identifier;
    };
} Expr;

void freeExpr(Expr *expr);
