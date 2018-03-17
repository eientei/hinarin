//
// Created by user on 3/12/18.
//

#include "util.h"

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#endif

#define CURL_STATICLIB
#include <curl/curl.h>
#include <sys/stat.h>

jerry_value_t hinarin_create_error(const char *name) {
    jerry_value_t err = jerry_create_error(JERRY_ERROR_COMMON, (const jerry_char_t *) name);
    jerry_value_set_error_flag(&err);
    return err;
}

bool hinarin_equals(const char *a, const char *b) {
    return strlen(a) == strlen(b) && strcmp(a, b) == 0;
}

bool hinarin_starts_with(const char *str, const char *prefix) {
    return strlen(str) >= strlen(prefix) && strcmp(str, prefix) == 0;
}

bool hinarin_has_key(jerry_value_t value, const char *keyname) {
    jerry_value_t string = jerry_create_string((const jerry_char_t *) keyname);
    jerry_value_t boolean = jerry_has_property(value, string);
    bool result = jerry_get_boolean_value(boolean);
    jerry_release_value(boolean);
    jerry_release_value(string);
    return result;
}
bool hinarin_has_own_key(jerry_value_t value, const char *keyname) {
    jerry_value_t string = jerry_create_string((const jerry_char_t *) keyname);
    jerry_value_t boolean = jerry_has_own_property(value, string);
    bool result = jerry_get_boolean_value(boolean);
    jerry_release_value(boolean);
    jerry_release_value(string);
    return result;
}

typedef struct {
    int depth;
    int iter;
} depth_iter_t;

static bool foreach_print(const jerry_value_t property_name, const jerry_value_t property_value, void *data) {
    depth_iter_t *d = data;
    if (d->iter > 0) {
        printf(",");
    }
    printf("\n");
    for (int i = 0; i < d->depth; i++) {
        printf(" ");
    }
    hinarin_print_recursive(property_name, 0);
    printf(": ");
    hinarin_print_recursive(property_value, d->depth);

    d->iter++;
    return true;
}

void hinarin_print_recursive(jerry_value_t value, int currentdepth){
    if (jerry_value_has_error_flag(value)) {
        jerry_error_t err = jerry_get_error_type(value);
        switch (err) {
            case JERRY_ERROR_COMMON:
                printf("Error");
                if (jerry_value_is_string(err)) {
                    hinarin_string(err, errmsg);
                    printf(": %s", errmsg);
                }
                break;
            case JERRY_ERROR_EVAL:
                printf("EvalError");
                break;
            case JERRY_ERROR_RANGE:
                printf("RangeError");
                break;
            case JERRY_ERROR_REFERENCE:
                printf("ReferenceError");
                break;
            case JERRY_ERROR_SYNTAX:
                printf("SyntaxError");
                break;
            case JERRY_ERROR_TYPE:
                printf("TypeError");
                break;
            case JERRY_ERROR_URI:
                printf("URIError");
                break;
        }
    } else if (jerry_value_is_number(value)) {
        printf("%g",jerry_get_number_value(value));
    } else if (jerry_value_is_string(value)) {
        hinarin_string(value, text);
        printf("\"%s\"", text);
    } else if (jerry_value_is_null(value)) {
        printf("null");
    } else if (jerry_value_is_undefined(value)) {
        printf("undefined");
    } else if (jerry_value_is_boolean(value)) {
        bool val = jerry_get_boolean_value(value);
        printf(val ? "true" : "false");
    } else if (jerry_value_is_function(value)) {
        printf("function () { ... }");
    } else if (jerry_value_is_array(value)) {
        printf("[");
        for(uint32_t idx = 0; idx < jerry_get_array_length(value); idx++) {
            if (idx > 0) {
                printf(",");
            }
            printf("\n");
            for (int i = 0; i < currentdepth+2; i++) {
                printf(" ");
            }
            jerry_value_t el = jerry_get_property_by_index(value, idx);
            hinarin_print_recursive(el, currentdepth+2);
            jerry_release_value(el);
        }
        printf("\n");
        for (int i = 0; i < currentdepth; i++) {
            printf(" ");
        }
        printf("]");
    } else if (jerry_value_is_object(value)) {
        printf("{");
        depth_iter_t d;
        d.depth = currentdepth+2;
        d.iter = 0;
        jerry_foreach_object_property(value, foreach_print, &d);

        printf("\n");
        for (int i = 0; i < currentdepth; i++) {
            printf(" ");
        }
        printf("}");
    }
}

