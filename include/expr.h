#pragma once

#include <stdbool.h>
#include <stddef.h>

#define ARRAY_DEFAULT_SIZE 10

typedef enum {
	LITERAL,
	LIST,
	IDENTIFIER
} ExprType;

typedef enum {
	STRING,
	NUMBER,
    ARRAY,
} LiteralType;

typedef struct Expr Expr;

typedef struct {
	union {
		char *string;
		double number;
        struct {
            size_t size;
            size_t count;
            Expr **elements;
        } array;
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

void printExpr(Expr *expr);
void freeExpr(Expr *expr);
