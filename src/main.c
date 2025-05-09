#include "expr.h"
#include "interpreter.h"
#include "object.h"
#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void printObj(Object *obj) {
    ArrayObject *arrayObject = obj->value;
    switch (obj->objectType) {
        case NUMBER_ID:
            printf("%f\n", ((NumberObject *)obj->value)->num);
            break;
        case STRING_ID:
            printf("%s\n", ((StringObject *)obj->value)->str);
            break;
        case ARRAY_ID:
            printf("Array:\n");

            for (int i = 0; i < arrayObject->count; i++) {
                printf("* ");
                printObj(arrayObject->elements[i]);
            }
        default:
            break;
    }
}

void printExpr(Expr *expr) {
    switch (expr->type) {
        case LITERAL:
            switch (expr->literal.type) {
                case STRING:
                    printf("Literal %s\n", expr->literal.string);
                    break;
                case NUMBER:
                    printf("Literal %f\n", expr->literal.number);
                    break;
                case ARRAY:
                    printf("Literal Array: \n");
                    for (int i = 0; i < expr->literal.array.count; i++) {
                        printf("* "); 
                        printExpr(expr->literal.array.elements[i]);
                    }
                    break;
            }
            break;
        case LIST:
            printf("List element count: %lu\n", expr->list.exprsCount);
            
            for (int i = 0; i < expr->list.exprsCount; i++) {
                printf("| ");
                printExpr(expr->list.exprs[i]); 
            }
            break;
        case IDENTIFIER:
            printf("Identifier '%s'\n", expr->identifier.name);
            break;
    }
}

char *readFile(char *fileName) {
    FILE *fp = fopen(fileName, "r");
    if (!fp) {
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *fileContents = calloc(size + 1, sizeof(char));
    if (!fileContents) {
        return NULL;
    }

    fileContents[size] = '\0';

    fread(fileContents, sizeof(char), size, fp);

    fclose(fp);
    return fileContents;
}

void reportParserError(ParserError err) {
    switch (err.errorType) {
        case PARSER_UNEXPECTED_CHAR:
            printf("Unexpected character at %lu:%lu, found %c.\n", err.line, err.where, err.ch);
            break;
        case PARSER_MISSING_CHAR:
            printf("Missing character at %lu%lu, expected %c.\n", err.line, err.where, err.ch);
            break;
        default:
            break;
    }
    return;
}

void run(Interpreter *interpreter, char *source) {
    size_t exprCount;
    Expr **exprs;
    ParserError err = parse(source, &exprCount, &exprs);

    if (err.errorType != PARSER_NONE) {
        reportParserError(err);
        goto free_exprs;
    }

    // for (int i = 0; i < exprCount; i++) {
    //     printExpr(exprs[i]);
    // }

    Object *output;
    InterpreterError interpreterErr = interpret(interpreter, &output, exprs, exprCount);
    printObj(output);
    release(output);

    if (interpreterErr.errorType != INTERPRETER_NONE) {
        printf("%s\n", interpreterErr.msg);
        return;
    }

free_exprs:

    for (int i = 0; i < exprCount; i++) {
        freeExpr(exprs[i]);
    }

    free(exprs);
    return;
}

void runFile(char *fileName) {
    char *fileContents = readFile(fileName);
    if (!fileContents) {
        return;
    }

    Interpreter *interpreter = initInterpreter();

    run(interpreter, fileContents);

    freeInterpreter(interpreter);
    free(fileContents);
}

void runPrompt() {
    char *input = NULL;
    size_t size = 0;
    Interpreter *interpreter = initInterpreter(); // to make the interpreter instance persistent

    while (true) {
        printf(" > ");
        size_t len = getline(&input, &size, stdin);
        if (len <= 1) {
            break;
        }
        run(interpreter, input);
    }

    freeInterpreter(interpreter);
    free(input);
}

int main(int argc, char **argv) {
    if (argc == 2) {
        runFile(argv[1]);
    } else if (argc == 1)  {
        runPrompt();
    } else {
        printf("Expects either 1 or 2 args.\n");
    }
    return 0;
}
