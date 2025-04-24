#include "expr.h"
#include "hash_table.h"
#include "object.h"
#include "stack_frame.h"
#include <interpreter.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StackFrame *initStackFrame() {
    StackFrame *frame = malloc(sizeof(StackFrame));
    frame->table = initHashTable();
    return frame;
}

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

Object *createErrorObject(const char *msg) {
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

InterpreterError interpret(Object **output, Expr **exprs, size_t exprsCount) {
    InterpreterError maybeErr;
    maybeErr.errorType = INTERPRETER_NONE;

    StackFrame *globalFrame = initStackFrame();
    setGlobalFrame(globalFrame);

    for (int i = 0; i < exprsCount; i++) {
        Expr *expr = exprs[i];

        Object *result = eval(expr, globalFrame);

        if (result->objectId == ERROR_ID) {
            maybeErr.errorType = INTERPRETER_ERR; // Tmp
            maybeErr.msg = ((ErrorObject *)result->value)->msg;
            break;
        }

        if (i == exprsCount - 1) {
            *output = result;
            break;
        }

        release(result);
    }

    closeStackFrame(globalFrame);
    return maybeErr;
}

Object *findVariable (StackFrame *frame, const char *key) {
    Object *result = hashTableGet(frame->table, key);
    
    if (!result) {
        if (!frame->prevFrame) {
            return NULL;
        }

        return findVariable(frame->prevFrame, key);
    }

    return result;
}

Object *eval(Expr *expr, StackFrame *stackFrame) {
    Object *result;

    Object *identifierObj;
    Expr *firstExpr;
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
            // identifierObj = hashTableGet(stackFrame->table, expr->identifier.name);
            identifierObj = findVariable(stackFrame, expr->identifier.name);

            if (!identifierObj) {
                result = createErrorObject("Did not expect identifier.");
                break;
            }

            result = identifierObj;
            break;
        case LIST:
            firstExpr = expr->list.exprs[0];
            if (firstExpr->type != IDENTIFIER) {
                result = createErrorObject("Expected identifier in list");
                break;
            }

            Object *firstObj = eval(firstExpr, stackFrame);
            if (firstObj->objectId != FUNCTION_ID && firstObj->objectId != CFUNCTION_ID) {
                result = createErrorObject("Expected function in list");
                break;
            }

            StackFrame *newFrame = initStackFrame();
            newFrame->prevFrame = stackFrame;

            if (firstObj->objectId == CFUNCTION_ID) {
                CFunctionObject *cFuncObject = firstObj->value;
                result = cFuncObject->fn(newFrame, expr->list.exprs + 1, expr->list.exprsCount - 1);
            }

            closeStackFrame(newFrame);

            break;
    }
    return result;
}

Object *equals(StackFrame *frame, Expr **exprs, size_t size) {
    return createNumberObject(0);
}

Object *add(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        printf("%lu\n", size);
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    Object *n2 = eval(exprs[1], frame);

    if (n1->objectId != NUMBER_ID || n2->objectId != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num + nTwo->num;
    release(n1);
    release(n2);

    return createNumberObject(result);
}

Object *subtract(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    Object *n2 = eval(exprs[1], frame);

    if (n1->objectId != NUMBER_ID || n2->objectId != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num - nTwo->num;
    release(n1);
    release(n2);

    return createNumberObject(result);
}

Object *divide(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    Object *n2 = eval(exprs[1], frame);

    if (n1->objectId != NUMBER_ID || n2->objectId != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num / nTwo->num;
    release(n1);
    release(n2);

    return createNumberObject(result);
}

Object *multiply(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    Object *n2 = eval(exprs[1], frame);

    if (n1->objectId != NUMBER_ID || n2->objectId != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num * nTwo->num;
    release(n1);
    release(n2);

    return createNumberObject(result);
}

Object *equality(StackFrame *frame, Expr **exprs, size_t size) {
    return createNumberObject(0);
}

Object *print(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 1) {
        return createErrorObject("wanted 1 param\n"); 
    }
    Object *toPrint = eval(exprs[0], frame);

    if (toPrint->objectId != STRING_ID) {
        return createErrorObject("expected string\n"); 
    }

    StringObject *strObj = (StringObject *)toPrint->value;
    printf("%s\n", strObj->str);

    return createNumberObject(0);
}
