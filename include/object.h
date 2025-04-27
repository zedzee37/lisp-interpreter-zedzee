#pragma once

#include "expr.h"
#include <stddef.h>
#include <stdint.h>
#include <stack_frame.h>

#define PI 3.14159

enum {
    NUMBER_ID,
    STRING_ID,
    ARRAY_ID,
    FUNCTION_ID,
    CFUNCTION_ID,
    ERROR_ID,
    COMPOSITE_ID,
};

typedef struct {
    uint32_t objectType;
    uint32_t refCount;
    void *value;
} Object;

void reference(Object *obj);
void dereference(Object *obj);
void release(Object *obj);

typedef struct {
    double num;
} NumberObject;

typedef struct {
    char *str;
    size_t len;
} StringObject;

typedef struct {
    Object **elements;
    size_t size;
    size_t count;
} ArrayObject;

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

typedef struct {
    Object *base;
    StackFrame *properties;
} CompositeObject;

Object *equals(StackFrame *frame, Expr **exprs, size_t size);
Object *add(StackFrame *frame, Expr **exprs, size_t size);
Object *subtract(StackFrame *frame, Expr **exprs, size_t size);
Object *divide(StackFrame *frame, Expr **exprs, size_t size);
Object *multiply(StackFrame *frame, Expr **exprs, size_t size);
Object *equality(StackFrame *frame, Expr **exprs, size_t size);
Object *print(StackFrame *frame, Expr **exprs, size_t size);
Object *toStr(StackFrame *frame, Expr **exprs, size_t size);
Object *set(StackFrame *frame, Expr **exprs, size_t size);

