//
// Created by user on 3/23/18.
//

#ifndef HINARIN_LINKEDMAP_H
#define HINARIN_LINKEDMAP_H

#include <stdlib.h>
#include <string.h>

typedef struct hinarin_linkedmap_bucket {
    struct hinarin_linkedmap_bucket *next;
    char *key;
    size_t keylen;
    char *value;
    size_t valuelen;
} hinarin_linkedmap_bucket;

typedef struct {
    hinarin_linkedmap_bucket *head;
} hinarin_linkedmap;


#define hinarin_linkedmap_put(map, key, value) hinarin_linkedmap_put_n((map), (key), strlen((key)), (value), strlen((value)))
#define hinarin_linkedmap_get(map, key) hinarin_linkedmap_get_n((map), (key), strlen((key)))

hinarin_linkedmap * hinarin_linkedmap_new();
void hinarin_linkedmap_free(hinarin_linkedmap *map);
void hinarin_linkedmap_put_n(hinarin_linkedmap *map, const char *key, size_t keylen, const char *value, size_t valuelen);
char * hinarin_linkedmap_get_n(hinarin_linkedmap *map, const char *key, size_t keylen);

#endif //HINARIN_LINKEDMAP_H
