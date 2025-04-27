#pragma once

#include "object.h"
#include "stack_frame.h"

typedef struct {
    StackFrame *frame;
} FixedStackFrame;

FixedStackFrame *initFixedStackFrame(char **keys, size_t len);
void closeFixedStackFrame(FixedStackFrame *frame);
void setFixedStackFrame(FixedStackFrame *frame, char *key, Object *obj);
Object *getFixedStackFrame(FixedStackFrame *frame, char *key, Object *obj);

