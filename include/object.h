#pragma once

#include "expr.h"
#include <stddef.h>
#include <stdint.h>
#include <stack_frame.h>

enum {
    NUMBER_ID,
    STRING_ID,
    FUNCTION_ID,
    CFUNCTION_ID,
    ERROR_ID,
};

typedef struct {
    uint32_t objectId;
    uint32_t refCount;
    void *value;
} Object;

void release(Object *obj);

typedef struct {
    double num;
} NumberObject;

typedef struct {
    char *str;
    size_t len;
} StringObject;

typedef struct {
    Expr **exprs;
    size_t exprsCount;
    size_t exprsSize;
    size_t argCount;
} FunctionObject;

typedef Object *(*CFunc)(StackFrame *, Expr **, size_t);

typedef struct {
    size_t argCount;
    CFunc fn;
} CFunctionObject;

typedef struct {
    const char *msg;
} ErrorObject;

Object *equals(StackFrame *frame, Expr **exprs, size_t size);
Object *add(StackFrame *frame, Expr **exprs, size_t size);
Object *subtract(StackFrame *frame, Expr **exprs, size_t size);
Object *divide(StackFrame *frame, Expr **exprs, size_t size);
Object *multiply(StackFrame *frame, Expr **exprs, size_t size);
Object *equality(StackFrame *frame, Expr **exprs, size_t size);
Object *print(StackFrame *frame, Expr **exprs, size_t size);

#define PI 3.14159
