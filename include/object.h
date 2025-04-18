#pragma once

#include "expr.h"
#include <stddef.h>
#include <stdint.h>

enum {
    NUMBER_ID,
    STRING_ID,
    FUNCTION_ID,
    CFUNCTION_ID,
};

typedef struct {
    uint32_t objectId;
    void *value;
} Object;

typedef struct {
    double num;
} NumberObject;

typedef struct {
    char *str;
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