void hinarin_print(jerry_value_t value) {
    hinarin_print_recursive(value, 0);
    printf("\n");
}

void hinarin_set_name_value(jerry_value_t parent, const char *keyname, jerry_value_t value) {
    hinarin_set_release_key(parent, jerry_create_string((const jerry_char_t *) keyname), value);
}

void hinarin_set_name_bool(jerry_value_t parent, const char *keyname, bool value) {
    hinarin_set_release_key(parent, jerry_create_string((const jerry_char_t *) keyname), jerry_create_boolean(value));
}

void hinarin_set_name_string(jerry_value_t parent, const char *keyname, char *value) {
    hinarin_set_release_key(parent, jerry_create_string((const jerry_char_t *) keyname), jerry_create_string((const jerry_char_t *) value));
}

void hinarin_set_name_number(jerry_value_t parent, const char *keyname, double value) {
    hinarin_set_release_key(parent, jerry_create_string((const jerry_char_t *) keyname), jerry_create_number(value));
}

void hinarin_set_name_function(jerry_value_t parent, const char *keyname, jerry_external_handler_t value, void *context) {
    jerry_value_t func = jerry_create_external_function(value);
    jerry_set_object_native_pointer(func, context, NULL);
    hinarin_set_release_key(parent, jerry_create_string((const jerry_char_t *) keyname), func);
    jerry_release_value(func);
}

void hinarin_set_name_null(jerry_value_t parent, const char *keyname) {
    hinarin_set_release(parent, jerry_create_string((const jerry_char_t *) keyname), jerry_create_null());
}

void hinarin_set_name_undefined(jerry_value_t parent, const char *keyname) {
    hinarin_set_release(parent, jerry_create_string((const jerry_char_t *) keyname), jerry_create_undefined());
}

void hinarin_unset_release(jerry_value_t parent, jerry_value_t releasekey) {
    jerry_delete_property(parent, releasekey);
    jerry_release_value(releasekey);
}

void hinarin_unset_name(jerry_value_t parent, const char *keyname) {
    hinarin_unset_release(parent, jerry_create_string((const jerry_char_t *) keyname));
}

void hinarin_set_release(jerry_value_t parent, jerry_value_t releasekey, jerry_value_t releasevalue) {
    jerry_release_value(jerry_set_property(parent, releasekey, releasevalue));
    jerry_release_value(releasekey);
    jerry_release_value(releasevalue);
}

void hinarin_set_release_key(jerry_value_t parent, jerry_value_t releasekey, jerry_value_t value) {
    jerry_release_value(jerry_set_property(parent, releasekey, value));
    jerry_release_value(releasekey);
}

void hinarin_set_release_value(jerry_value_t parent, jerry_value_t key, jerry_value_t releasevalue) {
    jerry_release_value(jerry_set_property(parent, key, releasevalue));
    jerry_release_value(releasevalue);
}

void hinarin_set_name_release(jerry_value_t parent, const char *keyname, jerry_value_t releasevalue) {
    hinarin_set_release(parent, jerry_create_string((const jerry_char_t *) keyname), releasevalue);
}

size_t hinarin_string_size_release(jerry_value_t releasevalue) {
    size_t size = jerry_get_string_size(releasevalue);
    jerry_release_value(releasevalue);
    return size;
}

