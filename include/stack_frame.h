#pragma once

#include <hash_table.h>

typedef struct StackFrame {
    struct StackFrame *nextFrame;
    HashTable *table;
} StackFrame;

void setGlobalFrame(StackFrame *frame);
void closeStackFrame(StackFrame *frame);
