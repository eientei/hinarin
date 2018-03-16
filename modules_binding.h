//
// Created by user on 3/16/18.
//

#ifndef HINARIN_MODULES_BINDING_H
#define HINARIN_MODULES_BINDING_H

#include <jerryscript.h>
#include <stdlib.h>

typedef struct {
    char *ptr;
    size_t len;
} string_t;

typedef struct {
    jerry_value_t list;
    jerry_value_t sources;
} modules_t;

size_t writer_to_string(void *ptr, size_t size, size_t nmemb, string_t *s);
int get_homepath(char *homepath, size_t size);
void load_module_url(const char* homepath, int homepath_len, const char *sourceurl);

void bind_modules();

#endif //HINARIN_MODULES_BINDING_H
