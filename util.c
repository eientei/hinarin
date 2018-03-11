//
// Created by user on 3/12/18.
//

#include "util.h"

#include <stdio.h>

void js_register_prop(jerry_value_t parent, jerry_value_t prop_name, jerry_value_t value) {
    jerry_value_t result = jerry_set_property(parent, prop_name, value);
    jerry_release_value(result);
}

void js_register_prop_name(jerry_value_t parent, const char *prop_name, jerry_value_t value) {
    jerry_value_t prop = jerry_create_string((const jerry_char_t *) prop_name);
    js_register_prop(parent, prop, value);
    jerry_release_value(prop);
}

void js_register_function(jerry_value_t parent, const char *name, jerry_external_handler_t handler, void *data) {
    jerry_value_t func_obj = jerry_create_external_function(handler);
    js_register_prop_name(parent, name, func_obj);
    jerry_set_object_native_pointer(func_obj, data, NULL);
    jerry_release_value(func_obj);
}

void js_register_double(jerry_value_t parent, const char *name, double value) {
    jerry_value_t number_obj = jerry_create_number(value);
    js_register_prop_name(parent, name, number_obj);
    jerry_release_value(number_obj);
}

void js_register_bool(jerry_value_t parent, const char *name, bool value) {
    jerry_value_t bool_obj = jerry_create_boolean(value);
    js_register_prop_name(parent, name, bool_obj);
    jerry_release_value(bool_obj);
}

jerry_value_t js_create_abort(const char *message) {
    jerry_value_t error_obj = jerry_create_error(JERRY_ERROR_COMMON, (const jerry_char_t *) message);
    jerry_value_set_abort_flag(&error_obj);
    return error_obj;
}

double js_to_double(jerry_value_t value) {
    jerry_value_t dbl = jerry_value_to_number(value);
    double data = jerry_get_number_value(dbl);
    jerry_release_value(dbl);
    return data;
}

bool js_to_bool(jerry_value_t value) {
    return jerry_value_to_boolean(value);
}

jerry_value_t js_prop(jerry_value_t value, const char *name) {
    jerry_value_t obj = jerry_value_to_object(value);
    jerry_value_t prop_obj = jerry_create_string((const jerry_char_t *) name);

    jerry_value_t prop = jerry_get_property(obj, prop_obj);

    jerry_release_value(prop_obj);
    jerry_release_value(obj);

    return prop;
}

double js_prop_double(jerry_value_t value, const char *name) {
    jerry_value_t prop = js_prop(value, name);

    double dbl = js_to_double(prop);

    jerry_release_value(prop);

    return dbl;
}

bool js_prop_bool(jerry_value_t value, const char *name) {
    jerry_value_t prop = js_prop(value, name);

    bool bln = js_to_bool(prop);

    jerry_release_value(prop);

    return bln;
}

void js_print(jerry_value_t value) {
    jerry_value_t str = jerry_value_to_string(value);
    js_define_string(str, text);
    printf("%s\n", (char *) text);
    jerry_release_value(str);
}

bool js_iter_print(const jerry_value_t property_name, const jerry_value_t property_value, void *user_data_p) {
    js_define_string(property_name, name);
    printf("%s: ", (char *) name);
    js_print(property_value);
    return true;
}