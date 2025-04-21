#include "expr.h"
#include "hash_table.h"
#include "object.h"
#include <interpreter.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void setVariable(StackFrame *frame, char *key, Object *object) {
    object->refCount++;
    hashTableSet(frame->table, key, object);
}

Object *createCFunc(CFunc func, size_t argCount) {
    CFunctionObject *cFuncObject = malloc(sizeof(CFunctionObject));
    cFuncObject->argCount = argCount;
    cFuncObject->fn = func;

    Object *obj = malloc(sizeof(Object));
    obj->objectId = CFUNCTION_ID;
    obj->value = cFuncObject;
    obj->refCount = 0;
    return obj;
}

Object *createNumberObject(double number) {
    NumberObject *numberObject = malloc(sizeof(NumberObject));
    numberObject->num = number;

    Object *obj = malloc(sizeof(Object));
    obj->objectId = NUMBER_ID;
    obj->value = numberObject;
    obj->refCount = 0;
    return obj;
}

Object *createStringObject(char *str) {
    size_t strLen = strlen(str);
    char *newStr = calloc(strLen, sizeof(char));
    strncpy(newStr, str, strLen);

    StringObject *stringObject = malloc(sizeof(StringObject));
    stringObject->str = newStr;
    stringObject->len = strLen;

    Object *obj = malloc(sizeof(Object));
    obj->objectId = STRING_ID;
    obj->value = stringObject;
    obj->refCount = 0;
    return obj;
}

Object *createErrorEntry(const char *msg) {
    ErrorObject *errObject = malloc(sizeof(ErrorObject));
    errObject->msg = msg;

    Object *obj = malloc(sizeof(Object));
    obj->objectId = ERROR_ID;
    obj->value = errObject;
    obj->refCount = 0;
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

void closeStackFrame(StackFrame *frame) {
    for (int i = 0; i < frame->table->size; i++) {
        Entry entry = frame->table->entries[i];
        
        if (entry.str == NULL) {
            continue;
        }

        Object *obj = (Object *)entry.value; 
        obj->refCount--;
        
        if (obj->refCount <= 0) {
            release(obj);
        }
    }
    freeHashTable(frame->table);
    free(frame);
}

void run(Expr **exprs, size_t exprsCount) {
    StackFrame *globalFrame = malloc(sizeof(StackFrame));
    setGlobalFrame(globalFrame);

    for (int i = 0; i < exprsCount; i++) {
        Expr *expr = exprs[i];

        if (expr->type != LIST) {
            continue;
        }

        eval(expr, globalFrame);
    }

    closeStackFrame(globalFrame);
}

Object *eval(Expr *expr, StackFrame *stackFrame) {
    Object *result;

    Entry *identifierEntry;
    switch (expr->type) {
        case LITERAL:
            switch (expr->literal.type) {
                case STRING:
                    result = createStringObject(expr->literal.string);
                    break;
                case NUMBER:
                    result = createNumberObject(expr->literal.number);
                    break;
                case BOOLEAN:
                    break;
            }
            break;
        case IDENTIFIER:
            identifierEntry = hashTableGet(stackFrame->table, expr->identifier.name);

            if (!identifierEntry->str) {
                result = createErrorEntry("Did not expect identifier.");
                break;
            }

            result = identifierEntry->value;
            break;
        case LIST:
            if (expr->list.exprsCount == 1) {
                result = eval(expr->list.exprs[0], stackFrame);
                break;
            }

            Expr *firstExpr = expr->list.exprs[0];
            if (firstExpr->type != IDENTIFIER) {
                result = createErrorEntry("Expected identifier in list");
            }

            Object *firstObj = eval(firstExpr, stackFrame);
            
            if (firstObj->objectId != FUNCTION_ID && firstObj->objectId != CFUNCTION_ID) {
                result = createErrorEntry("Expected function in list");
            }

            break;
    }
    return result;
}
