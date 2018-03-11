//
// Created by user on 3/16/18.
//

#include "modules_binding.h"
#include "util.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CURL_STATICLIB
#include <curl/curl.h>

int get_homepath(char *homepath) {
#ifdef _WIN32
    return snprintf(homepath, sizeof(homepath), "file://localhost/%s%s/.hinarin/modules/", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
    return snprintf(homepath, sizeof(homepath), "file://localhost/%s/.hinarin/modules/", getenv("HOME"));
#endif
}

typedef struct {
    jerry_value_t list;
    jerry_value_t sources;
} modules_t;

static void free_list(void *ptr) {
    modules_t *modules = ptr;
    jerry_release_value(modules->list);
    jerry_release_value(modules->sources);
    free(modules);
}

size_t writer_to_string(void *ptr, size_t size, size_t nmemb, string_t *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size*nmemb;
}

function(unload) {
    modules_t *modules = NULL;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);

    for (uint32_t i = 0; i < jerry_get_array_length(modules->list); i++) {
        jerry_delete_property_by_index(modules->list, i);
    }
    return jerry_create_undefined();
}

function(commit) {
    modules_t *modules = NULL;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);

    char homepath[1024];
    int homepath_len = get_homepath(homepath);

    for (uint32_t i = 0; i < jerry_get_array_length(modules->sources); i++) {
        jerry_value_t source = jerry_get_property_by_index(modules->sources, i);
        js_define_string(source, sourceurl);

        void *curl = curl_easy_init();

        size_t source_len = strlen((const char *) sourceurl);
        if (strncmp("file://", (const char *) sourceurl, 7) == 0) {
            char filestr[homepath_len + source_len - 7 + 1];
            snprintf(filestr, sizeof(filestr), "%s%s", homepath, sourceurl + 7);
            printf("Fetching %s\n", filestr);
            curl_easy_setopt(curl, CURLOPT_URL, filestr);
        } else {
            printf("Fetching %s\n", sourceurl);
            curl_easy_setopt(curl, CURLOPT_URL, sourceurl);
        }

        string_t s = {0, 0};
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer_to_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

        CURLcode err = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (err != 0) {
            printf("Fetching error %d\n", err);
            free(s.ptr);
        } else {
            jerry_value_t func = jerry_parse((const jerry_char_t *) s.ptr, s.len, true);
            if (jerry_value_has_error_flag(func)) {
                jerry_value_clear_error_flag(&func);
                printf("Error: ");
                js_print(func);
            } else {
                jerry_value_t value = jerry_run(func);
                if (jerry_value_has_error_flag(value)) {
                    jerry_value_clear_error_flag(&value);
                    printf("Error: ");
                    js_print(value);
                }
                jerry_release_value(value);
            }
            jerry_release_value(func);
            free(s.ptr);
        }

        jerry_release_value(source);
        jerry_delete_property_by_index(modules->sources, i);
    }
    return jerry_create_undefined();
}

function(add) {
    modules_t *modules = NULL;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);
    
    jerry_set_property_by_index(modules->sources, jerry_get_array_length(modules->sources), args[0]);
    
    return jerry_create_undefined();
}

function(load) {
    modules_t *modules = NULL;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);

    jerry_value_t newname = js_prop(args[0], "name");
    js_define_string(newname, newnamestr);
    size_t newnamestr_len = strlen((const char *) newnamestr);

    for (uint32_t i = 0; i < jerry_get_array_length(modules->list); i++) {
        jerry_value_t module = jerry_get_property_by_index(modules->list, i);

        jerry_value_t name = js_prop(module, "name");
        js_define_string(name, namestr);
        if (strlen((const char *) namestr) == newnamestr_len && strncmp((const char *) namestr, (const char *) newnamestr, newnamestr_len) == 0) {
            jerry_value_t err = jerry_create_error(JERRY_ERROR_COMMON, (const jerry_char_t *) "Module with that name already exists");
            jerry_value_set_error_flag(&err);
            jerry_release_value(module);
            jerry_release_value(newname);
            return err;
        }
        
        jerry_release_value(module);
    }
    jerry_release_value(newname);

    jerry_set_property_by_index(modules->list, jerry_get_array_length(modules->list), args[0]);
    return jerry_create_undefined();
}

function(loaded) {
    modules_t *modules = NULL;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);

    js_define_string(args[0], newnamestr);
    size_t newnamestr_len = strlen((const char *) newnamestr);

    for (uint32_t i = 0; i < jerry_get_array_length(modules->list); i++) {
        jerry_value_t module = jerry_get_property_by_index(modules->list, i);

        jerry_value_t name = js_prop(module, "name");
        js_define_string(name, namestr);
        if (strlen((const char *) namestr) == newnamestr_len && strncmp((const char *) namestr, (const char *) newnamestr, newnamestr_len) == 0) {
            jerry_value_t err = jerry_create_error(JERRY_ERROR_COMMON, (const jerry_char_t *) "Module with that name already exists");
            jerry_value_set_error_flag(&err);
            jerry_release_value(module);
            return jerry_create_boolean(true);
        }

        jerry_release_value(module);
    }

    return jerry_create_boolean(false);
}

function(find) {
    modules_t *modules = NULL;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);

    js_define_string(args[0], newnamestr);
    size_t newnamestr_len = strlen((const char *) newnamestr);

    for (uint32_t i = 0; i < jerry_get_array_length(modules->list); i++) {
        jerry_value_t module = jerry_get_property_by_index(modules->list, i);

        jerry_value_t name = js_prop(module, "name");
        js_define_string(name, namestr);
        if (strlen((const char *) namestr) == newnamestr_len && strncmp((const char *) namestr, (const char *) newnamestr, newnamestr_len) == 0) {
            jerry_value_t err = jerry_create_error(JERRY_ERROR_COMMON, (const jerry_char_t *) "Module with that name already exists");
            jerry_value_set_error_flag(&err);
            return module;
        }

        jerry_release_value(module);
    }

    return jerry_create_null();
}

void bind_modules() {
    jerry_value_t global = jerry_get_global_object();
    jerry_value_t modules = jerry_create_object();
    js_register_prop_name(global, "modules", modules);
    jerry_release_value(global);

    modules_t *mods = calloc(1, sizeof(modules_t));
    mods->list = jerry_create_array(0);
    mods->sources = jerry_create_array(0);

    js_register_function(modules, "unload", unload, mods);
    js_register_function(modules, "commit", commit, mods);
    js_register_function(modules, "add", add, mods);
    js_register_function(modules, "load", load, mods);
    js_register_function(modules, "loaded", loaded, mods);
    js_register_function(modules, "find", find, mods);

    jerry_object_native_info_t list_info;
    list_info.free_cb = free_list;
    jerry_set_object_native_pointer(modules, mods, &list_info);

    jerry_release_value(modules);
}