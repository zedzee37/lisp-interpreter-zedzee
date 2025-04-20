#pragma once

#include "hash_table.h"
#include "object.h"
#include <expr.h>

typedef enum {
    NONE,
    IDENTIFIER_NOT_EXPECTED,
} InterpreterErrorType;

typedef struct {
    InterpreterErrorType errorType;
    Expr *expr;
} InterpreterError;

typedef struct StackFrame {
    struct StackFrame *nextFrame;
    HashTable *table;
} StackFrame;

typedef struct {
    Expr **exprs;
    StackFrame *globalFrame;
} Interpreter;

void setVariable(StackFrame *frame, char *key, Object *object);
void setGlobalFrame(StackFrame *frame);
void closeStackFrame(StackFrame *frame);
void run(Expr **exprs);
void eval(Expr *expr);
