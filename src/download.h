//
// Created by user on 3/23/18.
//

#ifndef HINARIN_DOWNLOAD_H
#define HINARIN_DOWNLOAD_H

#include <stdlib.h>

#include "string.h"
#include "linkedmap.h"

typedef struct {
    long error;
    long code;
    size_t transferred;
    ssize_t total;
    hinarin_linkedmap *headers;
    hinarin_linkedmap *cookies;
} hinarin_download_result;

typedef void (*hinarin_progress)(size_t transferred, ssize_t total, void *data);

typedef struct {
    hinarin_string *url;
    hinarin_linkedmap *headers;
    hinarin_linkedmap *cookies;
    hinarin_progress progress;
    void *progress_data;
} hinarin_download_request;

void hinarin_download_ensure_url(hinarin_string *string);
hinarin_download_request *hinarin_download_request_new(const char *url, hinarin_progress progress, void *progress_data);
void hinarin_download_request_header(hinarin_download_request *request, const char* key, const char *value);
void hinarin_download_request_cookie(hinarin_download_request *request, const char* key, const char *value);
hinarin_download_result *hinarin_download_to_string(hinarin_download_request *request, hinarin_string *string);
hinarin_download_result *hinarin_download_to_file(hinarin_download_request *request, const char* path);
hinarin_download_result *hinarin_download_to_exists(hinarin_download_request *request, bool *exists);
void hinarin_download_free_result(hinarin_download_result *result);
void hinarin_download_free_request(hinarin_download_request *request);


#endif //HINARIN_DOWNLOAD_H
