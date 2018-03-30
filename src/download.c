//
// Created by user on 3/23/18.
//

#include "download.h"
#include "string.h"
#include "file.h"
#include "linkedmap.h"

#define CURL_STATICLIB
#include <curl/curl.h>

#include <ctype.h>
#include <stdio.h>

hinarin_download_request *hinarin_download_request_new(const char *url, hinarin_progress progress, void *progress_data) {
    hinarin_download_request *request = malloc(sizeof(hinarin_download_request));
    request->url = hinarin_string_new();
    hinarin_string_append(request->url, url);
    request->headers = hinarin_linkedmap_new();
    request->cookies = hinarin_linkedmap_new();
    request->progress = progress;
    request->progress_data = progress_data;
    return request;
}

void hinarin_download_request_header(hinarin_download_request *request, const char* key, const char *value) {
    size_t length = strlen(key);
    char *keylow = malloc(length+1);
    memmove(keylow, key, length);
    keylow[length] = '\0';
    for (char *k = keylow; *k != '\0'; k++) {
        *k = (char) tolower(*k);
    }
    hinarin_linkedmap_put(request->headers, keylow, value);
    free(keylow);
}

void hinarin_download_request_cookie(hinarin_download_request *request, const char* key, const char *value) {
    hinarin_linkedmap_put(request->cookies, key, value);
}

static void hinarin_download_set_range(hinarin_download_request *request, size_t length) {
    if (hinarin_linkedmap_get(request->headers, "range") == NULL) {
        hinarin_string *range = hinarin_string_new();
        hinarin_string_sprintf(range, "bytes=%ld-", length);
        hinarin_linkedmap_put(request->headers, "range", range->data);
        hinarin_string_free(range);
    }
}

void hinarin_download_ensure_url(hinarin_string *string) {
    if (hinarin_string_starts(string, "file://")) {
        hinarin_string *path = hinarin_string_new();
        hinarin_homedir(path);
        hinarin_string_append(path, "/.hinarin/modules/");
        hinarin_string_insert(string, 7, path->data);
        if (path->data[0] == '/') {
            hinarin_string_insert(string, 7, "localhost");
        } else {
            hinarin_string_insert(string, 7, "localhost/");
        }
        hinarin_string_free(path);
    }
}

typedef struct {
    void *curl;
    void *data;
    hinarin_download_result *result;
    hinarin_download_request *request;
} hinarin_download_curl;

static size_t hinarin_download_null_writer(char *ptr, size_t size, size_t n, void *user) {
    return size * n;
}

static size_t hinarin_download_string_writer(char *ptr, size_t size, size_t n, void *user) {
    size_t length = size * n;
    hinarin_download_curl *ctx = user;

    if (ctx->result->transferred == 0) {
        curl_easy_getinfo(ctx->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &ctx->result->total);
    }

    if (ctx->request->progress) {
        ctx->request->progress(ctx->result->transferred, ctx->result->total, ctx->request->progress_data);
    }

    hinarin_string_append_n(ctx->data, ptr, length);
    ctx->result->transferred += length;

    return length;
}

static size_t hinarin_download_file_writer(char *ptr, size_t size, size_t n, void *user) {
    size_t length = size * n;
    hinarin_download_curl *ctx = user;

    if (ctx->result->transferred == 0) {
        curl_easy_getinfo(ctx->curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &ctx->result->total);
    }

    if (ctx->request->progress) {
        ctx->request->progress(ctx->result->transferred, ctx->result->total, ctx->request->progress_data);
    }

    fwrite(ptr, size, n, ctx->data);
    ctx->result->transferred += length;

    return length;
}

static size_t hinarin_download_header_function(char *ptr, size_t size, size_t n, void *user) {
    size_t length = size * n;
    hinarin_download_curl *ctx = user;

    size_t keylen = 0;
    size_t seplen = 0;
    size_t valuelen = 0;
    size_t off = 0;

    for (size_t i = 0; i < length; i++) {
        if (seplen == 0) {
            if (ptr[i] == ':') {
                seplen = 1;
            } else {
                keylen++;
            }
            off++;
        } else if (valuelen == 0) {
            if (ptr[i] != ' ') {
                valuelen++;
            } else {
                seplen++;
            }
            off++;
        } else if (ptr[i] != '\r' && ptr[i] != '\n') {
            valuelen++;
            off++;
        } else {
            keylen = 0;
            seplen = 0;
            valuelen = 0;
        }
    }

    if (keylen > 0 && valuelen > 0) {
        printf("%ld %ld\n", keylen, valuelen);
        hinarin_linkedmap_put_n(ctx->result->headers, ptr+off, keylen, ptr+off+keylen+seplen, valuelen);
    }

    return length;
}

