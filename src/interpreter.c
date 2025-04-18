#include "expr.h"
#include "hash_table.h"
#include "object.h"
#include <interpreter.h>
#include <stdlib.h>

void setVariable(StackFrame *frame, char *key, Object *object) {
    hashTableSet(frame->table, key, object);
}

Object *createCFunc(CFunc func, size_t argCount) {
    CFunctionObject *cFuncObject = malloc(sizeof(CFunctionObject));
    cFuncObject->argCount = argCount;
    cFuncObject->fn = func;

    Object *obj = malloc(sizeof(Object));
    obj->objectId = CFUNCTION_ID;
    obj->value = cFuncObject;
    return obj;
}

Object *createNumberObject(double number) {
    NumberObject *numberObject = malloc(sizeof(NumberObject));
    numberObject->num = number;

    Object *obj = malloc(sizeof(Object));
    obj->objectId = NUMBER_ID;
    obj->value = numberObject;
    return obj;
}

void setGlobalFrame(StackFrame *frame) {
    setVariable(frame, "=", createCFunc(equals, 2));
    setVariable(frame, "+", createCFunc(add, 2));
    setVariable(frame, "-", createCFunc(subtract, 2));
    setVariable(frame, "*", createCFunc(multiply, 2));
    setVariable(frame, "/", createCFunc(divide, 2));
    setVariable(frame, "==", createCFunc(equals, 2));
    setVariable(frame, "print", createCFunc(print, 1));
    setVariable(frame, "pi", createNumberObject(PI));
}

void run(Expr **exprs) {
    Interpreter *interpreter = malloc(sizeof(Interpreter));
    interpreter->exprs = exprs;
    interpreter->globalFrame = malloc(sizeof(StackFrame));
    setGlobalFrame(interpreter->globalFrame);
}

void eval(Expr *expr) {
}
