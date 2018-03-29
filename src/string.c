//
// Created by user on 3/23/18.
//

#include "string.h"
#include <stdarg.h>
#include <stdio.h>

static void hinarin_sring_ensure(hinarin_string *string, size_t required) {
    if (required > string->alloc) {
        if (string->alloc == 0) {
            string->alloc = 2;
        }
        while (required > string->alloc) {
            string->alloc <<= 1;
        }
        string->data = realloc(string->data, string->alloc);
    }
}

hinarin_string *hinarin_string_new() {
    hinarin_string *string = malloc(sizeof(hinarin_string));
    string->length = 0;
    string->alloc = 0;
    string->data = 0;
    return string;
}

void hinarin_string_free(hinarin_string *string) {
    free(string->data);
    free(string);
}

void hinarin_string_append_n(hinarin_string *string, const char *chars, size_t length) {
    size_t required = string->length + length + 1;

    hinarin_sring_ensure(string, required);

    memmove(string->data+string->length, chars, length);
    string->length += length;
    string->data[string->length] = '\0';
}

void hinarin_string_insert_n(hinarin_string *string, size_t pos, const char *chars, size_t length) {
    size_t required = string->length + length + 1;

    hinarin_sring_ensure(string, required);

    memmove(string->data+pos+length, string->data+pos, string->length - pos);
    memmove(string->data + pos, (void *) chars, length);
    string->length += length;
    string->data[string->length] = '\0';
}

bool hinarin_string_equals_n(hinarin_string *string, const char *chars, size_t length) {
    return length == string->length && strncmp(string->data, chars, string->length) == 0;
}

bool hinarin_string_starts_n(hinarin_string *string, const char *chars, size_t length) {
    return length <= string->length && strncmp(string->data, chars, length) == 0;
}

bool hinarin_string_ends_n(hinarin_string *string, const char *chars, size_t length) {
    return length <= string->length && strncmp(string->data+string->length - length, chars, length) == 0;
}

bool hinarin_string_contains_n(hinarin_string *string, const char *chars, size_t length) {
    for (size_t i = 0; i < string->length - length; i++) {
        if (strncmp(string->data+i, chars, length) == 0) {
            return true;
        }
    }
    return false;
}

void hinarin_string_sprintf(hinarin_string *string, const char *format, ...) {
    va_list ap;
    va_start(ap, format);
    size_t length = (size_t) vsnprintf(NULL, 0, format, ap);
    va_end(ap);
    hinarin_sring_ensure(string, length+1);
    va_start(ap, format);
    vsnprintf(string->data, string->length, format, ap);
    va_end(ap);
}