static hinarin_download_result *hinarin_download_process(hinarin_download_request *request, curl_write_callback cb, void *data) {
    hinarin_download_curl ctx;
    ctx.curl = curl_easy_init();
    ctx.data = data;
    ctx.result = malloc(sizeof(hinarin_download_result));
    ctx.result->headers = hinarin_linkedmap_new();
    ctx.result->cookies = hinarin_linkedmap_new();
    ctx.result->transferred = 0;
    ctx.result->total = 0;
    ctx.request = request;

    hinarin_download_ensure_url(request->url);

    struct curl_slist *sendheaders_list = NULL;
    if (request->headers->head != NULL) {
        hinarin_linkedmap_bucket *ptr = request->headers->head;
        while (ptr != NULL) {
            hinarin_string *entry = hinarin_string_new();
            char *c;
            for (c = ptr->key; *c != '\0'; c++) {
                if (c - ptr->key == 0 || *(c-1) == '-') {
                    char ch = (char) toupper(*c);
                    hinarin_string_append_n(entry, &ch, 1);
                } else {
                    hinarin_string_append_n(entry, c, 1);
                }
            }
            hinarin_string_append(entry, ": ");
            hinarin_string_append(entry, ptr->value);
            sendheaders_list = curl_slist_append(sendheaders_list, entry->data);
            hinarin_string_free(entry);
            ptr = ptr->next;
        }

        curl_easy_setopt(ctx.curl, CURLOPT_HTTPHEADER, sendheaders_list);
    }

    if (request->cookies->head != NULL) {
        hinarin_linkedmap_bucket *ptr = request->cookies->head;
        hinarin_string *entry = hinarin_string_new();
        while (ptr != NULL) {
            if (ptr != request->cookies->head) {
                hinarin_string_append(entry, "; ");
            }
            hinarin_string_append(entry, ptr->key);
            hinarin_string_append(entry, "=");
            hinarin_string_append(entry, ptr->value);
            ptr = ptr->next;
        }

        curl_easy_setopt(ctx.curl, CURLOPT_COOKIE, entry->data);
        hinarin_string_free(entry);
    }

    curl_easy_setopt(ctx.curl, CURLOPT_URL, request->url->data);
    if (cb) {
        curl_easy_setopt(ctx.curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(ctx.curl, CURLOPT_WRITEDATA, &ctx);
    } else {
        curl_easy_setopt(ctx.curl, CURLOPT_WRITEFUNCTION, hinarin_download_null_writer);
        curl_easy_setopt(ctx.curl, CURLOPT_NOBODY, 1);
    }
    curl_easy_setopt(ctx.curl, CURLOPT_HEADERFUNCTION, hinarin_download_header_function);
    curl_easy_setopt(ctx.curl, CURLOPT_HEADERDATA, &ctx);
    curl_easy_setopt(ctx.curl, CURLOPT_FOLLOWLOCATION, 1);
    
    ctx.result->error = curl_easy_perform(ctx.curl);
    curl_easy_getinfo(ctx.curl, CURLINFO_RESPONSE_CODE, &ctx.result->code);

    struct curl_slist *cookielist, *nc;
    curl_easy_getinfo(ctx.curl, CURLINFO_COOKIELIST, &cookielist);
    nc = cookielist;
    while (nc) {
        printf("!!! %s\n", nc->data);
        nc = nc->next;
    }
    curl_slist_free_all(cookielist);


    if (sendheaders_list != NULL) {
        curl_slist_free_all(sendheaders_list);
    }

    if (request->progress) {
        request->progress(ctx.result->transferred, ctx.result->total, request->progress_data);
    }

    curl_easy_cleanup(ctx.curl);
    hinarin_download_free_request(request);
    return ctx.result;
}

hinarin_download_result *hinarin_download_to_string(hinarin_download_request *request, hinarin_string *string) {
    if (string->length > 0) {
        hinarin_download_set_range(request, string->length);
    }
    return hinarin_download_process(request, hinarin_download_string_writer, string);
}

hinarin_download_result *hinarin_download_to_file(hinarin_download_request *request, const char* path) {
    hinarin_mkdir_basedir_recursive(path);
    FILE *f = fopen(path, "a");
    size_t length = (size_t) ftell(f);
    if (length > 0) {
        hinarin_download_set_range(request, length);
    }
    hinarin_download_result *result = hinarin_download_process(request, hinarin_download_file_writer, f);
    fclose(f);
    return result;
}

hinarin_download_result *hinarin_download_to_exists(hinarin_download_request *request, bool *exists) {
    bool http = hinarin_string_starts(request->url, "http");
    hinarin_download_result *result = hinarin_download_process(request, NULL, NULL);
    if (http) {
        *exists = result->code / 100 == 2;
    } else {
        *exists = result->error == 0;
    }
    return result;
}

void hinarin_download_free_result(hinarin_download_result *result) {
    hinarin_linkedmap_free(result->headers);
    hinarin_linkedmap_free(result->cookies);
    free(result);
}

void hinarin_download_free_request(hinarin_download_request *request) {
    hinarin_string_free(request->url);
    hinarin_linkedmap_free(request->cookies);
    hinarin_linkedmap_free(request->headers);
    free(request);
}