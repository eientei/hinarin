#ifndef HINARIN_UTIL_H
#define HINARIN_UTIL_H

#include <stdlib.h>
#include <jerryscript.h>

// helper functions

jerry_value_t hinarin_create_error(const char *name);
bool hinarin_equals(const char *a, const char *b);
bool hinarin_starts_with(const char *str, const char *prefix);
bool hinarin_has_key(jerry_value_t value, const char *keyname);
bool hinarin_has_own_key(jerry_value_t value, const char *keyname);
void hinarin_print_recursive(jerry_value_t value, int currentdepth);
void hinarin_print(jerry_value_t value);
void hinarin_set_name_value(jerry_value_t parent, const char *keyname, jerry_value_t value);
void hinarin_set_name_bool(jerry_value_t parent, const char *keyname, bool value);
void hinarin_set_name_string(jerry_value_t parent, const char *keyname, char *value);
void hinarin_set_name_number(jerry_value_t parent, const char *keyname, double value);
void hinarin_set_name_function(jerry_value_t parent, const char *keyname, jerry_external_handler_t value, void *context);
void hinarin_set_name_null(jerry_value_t parent, const char *keyname);
void hinarin_set_name_undefined(jerry_value_t parent, const char *keyname);
void hinarin_unset_release(jerry_value_t parent, jerry_value_t releasekey);
void hinarin_unset_name(jerry_value_t parent, const char *keyname);
void hinarin_set_release(jerry_value_t parent, jerry_value_t releasekey, jerry_value_t releasevalue);
void hinarin_set_release_key(jerry_value_t parent, jerry_value_t releasekey, jerry_value_t value);
void hinarin_set_release_value(jerry_value_t parent, jerry_value_t key, jerry_value_t releasevalue);
void hinarin_set_name_release(jerry_value_t parent, const char *keyname, jerry_value_t releasevalue);
size_t hinarin_string_size_release(jerry_value_t releasevalue);
size_t hinarin_string_to_buffer_release(jerry_value_t releasevalue, char *buf, size_t buflen);
jerry_value_t hinarin_get_release(jerry_value_t parent, jerry_value_t releasekey);
bool hinarin_get_release_bool(jerry_value_t parent, jerry_value_t releasekey);

#define hinarin_get_release_string(parent, releasekey, stringname) \
  char stringname[hinarin_string_size_release(jerry_get_property((parent), (releasekey)))+1]; \
  (stringname)[hinarin_string_to_buffer_release(jerry_get_property((parent), (releasekey)), (stringname), hinarin_string_size_release(jerry_get_property((parent), (releasekey))))] = '\0'; \
  jerry_release_value((releasekey))

double hinarin_get_release_number(jerry_value_t parent, jerry_value_t releasekey);

jerry_value_t hinarin_get_index_release(jerry_value_t releasevalue, uint32_t index);
bool hinarin_get_index_bool(jerry_value_t parent, uint32_t index);
double hinarin_get_index_number(jerry_value_t parent, uint32_t index);

#define hinarin_get_index_string(parent, index, stringname) \
  char stringname[hinarin_string_size_release(jerry_get_property_by_index((parent), (index)))+1]; \
  (stringname)[hinarin_string_to_buffer_release(jerry_get_property_by_index((parent), (index)), (stringname), hinarin_string_size_release(jerry_get_property_by_index((parent), (index))))] = '\0'


jerry_value_t hinarin_get_name(jerry_value_t parent, const char* keyname);
bool hinarin_get_name_bool(jerry_value_t parent, const char *keyname);

#define hinarin_get_name_string(parent, keyname, stringname) \
  char stringname[hinarin_string_size_release(hinarin_get_release((parent), jerry_create_string((jerry_char_t *)(keyname))))+1]; \
  (stringname)[hinarin_string_to_buffer_release(hinarin_get_release((parent), jerry_create_string((jerry_char_t *)(keyname))), (stringname), hinarin_string_size_release(hinarin_get_release((parent), jerry_create_string((jerry_char_t *)(keyname)))))] = '\0'

double hinarin_get_name_number(jerry_value_t parent, const char *keyname);

#define hinarin_string(value, stringname) \
  char stringname[jerry_get_string_size((value))+1]; \
  (stringname)[jerry_string_to_char_buffer((value), (jerry_char_t *)(stringname), jerry_get_string_size((value)))] = '\0'

// native functions
int hinarin_mkdir(const char *path);
int hinarin_mkdir_recursive(const char *path);
int hinarin_mkdir_basedir_recursive(const char *path);
size_t hinarin_homedir(char *buf, size_t buflen);

typedef size_t (*hinarin_writefunc)(void *ptr, size_t length, void *data);
typedef void (*hinarin_progressfunc)(size_t process_bytes, ssize_t total_bytes, void *data);

// semi-native functions
jerry_value_t hinarin_download(const char *url, jerry_value_t *sendheaders, jerry_value_t *sendcookies, hinarin_writefunc writefunc, void *writefunc_data, hinarin_progressfunc progressfunc, void *progressfunc_data);
jerry_value_t hinarin_download_to_file(const char *url, const char *filepath, jerry_value_t *sendheaders, jerry_value_t *sendcookies, hinarin_progressfunc progressfunc, void *progressfunc_data);
jerry_value_t hinarin_download_to_data(const char *url, jerry_value_t *sendheaders, jerry_value_t *sendcookies, hinarin_progressfunc progressfunc, void *progressfunc_data);

#define hinarin_function(name) static jerry_value_t (name)(const jerry_value_t function_obj, const jerry_value_t this_val, const jerry_value_t *args, const jerry_length_t args_count)

#endif //HINARIN_UTIL_H
