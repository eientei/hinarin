//
// Created by user on 3/16/18.
//

#ifndef HINARIN_MODULES_BINDING_H
#define HINARIN_MODULES_BINDING_H

#include <jerryscript.h>
#include <stdlib.h>

typedef struct {
    jerry_value_t modules_list;
    jerry_value_t modules_sources_to_load;
} modules_t;

void hinarin_load_module(const char *sourceurl);
void hinarin_modules_bind();

#endif //HINARIN_MODULES_BINDING_H
