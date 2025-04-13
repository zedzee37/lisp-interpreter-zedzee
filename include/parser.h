#pragma once

#include <stddef.h>
#include <expr.h>

typedef enum {
	NONE,
	UNEXPECTED_CHAR,
} ParserErrorType;

typedef struct {
	ParserErrorType errorType;
	size_t line;
	size_t where;
	char unexpected;
} ParserError;

typedef struct {
	const char *source;
	size_t sourceLength;
	size_t current;
	size_t line;
	Expr **exprs;
	size_t exprsSize;
	size_t exprsCount;
} Parser;

ParserError parse(const char *source, size_t *exprsSize, Expr ***returnExprs);
ParserError parseExpr(Parser *parser, Expr **expr);
