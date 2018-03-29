//
// Created by user on 3/28/18.
//

#ifndef HINARIN_HINARIN_H
#define HINARIN_HINARIN_H

#include <string.h>

#define GLEW_STATIC
#include <GL/glew.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <GLFW/glfw3.h>
#include <nuklear.h>
#include <nuklear_glfw_gl3.h>
#define PTW32_STATIC_LIB
#include <pthread.h>
#include <xs.h>

typedef struct {
    GLFWwindow *window;
    pthread_t init_thread;
    struct nk_context *nuklear;
    int width;
    int height;
    int xpos;
    int ypos;
    uint32_t frame;
    time_t milliseconds;
} hinarin_context;

#endif //HINARIN_HINARIN_H
