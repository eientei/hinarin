//
// Created by user on 3/12/18.
//

#ifndef HINARIN_UTIL_H
#define HINARIN_UTIL_H

#include <jerryscript-core.h>

#define function(name) static jerry_value_t (name)(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args[], const jerry_length_t args_count)

void js_register_prop(jerry_value_t parent, jerry_value_t prop_name, jerry_value_t value);
void js_register_prop_name(jerry_value_t parent, const char *prop_name, jerry_value_t value);
void js_register_function(jerry_value_t parent, const char *name, jerry_external_handler_t handler, void *data);
void js_register_double(jerry_value_t parent, const char *name, double value);
void js_register_bool(jerry_value_t parent, const char *name, bool value);
jerry_value_t js_create_abort(const char *message);

double js_to_double(jerry_value_t value);
bool js_to_bool(jerry_value_t value);

#define js_define_string(value, name) jerry_char_t name[jerry_get_string_size(value)+1]; (name)[jerry_string_to_char_buffer((value), (name), jerry_get_string_size(value))] = '\0'

jerry_value_t js_prop(jerry_value_t value, const char *name);
double js_prop_double(jerry_value_t value, const char *name);
bool js_prop_bool(jerry_value_t value, const char *name);

void js_print(jerry_value_t value);
bool js_iter_print(const jerry_value_t property_name, const jerry_value_t property_value, void *user_data_p);

#endif //HINARIN_UTIL_H
