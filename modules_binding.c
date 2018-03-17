//
// Created by user on 3/16/18.
//

#include "modules_binding.h"
#include "util.h"
#include <stdio.h>
#include <string.h>

void hinarin_load_module(const char *url) {
    jerry_value_t value = hinarin_download_to_data(url, NULL, NULL, NULL, NULL);

    hinarin_get_name_string(value, "data", data);
    jerry_value_t func = jerry_parse((const jerry_char_t *) data, strlen(data), true);
    if (jerry_value_has_error_flag(func)) {
        hinarin_print(func);
    } else {
        jerry_value_t result = jerry_run(func);
        if (jerry_value_has_error_flag(result)) {
            hinarin_print(result);
        }
        jerry_release_value(result);
    }
    jerry_release_value(func);
    jerry_release_value(value);
}

hinarin_function(hinarin_modules_unload) {
    modules_t *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    for (uint32_t i = 0; i < jerry_get_array_length(ctx->modules_list); i++) {
        jerry_delete_property_by_index(ctx->modules_list, i);
    }
    return jerry_create_undefined();
}

hinarin_function(hinarin_modules_load) {
    modules_t *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    for (uint32_t i = 0; i < jerry_get_array_length(ctx->modules_sources_to_load); i++) {
        hinarin_get_index_string(ctx->modules_sources_to_load, i, url);
        hinarin_load_module(url);
        jerry_delete_property_by_index(ctx->modules_sources_to_load, i);
    }

    return jerry_create_undefined();
}

hinarin_function(hinarin_modules_add) {
    modules_t *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    jerry_set_property_by_index(ctx->modules_sources_to_load, jerry_get_array_length(ctx->modules_sources_to_load), args[0]);

    return jerry_create_undefined();
}

hinarin_function(hinarin_modules_define) {
    modules_t *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_get_name_string(args[0], "name", name);

    for (uint32_t i = 0; i < jerry_get_array_length(ctx->modules_list); i++) {
        hinarin_get_index_string(ctx->modules_list, i, exname);
        if (hinarin_equals(exname, name)) {
            jerry_value_t err = hinarin_create_error("Module with that name is already exists");
            jerry_value_set_error_flag(&err);
            return err;
        }
    }

    jerry_set_property_by_index(ctx->modules_list, jerry_get_array_length(ctx->modules_list), args[0]);
    return  jerry_create_undefined();
}

hinarin_function(hinarin_modules_find) {
    modules_t *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_get_name_string(args[0], "name", name);

    for (uint32_t i = 0; i < jerry_get_array_length(ctx->modules_list); i++) {
        hinarin_get_index_string(ctx->modules_list, i, exname);
        if (hinarin_equals(exname, name)) {
            return jerry_get_property_by_index(ctx->modules_list, i);
        }
    }

    return jerry_create_null();
}

hinarin_function(hinarin_modules_loaded) {
    modules_t *ctx;
    jerry_get_object_native_pointer(function_obj, (void **) &ctx, NULL);

    hinarin_get_name_string(args[0], "name", name);

    for (uint32_t i = 0; i < jerry_get_array_length(ctx->modules_list); i++) {
        hinarin_get_index_string(ctx->modules_list, i, exname);
        if (hinarin_equals(exname, name)) {
            return jerry_create_boolean(true);
        }
    }

    return jerry_create_boolean(false);
}

void hinarin_modules_bind() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t modules = jerry_create_object();
    hinarin_set_name_value(global, "modules", modules);

    modules_t *ctx = calloc(1, sizeof(modules_t));
    ctx->modules_list = jerry_create_array(0);
    ctx->modules_sources_to_load = jerry_create_array(0);

    hinarin_set_name_function(modules, "unload", hinarin_modules_unload, ctx);
    hinarin_set_name_function(modules, "load", hinarin_modules_load, ctx);
    hinarin_set_name_function(modules, "add", hinarin_modules_add, ctx);
    hinarin_set_name_function(modules, "define", hinarin_modules_define, ctx);
    hinarin_set_name_function(modules, "find", hinarin_modules_find, ctx);
    hinarin_set_name_function(modules, "loaded", hinarin_modules_loaded, ctx);

    jerry_set_object_native_pointer(modules, ctx, NULL);
    jerry_release_value(modules);
    jerry_release_value(global);
}