size_t hinarin_string_to_buffer_release(jerry_value_t releasevalue, char *buf, size_t buflen) {
    size_t size = jerry_string_to_char_buffer(releasevalue, (jerry_char_t *) buf, (jerry_size_t) buflen);
    jerry_release_value(releasevalue);
    return size;
}

jerry_value_t hinarin_get_release(jerry_value_t parent, jerry_value_t releasekey) {
    jerry_value_t value = jerry_get_property(parent, releasekey);
    jerry_release_value(releasekey);
    return value;
}

bool hinarin_get_release_bool(jerry_value_t parent, jerry_value_t releasekey) {
    jerry_value_t value = jerry_get_property(parent, releasekey);
    jerry_release_value(releasekey);
    bool result = jerry_get_boolean_value(value);
    jerry_release_value(value);
    return result;
}

double hinarin_get_release_number(jerry_value_t parent, jerry_value_t releasekey) {
    jerry_value_t value = jerry_get_property(parent, releasekey);
    jerry_release_value(releasekey);
    double result = jerry_get_number_value(value);
    jerry_release_value(value);
    return result;
}

jerry_value_t hinarin_get_index_release(jerry_value_t releasevalue, uint32_t index) {
    jerry_value_t value = jerry_get_property_by_index(releasevalue, index);
    jerry_release_value(releasevalue);
    return value;
}

bool hinarin_get_index_bool(jerry_value_t parent, uint32_t index) {
    jerry_value_t value = jerry_get_property_by_index(parent, index);
    bool result = jerry_get_boolean_value(value);
    jerry_release_value(value);
    return result;
}

double hinarin_get_index_number(jerry_value_t parent, uint32_t index) {
    jerry_value_t value = jerry_get_property_by_index(parent, index);
    double result = jerry_get_number_value(value);
    jerry_release_value(value);
    return result;
}

jerry_value_t hinarin_get_name(jerry_value_t parent, const char* keyname) {
    jerry_value_t value = hinarin_get_release(parent, jerry_create_string((const jerry_char_t *) keyname));
    return value;
}

bool hinarin_get_name_bool(jerry_value_t parent, const char *keyname) {
    jerry_value_t value = hinarin_get_release(parent, jerry_create_string((const jerry_char_t *) keyname));
    bool result = jerry_get_boolean_value(value);
    jerry_release_value(value);
    return result;
}

double hinarin_get_name_number(jerry_value_t parent, const char* keyname) {
    jerry_value_t value = hinarin_get_release(parent, jerry_create_string((const jerry_char_t *) keyname));
    double result = jerry_get_number_value(value);
    jerry_release_value(value);
    return result;
}


int hinarin_mkdir(const char *path) {
#ifdef _WIN32
    return _mkdir(path);
#else
    return mkdir(path, 0700);
#endif
}

int hinarin_mkdir_recursive(const char *path) {
    if (hinarin_mkdir_basedir_recursive(path)) {
        hinarin_mkdir(path);
    }
}

int hinarin_mkdir_basedir_recursive(const char *path) {
    int last = 0;
    for (char *c = (char *) path; *c != '\0'; c++) {
        if (last && *c == '/') {
            *c = '\0';
            hinarin_mkdir(path);
            *c = '/';
            last = 0;
        } else {
            last = 1;
        }
    }
    return last;
}

