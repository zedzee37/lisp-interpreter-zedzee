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
    Object *old = hashTableGet(frame->table, key);
    
    if (old) {
        dereference(old);
    }

    hashTableSet(frame->table, key, object);
    reference(object);
}

Object *findVariable(StackFrame *frame, const char *key) {
    Object *result = hashTableGet(frame->table, key);
    
    if (!result) {
        if (!frame->prevFrame) {
            return NULL;
        }

        return findVariable(frame->prevFrame, key);
    }

    return result;
}


Object *createCFunc(CFunc func, size_t argCount) {
    CFunctionObject *cFuncObject = malloc(sizeof(CFunctionObject));
    cFuncObject->argCount = argCount;
    cFuncObject->fn = func;

    Object *obj = malloc(sizeof(Object));
    obj->objectType = CFUNCTION_ID;
    obj->value = cFuncObject;
    obj->refCount = 0;
    return obj;
}

Object *createNumberObject(double number) {
    NumberObject *numberObject = malloc(sizeof(NumberObject));
    numberObject->num = number;

    Object *obj = malloc(sizeof(Object));
    obj->objectType = NUMBER_ID;
    obj->value = numberObject;
    obj->refCount = 0;
    return obj;
}

Object *createStringObject(char *str) {
    char *newStr = strdup(str);

    StringObject *stringObject = malloc(sizeof(StringObject));
    stringObject->str = newStr;
    stringObject->len = strlen(str);

    Object *obj = malloc(sizeof(Object));
    obj->objectType = STRING_ID;
    obj->value = stringObject;
    obj->refCount = 0;
    return obj;
}

Object *createArrayObject(Object **elements, size_t size, size_t count) {
    ArrayObject *arrayObject = malloc(sizeof(ArrayObject));
    arrayObject->elements = elements;
    arrayObject->size = size;
    arrayObject->count = count;
    
    Object *obj = malloc(sizeof(Object));
    obj->objectType = ARRAY_ID;
    obj->value = arrayObject;
    obj->refCount = 0;
    return obj;
}

Object *createErrorObject(const char *msg) {
    ErrorObject *errObject = malloc(sizeof(ErrorObject));
    errObject->msg = msg;

    Object *obj = malloc(sizeof(Object));
    obj->objectType = ERROR_ID;
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
    setVariable(frame, "while", createCFunc(whileLoop, 2));
    setVariable(frame, "pi", createNumberObject(PI));
    setVariable(frame, "str", createCFunc(toStr, 1));
}

void closeStackFrame(StackFrame *frame) {
    for (int i = 0; i < frame->table->size; i++) {
        Entry entry = frame->table->entries[i];
        
        if (entry.str == NULL) {
            continue;
        }

        Object *obj = (Object *)entry.value; 
        dereference(obj);
    }
    freeHashTable(frame->table);
    free(frame);
}

Interpreter *initInterpreter() {
    Interpreter *interpreter = malloc(sizeof(Interpreter));
    
    interpreter->globalFrame = initStackFrame();
    setGlobalFrame(interpreter->globalFrame);

    return interpreter;
}

InterpreterError interpret(Interpreter *interpreter, Object **output, Expr **exprs, size_t exprsCount) {
    InterpreterError maybeErr;
    maybeErr.errorType = INTERPRETER_NONE;

    for (int i = 0; i < exprsCount; i++) {
        Expr *expr = exprs[i];

        Object *result = eval(expr, interpreter->globalFrame);

        if (result->objectType == ERROR_ID) {
            maybeErr.errorType = INTERPRETER_ERR; // Tmp
            maybeErr.msg = ((ErrorObject *)result->value)->msg;
            *output = result;
            break;
        }

        if (i == exprsCount - 1) {
            *output = result;
            break;
        }

        release(result);
    }

    return maybeErr;
}

void freeInterpreter(Interpreter *interpreter) {
    closeStackFrame(interpreter->globalFrame);
    free(interpreter);
}


