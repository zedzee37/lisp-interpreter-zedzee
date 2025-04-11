#pragma once

#include <stddef.h>
#include <expr.h>

Expr **parse(const char *source, size_t *tokensSize);
Expr *parseExpr(const char *source, size_t *exprSize);
