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
}

void hashTableSet(HashTable *table, const char *key, void *value) {
    if (hashTableLoad(table) >= MAX_HASH_TABLE_LOAD) {
        hashTableGrow(table);
    }
}

void hashTableGrow(HashTable *table) {
    size_t oldSize = table->size;
    table->size *= 2;
    Entry *entries = calloc(table->size, sizeof(Entry));

    for (int i = 0; i < oldSize; i++) {
        Entry e = table->entries[i];

        if (e.str == NULL) {
            continue;
        }


        size_t strLen = strlen(e.str);
        uint32_t hash = hashString(e.str, strLen);
        entries[hash % table->size] = (Entry) { e.str, e.value };
    }

    free(table->entries);
    table->entries = entries;
}

Entry *findEntry(const Entry *entries, const char *key);

void freeHashTable(HashTable *table) {
    free(table); 
}

