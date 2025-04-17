#include "hash_table.h"
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// FNV-1a hashing algorithm
static uint32_t hashString(const char* str, size_t length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)str[i];
        hash *= 16777619;
    }
    return hash;
}

static float hashTableLoad(const HashTable *table) {
    return (float)table->count / (float)table->size;
}
HashTable *initHashTable() {
    HashTable *table = malloc(sizeof(HashTable));
    table->size = HASH_TABLE_START_SIZE;
    table->count = 0;
    table->entries = calloc(table->size, sizeof(Entry));
    return table;
}

void *hashTableGet(const HashTable *table, const char *key) {
    Entry *entry = findEntry(table->entries, table->size, key);
    return entry->value;
}

void hashTableSet(HashTable *table, const char *key, void *value) {
    if (hashTableLoad(table) >= MAX_HASH_TABLE_LOAD) {
        hashTableGrow(table);
    }
    Entry *entry = findEntry(table->entries, table->size, key);
    entry->value = value;
}

void hashTableDelete(HashTable *table, const char *key) {
    if (hashTableLoad(table) >= MAX_HASH_TABLE_LOAD) {
        hashTableGrow(table);
    }
    Entry *entry = findEntry(table->entries, table->size, key);
    entry->str = NULL;
    entry->value = NULL;
}

static void hashTableGrow(HashTable *table) {
    size_t oldSize = table->size;
    
    table->size *= 2;
    Entry *entries = calloc(table->size, sizeof(Entry));

    for (int i = 0; i < oldSize; i++) {
        Entry e = table->entries[i];

        if (e.str == NULL) {
            continue;
        }

        Entry *entryAddress = findEntry(entries, table->size, e.str);
        *entryAddress = (Entry){ e.str, e.value };
    }

    free(table->entries);
    table->entries = entries;
}

static Entry *findEntry(const Entry *entries, size_t size, const char *key) {
    uint32_t idx = hashString(key, strlen(key)) % size;

    while (entries[idx].str != NULL || strcmp(entries[idx].str, key) != 0) {
        idx = (idx + 1) % size; 
    }

    return &entries[idx];
}

void freeHashTable(HashTable *table) {
    free(table); 
}

