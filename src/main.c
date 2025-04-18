#include "expr.h"
#include "hash_table.h"
#include "parser.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
                case BOOLEAN:
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

void reportError(ParserError err) {
    switch (err.errorType) {
        case UNEXPECTED_CHAR:
            printf("Unexpected character at %lu:%lu, found %c.\n", err.line, err.where, err.ch);
            break;
        case MISSING_CHAR:
            printf("Missing character at %lu%lu, expected %c.\n", err.line, err.where, err.ch);
            break;
        default:
            break;
    }
    return;
}

void run(char *source) {
    size_t exprCount;
    Expr **exprs;
    ParserError err = parse(source, &exprCount, &exprs);

    if (err.errorType != NONE) {
        reportError(err);
        return;
    }

    for (int i = 0; i < exprCount; i++) {
        Expr *expr = exprs[i];
        printExpr(expr);
        freeExpr(expr);
    }
    free(exprs);
}

void runFile(char *fileName) {
    char *fileContents = readFile(fileName);
    if (!fileContents) {
        return;
    }

    run(fileContents);
    free(fileContents);
}

void runPrompt() {
    char *input = NULL;
    size_t size = 0;

    while (true) {
        printf(" > ");
        size_t len = getline(&input, &size, stdin);
        if (len <= 1) {
            break;
        }
        run(input);
    }

    free(input);
}

int main(int argc, char **argv) {
    // if (argc == 2) {
    //     runFile(argv[1]);
    // } else if (argc == 1)  {
    //     runPrompt();
    // } else {
    //     printf("Expects either 1 or 2 args.\n");
    // }
    return 0;
}
