//
// Created by user on 3/12/18.
//

#include "util.h"

#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#endif

#define CURL_STATICLIB
#include <curl/curl.h>

size_t writer_to_string(void *ptr, size_t size, size_t nmemb, string_t *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size*nmemb;
}

size_t writer_to_length(void *ptr, size_t size, size_t nmemb, int *len) {
    *len += size*nmemb;
    return size*nmemb;
}

size_t writer_to_file(void *ptr, size_t size, size_t nmemb, FILE *f) {
    return fwrite(ptr, size, nmemb, f) * size;
}

size_t writer_to_file_progress(void *ptr, size_t size, size_t nmemb, progress_t *prog) {
    size_t current = fwrite(ptr, size, nmemb, prog->file) * size;
    prog->processed += current;
    if (prog->iter == 0) {
        curl_off_t total;
        curl_easy_getinfo(prog->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &total);
        prog->total = total;
    }
    if (prog->progress) {
        prog->progress(prog->processed, prog->total);
    }
    prog->iter++;
    return current;
}

size_t headers_to_jerry_object(char *buffer, size_t size, size_t nitems, jerry_value_t *object) {
    size_t processed = size * nitems;
    int keylen = 0;
    int seplen = 0;
    int valuelen = 0;

    for (size_t i = 0; i < processed; i++) {
        if (seplen == 0) {
            if (buffer[i] == ':') {
                seplen = 1;
            } else {
                keylen++;
            }
        } else if (valuelen == 0) {
            if (buffer[i] != ' ') {
                valuelen++;
            } else {
                seplen++;
            }
        } else if (buffer[i] != '\r' && buffer[i] != '\n') {
            valuelen++;
        }
    }
    if (keylen > 0 && valuelen > 0) {
        jerry_value_t key = jerry_create_string_sz((const jerry_char_t *) buffer, (jerry_size_t) keylen);
        jerry_value_t value = jerry_create_string_sz((const jerry_char_t *) buffer + keylen + seplen, (jerry_size_t) valuelen);
        js_register_prop(*object, key, value);
        jerry_release_value(key);
        jerry_release_value(value);
    }

    return processed;
}

size_t get_homepath(char *homepath, size_t size) {
#ifdef _WIN32
    return (size_t) snprintf(homepath, size, "file://localhost/%s%s/.hinarin/modules/", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
    return (size_t) snprintf(homepath, size, "file://localhost/%s/.hinarin/modules/", getenv("HOME"));
#endif
}

void cross_mkdir(const char *path) {
#ifdef _WIN32
    _mkdir(path);
#else
    mkdir(path, 0700);
#endif
}

void mkdir_p(char* path) {
    if (mkdir_basedir(path)) {
        cross_mkdir(path);
    }
}

int mkdir_basedir(char *path) {
    int last = 0;
    for (char *c = path; *c != '\0'; c++) {
        if (last && *c == '/') {
            *c = '\0';
            cross_mkdir(path);
            *c = '/';
            last = 0;
        } else {
            last = 1;
        }
    }
    return last;
}

jerry_value_t download_to_file(const char *url, char *localfile, progress_func progress) {
    mkdir_basedir(localfile);
    jerry_value_t response = jerry_create_object();

    progress_t prog = { 0 };
    prog.headers = jerry_create_object();
    prog.progress = progress;
    prog.file = fopen(localfile, "a");
    prog.curl = curl_easy_init();
    curl_easy_setopt(prog.curl, CURLOPT_URL, url);
    curl_easy_setopt(prog.curl, CURLOPT_WRITEFUNCTION, writer_to_file_progress);
    curl_easy_setopt(prog.curl, CURLOPT_WRITEDATA, &prog);
    curl_easy_setopt(prog.curl, CURLOPT_HEADERFUNCTION, headers_to_jerry_object);
    curl_easy_setopt(prog.curl, CURLOPT_HEADERDATA, &prog.headers);
    CURLcode err = curl_easy_perform(prog.curl);
    long response_code;
    curl_easy_getinfo(prog.curl, CURLINFO_RESPONSE_CODE, &response_code);
    struct curl_slist *cookielist, *nc;
    curl_easy_getinfo(prog.curl, CURLINFO_COOKIELIST, &cookielist);
    js_register_double(response, "code", response_code);
    js_register_double(response, "error", err);
    js_register_prop_name(response, "headers", prog.headers);
    jerry_release_value(prog.headers);
    nc = cookielist;
    while (nc) {
        printf("%s\n", nc->data);
        nc = nc->next;
    }
    curl_slist_free_all(cookielist);

    curl_easy_cleanup(prog.curl);
    fclose(prog.file);

    return response;
}

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
    if (jerry_value_is_object(property_value)) {
        printf("{\n");
        jerry_foreach_object_property(property_value, js_iter_print, NULL);
        printf("}\n");
    } else {
        js_print(property_value);
    }
    return true;
}