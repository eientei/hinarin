//
// Created by user on 3/23/18.
//

#include "linkedmap.h"

hinarin_linkedmap * hinarin_linkedmap_new() {
    hinarin_linkedmap *map = malloc(sizeof(hinarin_linkedmap));
    map->head = NULL;
    return map;
}

void hinarin_linkedmap_free(hinarin_linkedmap *map) {
    hinarin_linkedmap_bucket *ptr = map->head;
    while (ptr != NULL) {
        hinarin_linkedmap_bucket *next = ptr->next;
        free(ptr->key);
        free(ptr->value);
        free(ptr);
        ptr = next;
    }
    free(map);
}

static hinarin_linkedmap_bucket *hinarin_linkedmap_bucket_new(const char *key, size_t keylen, const char *value, size_t valuelen) {
    hinarin_linkedmap_bucket *bucket = malloc(sizeof(hinarin_linkedmap_bucket));
    bucket->key = malloc(keylen+1);
    bucket->value = malloc(valuelen+1);
    bucket->keylen = keylen;
    bucket->valuelen = valuelen;
    bucket->next = NULL;
    memmove(bucket->key, key, keylen);
    bucket->key[keylen] = '\0';
    memmove(bucket->value, value, valuelen);
    bucket->value[valuelen] = '\0';
    return bucket;
}

void hinarin_linkedmap_put_n(hinarin_linkedmap *map, const char *key, size_t keylen, const char *value, size_t valuelen) {
    hinarin_linkedmap_bucket *ptr = map->head;
    while (ptr != NULL) {
        hinarin_linkedmap_bucket *next = ptr->next;
        if (ptr->keylen == keylen && strncmp(ptr->key, key, keylen) == 0) {
            ptr->value = realloc(ptr->value, valuelen+1);
            memmove(ptr->value, value, valuelen);
            ptr->valuelen = valuelen;
            ptr->value[valuelen] = '\0';
            return;
        }
        if (next == NULL) {
            ptr->next = hinarin_linkedmap_bucket_new(key, keylen, value, valuelen);
            return;
        }
        ptr = next;
    }
    map->head = hinarin_linkedmap_bucket_new(key, keylen, value, valuelen);
}

char * hinarin_linkedmap_get_n(hinarin_linkedmap *map, const char *key, size_t keylen) {
    hinarin_linkedmap_bucket *ptr = map->head;
    while (ptr != NULL) {
        if (ptr->keylen == keylen && strncmp(ptr->key, key, keylen) == 0) {
            return ptr->value;
        }
        ptr = ptr->next;
    }
    return NULL;
}