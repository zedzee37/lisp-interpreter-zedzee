#pragma once

#include <expr.h>

typedef struct {
    Expr **exprs;
} Interpreter;

void eval(Expr *expr);
