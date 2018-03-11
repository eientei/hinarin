//
// Created by user on 3/12/18.
//

#ifndef HINARIN_MODULES_H
#define HINARIN_MODULES_H

#include <jerryscript.h>
#include "util.h"

jerry_value_t addsource(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args[], const jerry_length_t args_count);
jerry_value_t removesource(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args[], const jerry_length_t args_count);

void modules_init();
void modules_load(jerry_value_t modules);

#endif //HINARIN_MODULES_H
