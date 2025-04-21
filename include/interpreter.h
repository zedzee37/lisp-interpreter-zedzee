#pragma once

#include "object.h"
#include <stack_frame.h>
#include <expr.h>

typedef enum {
    INTERPRETER_NONE,
    INTERPRETER_ERR,
    INTERPRETER_IDENTIFIER_NOT_EXPECTED,
} InterpreterErrorType;

typedef struct {
    InterpreterErrorType errorType;
    Expr *expr;
    const char *msg; // Tmp
} InterpreterError;

InterpreterError interpret(Expr **exprs, size_t exprsCount);
Object *eval(Expr *expr, StackFrame *stackFrame);
void setVariable(StackFrame *frame, char *key, Object *object);
