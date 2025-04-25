#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define HASH_TABLE_START_SIZE 10
#define MAX_HASH_TABLE_LOAD 0.75

typedef struct {
    char *str;
    void *value;
} Entry;

typedef struct {
    size_t size;
    size_t count;
    Entry *entries;
} HashTable;

uint32_t hashString(const char *str, size_t length);
HashTable *initHashTable();
void freeHashTable(HashTable *table);
bool hashTableSet(HashTable *table, const char *key, void *value);
void *hashTableGet(const HashTable *table, const char *key);
void hashTableDelete(HashTable *table, const char *key);
void hashTableGrow(HashTable *table);
float hashTableLoad(const HashTable *table);
Entry *findEntry(Entry *entries, size_t size, const char *key);

