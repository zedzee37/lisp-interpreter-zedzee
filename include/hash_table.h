#pragma once

#include <stddef.h>
#include <stdint.h>

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

static uint32_t hashString(const char *str, size_t length);
HashTable *initHashTable();
void freeHashTable(HashTable *table);
void hashTableSet(HashTable *table, const char *key, void *value);
void *hashTableGet(const HashTable *table, const char *key);
void hashTableGrow(HashTable *table);
static float hashTableLoad(const HashTable *table);
static Entry *findEntry(const Entry *entries, const char *key);

