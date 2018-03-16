//
// Created by user on 3/12/18.
//

#ifndef HINARIN_UTIL_H
#define HINARIN_UTIL_H

#include <jerryscript.h>
#include <stdio.h>

typedef struct {
    char *ptr;
    size_t len;
} string_t;

typedef void (*progress_func)(size_t processed_bytes, ssize_t total_bytes);

typedef struct {
    size_t iter;
    ssize_t total;
    size_t processed;
    progress_func progress;
    jerry_value_t headers;
    FILE *file;
    void *curl;
} progress_t;

size_t writer_to_file_progress(void *ptr, size_t size, size_t nmemb, progress_t *prog);
size_t writer_to_string(void *ptr, size_t size, size_t nmemb, string_t *s);
size_t writer_to_length(void *ptr, size_t size, size_t nmemb, int *len);
size_t writer_to_file(void *ptr, size_t size, size_t nmemb, FILE *f);
size_t headers_to_jerry_object(char *buffer, size_t size, size_t nitems, jerry_value_t *object);
void cross_mkdir(const char *path);
void mkdir_p(char* path);
int mkdir_basedir(char *path);
size_t get_homepath(char *homepath, size_t size);
jerry_value_t download_to_file(const char *url, char *localfile, progress_func progress);

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
