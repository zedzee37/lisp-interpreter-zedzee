#pragma once

#include <hash_table.h>

typedef struct StackFrame {
    struct StackFrame *prevFrame;
    HashTable *table;
} StackFrame;

void setGlobalFrame(StackFrame *frame);
void closeStackFrame(StackFrame *frame);
