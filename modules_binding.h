//
// Created by user on 3/16/18.
//

#ifndef HINARIN_MODULES_BINDING_H
#define HINARIN_MODULES_BINDING_H

#include <jerryscript.h>
#include <stdlib.h>

void load_module_url(const char* homepath, size_t homepath_len, const char *sourceurl);
void bind_modules();

#endif //HINARIN_MODULES_BINDING_H