Object *eval(Expr *expr, StackFrame *stackFrame) {
    Object *result;

    Object *identifierObj;
    Expr *firstExpr;

    Object **arrElements;
    switch (expr->type) {
        case LITERAL:
            switch (expr->literal.type) {
                case STRING:
                    result = createStringObject(expr->literal.string);
                    break;
                case NUMBER:
                    result = createNumberObject(expr->literal.number);
                    break;
                case ARRAY:
                    arrElements = calloc(expr->literal.array.size, sizeof(Object *));
                    for (int i = 0; i < expr->literal.array.count; i++) {
                        Object *element = eval(expr->literal.array.elements[i], stackFrame);
                        reference(element);
                        arrElements[i] = element;
                    }
                    result = createArrayObject(arrElements, expr->literal.array.size, expr->literal.array.count);
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
            if (firstObj->objectType != FUNCTION_ID && firstObj->objectType != CFUNCTION_ID) {
                result = createErrorObject("Expected function in list");
                break;
            }

            StackFrame *newFrame = initStackFrame();
            newFrame->prevFrame = stackFrame;

            if (firstObj->objectType == CFUNCTION_ID) {
                CFunctionObject *cFuncObject = firstObj->value;
                result = cFuncObject->fn(newFrame, expr->list.exprs + 1, expr->list.exprsCount - 1);
            } else if (firstObj->objectType == FUNCTION_ID) {
                 
            } else {
                result = createErrorObject("Expected function\n");  
            }

            closeStackFrame(newFrame);
            break;
    }
    return result;
}

Object *equals(StackFrame *frame, Expr **exprs, size_t size) {
    if (!frame->prevFrame) {
        return createErrorObject("Need prev stack frame\n");
    } else if (size != 2) {
        return createErrorObject("need 2 args\n");
    }

    Expr *identifierExpr = exprs[0];
    Object *value = eval(exprs[1], frame);

    if (identifierExpr->type != IDENTIFIER) {
        return createErrorObject("expected identifier\n");
    }
    
    setVariable(frame->prevFrame, identifierExpr->identifier.name, value);

    return createNumberObject(0);
}

Object *add(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    reference(n1);
    Object *n2 = eval(exprs[1], frame);
    reference(n2);

    if (n1->objectType != NUMBER_ID || n2->objectType != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num + nTwo->num;
    dereference(n1);
    dereference(n2);

    return createNumberObject(result);
}

Object *subtract(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    reference(n1);
    Object *n2 = eval(exprs[1], frame);
    reference(n2);

    if (n1->objectType != NUMBER_ID || n2->objectType != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num - nTwo->num;
    dereference(n1);
    dereference(n2);

    return createNumberObject(result);
}

Object *divide(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    reference(n1);
    Object *n2 = eval(exprs[1], frame);
    reference(n2);

    if (n1->objectType != NUMBER_ID || n2->objectType != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num / nTwo->num;
    dereference(n1);
    dereference(n2);

    return createNumberObject(result);
}

Object *multiply(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *n1 = eval(exprs[0], frame);
    reference(n1);
    Object *n2 = eval(exprs[1], frame);
    reference(n2);

    if (n1->objectType != NUMBER_ID || n2->objectType != NUMBER_ID) {
        return createErrorObject("wanted numbers\n"); 
    }
    
    NumberObject *nOne = n1->value;
    NumberObject *nTwo = n2->value;

    double result = nOne->num * nTwo->num;
    dereference(n1);
    dereference(n2);

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

    if (toPrint->objectType != STRING_ID) {
        return createErrorObject("expected string\n"); 
    }

    StringObject *strObj = (StringObject *)toPrint->value;
    printf("%s\n", strObj->str);

    release(toPrint);
    return createNumberObject(0);
}

Object *toStr(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 1) {
        return createErrorObject("wanted 1 param\n"); 
    }

    Object *toConv = eval(exprs[0], frame);
    Object *result;
    char buffer[100];

    switch (toConv->objectType) {
        case NUMBER_ID:
            snprintf(buffer, 100, "%f", ((NumberObject *)toConv->value)->num);
            result = createStringObject(buffer);
            break;
        default:
            result = createStringObject("obj\n");
            break;
    }
    
    release(toConv);
    return result;
}

Object *whileLoop(StackFrame *frame, Expr **exprs, size_t size) {
    if (size != 2) {
        return createErrorObject("wanted 2 params\n");
    }

    Expr *condition = exprs[0];
    Expr *loopBody = exprs[1];

    if (loopBody->type != LIST) {
        return createErrorObject("Expected list expression!");
    }
    
    Object *conditionResult = eval(condition, frame);
    reference(conditionResult);
    while (conditionResult->objectType == NUMBER_ID && ((NumberObject *)conditionResult->value)->num == 1.0) {
        for (int i = 0; i < loopBody->list.exprsCount; i++) {
            Object *result = eval(loopBody->list.exprs[i], frame);
            release(result);
        }
        
        dereference(conditionResult);
        conditionResult = eval(condition, frame);
        reference(conditionResult);
    }

    return createNumberObject(0.0);
}
