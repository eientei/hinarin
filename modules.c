//
// Created by user on 3/12/18.
//

#include "modules.h"

#define CURL_STATICLIB

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

struct string {
    char *ptr;
    size_t len;
};

static jerry_value_t modules_sources;

/*
jerry_value_t addsource(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args[], const jerry_length_t args_count) {
    if (args_count != 1) {
        return js_create_abort("Invalid argument");
    }
    if (!jerry_value_is_string(args[0])) {
        return js_create_abort("Invalid argument");
    }

    jerry_value_t result = jerry_set_property_by_index(modules_sources, jerry_get_array_length(modules_sources), args[0]);
    jerry_release_value(result);
    return jerry_create_undefined();
}

jerry_value_t removesource(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t args[], const jerry_length_t args_count) {
    if (args_count != 1) {
        return js_create_abort("Invalid argument");
    }
    if (!jerry_value_is_number(args[0])) {
        return js_create_abort("Invalid argument");
    }
    
    double index = js_to_double(args[0]);
    
    jerry_delete_property_by_index(modules_sources, (uint32_t) index);
    return jerry_create_undefined();
}

void modules_init() {
    modules_sources = jerry_create_array(0);
}

size_t writer_to_string(void *ptr, size_t size, size_t nmemb, struct string *s) {
    size_t new_len = s->len + size*nmemb;
    s->ptr = realloc(s->ptr, new_len+1);
    memcpy(s->ptr+s->len, ptr, size*nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    printf("FFFF %s %ld\n", s->ptr, s->len);
    return size*nmemb;
}

void modules_load(jerry_value_t modules) {
    char *home = "localhost/home/user/soft/hinarin/base/";
    size_t home_len = strlen(home);
    for (uint32_t  i = 0; i < jerry_get_array_length(modules_sources); i++) {
        jerry_value_t source = jerry_get_property_by_index(modules_sources, i);
        js_define_string(source, str);

        struct string s = {0, 0};

        void *curl = curl_easy_init();
        size_t len = strlen((const char *) str);
        if (len > 7 && memcmp("file://", str, 7) == 0) {
            char filestr[len+home_len+1];
            memmove(filestr, "file://", 7);
            memmove(filestr+7, home, home_len);
            memmove(filestr+7+home_len, str+7, len-7);
            filestr[len+home_len] = '\0';
            printf("Fetching %s\n", filestr);
            curl_easy_setopt(curl, CURLOPT_URL, filestr);
        } else {
            printf("Fetching %s\n", str);
            curl_easy_setopt(curl, CURLOPT_URL, str);
        }

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer_to_string);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
        CURLcode err = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (err != 0) {
            printf("Curl error %d\n", err);
            free(s.ptr);
            jerry_release_value(source);
            continue;
        }
        jerry_value_t value = jerry_eval((const jerry_char_t *) s.ptr, s.len, true);
        if (jerry_value_has_error_flag(value)) {
            jerry_value_clear_error_flag(&value);
            js_print(value);
        }
        jerry_release_value(value);
        
        free(s.ptr);

        jerry_release_value(source);
    }
}*/