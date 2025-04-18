#include "hash_table.h"
#include <interpreter.h>
#include <stdlib.h>

void setVariable(StackFrame *frame, char *key, Object *object) {
    hashTableSet(frame->table, key, object);
}

void setGlobalFrame(StackFrame *frame) {
}

void run(Expr **exprs) {
    Interpreter *interpreter = malloc(sizeof(Interpreter));
    interpreter->exprs = exprs;
    interpreter->globalFrame = malloc(sizeof(StackFrame));
    setGlobalFrame(interpreter->globalFrame);
}

void eval(Expr *expr) {
}
