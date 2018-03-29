//
// Created by user on 3/23/18.
//

#ifndef HINARIN_STRING_H
#define HINARIN_STRING_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t length;
    size_t alloc;
} hinarin_string;

#define hinarin_string_append(string, chars) hinarin_string_append_n((string), (chars), strlen((chars)))
#define hinarin_string_insert(string, pos, chars) hinarin_string_insert_n((string), (pos), (chars), strlen((chars)))
#define hinarin_string_equals(string, chars) hinarin_string_equals_n((string), (chars), strlen((chars)))
#define hinarin_string_starts(string, chars) hinarin_string_starts_n((string), (chars), strlen((chars)))
#define hinarin_string_ends(string, chars) hinarin_string_ends_n((string), (chars), strlen((chars)))
#define hinarin_string_contains(string, chars) hinarin_string_contains_n((string), (chars), strlen((chars)))

hinarin_string *hinarin_string_new();
void hinarin_string_free(hinarin_string *string);
void hinarin_string_append_n(hinarin_string *string, const char *chars, size_t length);
void hinarin_string_insert_n(hinarin_string *string, size_t pos, const char *chars, size_t length);
bool hinarin_string_equals_n(hinarin_string *string, const char *chars, size_t length);
bool hinarin_string_starts_n(hinarin_string *string, const char *chars, size_t length);
bool hinarin_string_ends_n(hinarin_string *string, const char *chars, size_t length);
bool hinarin_string_contains_n(hinarin_string *string, const char *chars, size_t length);
void hinarin_string_sprintf(hinarin_string *string, const char *format, ...);

#endif //HINARIN_STRING_H
