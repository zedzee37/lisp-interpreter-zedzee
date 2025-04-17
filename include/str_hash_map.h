#pragma once

#include <stddef.h>

typedef struct {
    size_t size;
    size_t count;
    void **table;
} StrHashMap;

int strHash(const char *str);
StrHashMap *initStrHashMap();
void freeStrHashMap(StrHashMap *map);
void *getStrHashMap(const StrHashMap *map, const char *key);
void setStrHashMap(StrHashMap *map, const char *key, void *value);
void removeStrHashMap(StrHashMap *map, const char *key);
