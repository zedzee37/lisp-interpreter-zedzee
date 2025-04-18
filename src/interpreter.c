#include <interpreter.h>
#include <stdlib.h>

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
