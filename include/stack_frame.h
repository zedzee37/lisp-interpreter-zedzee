#pragma once

#include <hash_table.h>

typedef struct StackFrame {
    struct StackFrame *prevFrame;
    HashTable *table;
} StackFrame;

void setGlobalFrame(StackFrame *frame);
StackFrame *initStackFrame();
void closeStackFrame(StackFrame *frame);
