//
// Created by user on 3/23/18.
//

#ifndef HINARIN_FILE_H
#define HINARIN_FILE_H

#include <stdbool.h>

#include "string.h"

int hinarin_mkdir(const char *name);
void hinarin_mkdir_recursive(const char *name);
bool hinarin_mkdir_basedir_recursive(const char *name);
void hinarin_homedir(hinarin_string *string);

#endif //HINARIN_FILE_H
