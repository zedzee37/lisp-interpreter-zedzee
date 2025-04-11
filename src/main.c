#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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

void run(char *source) {
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
        run(input);

        if (len <= 1) {
            break;
        }
    }

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