size_t hinarin_homedir(char *buf, size_t buflen) {
#ifdef _WIN32
    size_t len = (size_t) snprintf(buf, buflen, "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
    size_t len = (size_t) snprintf(buf, buflen, "%s", getenv("HOME"));
#endif
    for (int i = 0; i < len; i++) {
        if (buf[i] == '\\') {
            buf[i] = '/';
        }
    }
    return len;
}

typedef struct {
    void *curl;
    hinarin_writefunc writefunc;
    hinarin_progressfunc progressfunc;
    void *writefunc_data;
    void *progressfunc_data;
    ssize_t total;
    size_t processed;
} hinarin_download_t;

static size_t curl_writefunc(void *ptr, size_t size, size_t n, void *data) {
    size_t length = size * n;
    hinarin_download_t *download = data;
    if (download->processed == 0) {
        curl_easy_getinfo(download->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &download->total);
    }

    if (download->progressfunc) {
        download->progressfunc(download->processed, download->total, download->progressfunc_data);
    }

    size_t iterprocessed = 0;
    if (download->writefunc) {
        iterprocessed = download->writefunc(ptr, length, download->writefunc_data);
    } else {
        iterprocessed = length;
    }

    download->processed += iterprocessed;
    return iterprocessed;
}

static size_t curl_headerfunc(char *ptr, size_t size, size_t n, void *data) {
    size_t length = size * n;
    jerry_value_t *headers = data;

    int keylen = 0;
    int seplen = 0;
    int valuelen = 0;

    for (size_t i = 0; i < length; i++) {
        if (seplen == 0) {
            if (ptr[i] == ':') {
                seplen = 1;
            } else {
                keylen++;
            }
        } else if (valuelen == 0) {
            if (ptr[i] != ' ') {
                valuelen++;
            } else {
                seplen++;
            }
        } else if (ptr[i] != '\r' && ptr[i] != '\n') {
            valuelen++;
        }
    }

    if (keylen > 0 && valuelen > 0) {
        jerry_value_t key = jerry_create_string_sz((const jerry_char_t *) ptr, (jerry_size_t) keylen);
        jerry_value_t value = jerry_create_string_sz((const jerry_char_t *) ptr+ keylen + seplen, (jerry_size_t) valuelen);
        hinarin_set_release(*headers, key, value);
    }

    return length;
}

static bool headers_foreach(const jerry_value_t property_name, const jerry_value_t property_value, void *data) {
    struct curl_slist **sendheaders_list = data;
    hinarin_string(property_name, key);
    hinarin_string(property_value, value);

    char buf[1024];
    snprintf(buf, sizeof(buf), "%s: %s", key, value);

    *sendheaders_list = curl_slist_append(*sendheaders_list, buf);
    return true;
}

static size_t hinarin_write_to_file(void *ptr, size_t length, void *data) {
    FILE *file = data;
    return fwrite(ptr, length, 1, file) * length;
}

jerry_value_t hinarin_download(const char *url, jerry_value_t *sendheaders, jerry_value_t *sendcookies, hinarin_writefunc writefunc, void *writefunc_data, hinarin_progressfunc progressfunc, void *progressfunc_data) {
    hinarin_download_t download;
    download.total = 0;
    download.processed = 0;
    download.writefunc = writefunc;
    download.writefunc_data = writefunc_data;
    download.progressfunc = progressfunc;
    download.progressfunc_data = progressfunc_data;
    download.curl = curl_easy_init();
    jerry_value_t result = jerry_create_object();
    jerry_value_t headers = jerry_create_object();
    jerry_value_t cookies = jerry_create_object();

    struct curl_slist *sendheaders_list = NULL;
    if (sendheaders) {
        jerry_foreach_object_property(*sendheaders, headers_foreach, &sendheaders_list);
        curl_easy_setopt(download.curl, CURLOPT_HTTPHEADER, sendheaders_list);
    }

    printf("Downloading %s", url);
    if (strlen(url) > 7 && strncmp("file://", url, 7) == 0) {
        char homepath[1024];
        hinarin_homedir(homepath, sizeof(homepath));
        char urlpath[1024];
#ifdef _WIN32
        snprintf(urlpath, sizeof(urlpath), "file://localhost/%s/.hinarin/modules/%s", homepath, url+7);
#else
        snprintf(urlpath, sizeof(urlpath), "file://localhost%s/.hinarin/modules/%s", homepath, url+7);
#endif
        curl_easy_setopt(download.curl, CURLOPT_URL, urlpath);
        printf(" (-> %s)", urlpath);
    } else {
        curl_easy_setopt(download.curl, CURLOPT_URL, url);
    }
    printf(" ... ");
    curl_easy_setopt(download.curl, CURLOPT_WRITEFUNCTION, curl_writefunc);
    curl_easy_setopt(download.curl, CURLOPT_WRITEDATA, &download);
    curl_easy_setopt(download.curl, CURLOPT_HEADERFUNCTION, curl_headerfunc);
    curl_easy_setopt(download.curl, CURLOPT_HEADERDATA, &headers);
    CURLcode err = curl_easy_perform(download.curl);
    long code;
    curl_easy_getinfo(download.curl, CURLINFO_RESPONSE_CODE, &code);
    curl_easy_getinfo(download.curl, CURLINFO_RESPONSE_CODE, &code);
    struct curl_slist *cookielist, *nc;
    curl_easy_getinfo(download.curl, CURLINFO_COOKIELIST, &cookielist);
    nc = cookielist;

    while (nc) {
        printf("!!! %s\n", nc->data);
        nc = nc->next;
    }

    if (download.progressfunc) {
        download.progressfunc(download.processed, download.total, download.progressfunc_data);
    }

    curl_easy_cleanup(download.curl);
    curl_slist_free_all(sendheaders_list);
    curl_slist_free_all(cookielist);

    hinarin_set_name_release(result, "headers", headers);
    hinarin_set_name_release(result, "cookies", cookies);
    hinarin_set_name_number(result, "error", err);
    hinarin_set_name_number(result, "code", code);
    printf("%ld processed %ld bytes, error #%d\n", code, download.processed, err);

    hinarin_set_name_number(result, "processed", download.processed);
    return result;
}

jerry_value_t hinarin_download_to_file(const char *url, const char *filepath, jerry_value_t *sendheaders, jerry_value_t *sendcookies, hinarin_progressfunc progressfunc, void *progressfunc_data) {
    hinarin_mkdir_basedir_recursive(filepath);
    FILE *file = fopen(filepath, "a");
    long position = (size_t) ftell(file);
    bool cleanup = false;
    if (position > 0) {
        if (!sendheaders) {
            jerry_value_t resumeheader = jerry_create_object();
            sendheaders = &resumeheader;
            cleanup = true;
        }
        char bufbytes[1024];
        snprintf(bufbytes, sizeof(bufbytes), "bytes=%ld-", position);
        hinarin_set_name_string(*sendheaders, "Range", bufbytes);
    }
    jerry_value_t value = hinarin_download(url, sendheaders, sendcookies, hinarin_write_to_file, file, progressfunc, progressfunc_data);
    fclose(file);
    if (cleanup) {
        jerry_release_value(*sendheaders);
    }
    return value;
}

typedef struct {
    char *buf;
    size_t len;
} string_t;

static size_t hinarin_write_to_string(void *ptr, size_t length, void *data) {
    string_t *str = data;
    str->len += length;
    str->buf = realloc(str->buf, str->len+1);
    memmove(str->buf+str->len-length, ptr, length);
    str->buf[str->len] = '\0';
    return length;
}

jerry_value_t hinarin_download_to_data(const char *url, jerry_value_t *sendheaders, jerry_value_t *sendcookies, hinarin_progressfunc progressfunc, void *progressfunc_data) {
    string_t str;
    str.buf = 0;
    str.len = 0;
    jerry_value_t value = hinarin_download(url, sendheaders, sendcookies, hinarin_write_to_string, &str, progressfunc, progressfunc_data);
    if (str.buf) {
        jerry_value_t string = jerry_create_string((const jerry_char_t *) str.buf);
        hinarin_set_name_release(value, "data", string);
        free(str.buf);
    } else {
        hinarin_set_name_release(value, "data", jerry_create_string((const jerry_char_t *) ""));
    }

    return value;
}