#pragma once

#include "expr.h"
#include <stddef.h>
#include <stdint.h>

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

typedef Object *(*CFunc)(Object *, size_t);

typedef struct {
    size_t argCount;
    CFunc fn;
} CFunctionObject;

typedef struct {
    const char *msg;
} ErrorObject;

Object *equals(Object *objects, size_t size);
Object *add(Object *objects, size_t size);
Object *subtract(Object *objects, size_t size);
Object *divide(Object *objects, size_t size);
Object *multiply(Object *objects, size_t size);
Object *equality(Object *objects, size_t size);
Object *print(Object *objects, size_t size);

#define PI 3.14159
