//
// Created by user on 3/23/18.
//

#include "file.h"
#include "string.h"

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

int hinarin_mkdir(const char *name) {
#ifdef _WIN32
    return _mkdir(name);
#else
    return mkdir(name, 0700);
#endif
}

void hinarin_mkdir_recursive(const char *name) {
    if (hinarin_mkdir_basedir_recursive(name)) {
        hinarin_mkdir(name);
    }
}

bool hinarin_mkdir_basedir_recursive(const char *name) {
    bool last = false;
    for (char *c = (char *) name; *c != '\0'; c++) {
        if (last && *c == '/') {
            *c = '\0';
            hinarin_mkdir(name);
            *c = '/';
            last = false;
        } else {
            last = true;
        }
    }
    return last;
}

void hinarin_homedir(hinarin_string *string) {
#ifdef _WIN32
    hinarin_string_append(string, getenv("HOMEDRIVE"));
    hinarin_string_append(string, getenv("HOMEPATH"));
    for (size_t i = 0; i < string->length; i++) {
        if (string->data[i] == '\\') {
            string->data[i] = '/';
        }
    }
#else
    hinarin_string_append(string, getenv("HOME"));
#endif
}