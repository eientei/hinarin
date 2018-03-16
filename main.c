#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <jerryscript.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

#define CURL_STATICLIB
#include <curl/curl.h>

#define PTW32_STATIC_LIB
#include <pthread.h>

#include "nuklear_binding.h"
#include "system_binding.h"
#include "util.h"
#include "modules_binding.h"

static void error_callback(int e, const char *d)  {
    printf("Error %d: %s\n", e, d);
}

static void *init_start(void *data) {
    char initpath[1024];
    size_t homepath_len = get_homepath(initpath, sizeof(initpath));
    memmove(initpath+homepath_len, "init.js", 8);
    void *curl = curl_easy_init();
    int len = 0;
    curl_easy_setopt(curl, CURLOPT_URL, initpath);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer_to_length);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &len);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (len == 0) {
        jerry_value_t response = download_to_file("https://raw.githubusercontent.com/eientei/hinarin/master/base/init.js", initpath+17, NULL);
        jerry_release_value(response);
    }
    load_module_url(initpath, homepath_len, (const char *) "file://init.js");
}

int main() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to setup GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int width, height, xpos, ypos;
    GLFWwindow *window = glfwCreateWindow(900, 600, "Hinarin", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwGetWindowSize(window, &width, &height);
    glViewport(0, 0, width, height);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    jerry_init(JERRY_INIT_EMPTY);

    bind_system();
    bind_modules();
    jerry_value_t window_obj = bind_nuklear(window);

    pthread_t init_thread;
    pthread_create(&init_thread, NULL, init_start, NULL);

    struct timeval te;
    gettimeofday(&te, NULL);
    time_t start = (te.tv_sec * 1000 + te.tv_usec / 1000);
    uint32_t frame = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        nk_glfw3_new_frame();

        glfwGetWindowPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1.0);

        gettimeofday(&te, NULL);
        js_register_double(window_obj, "xpos", xpos);
        js_register_double(window_obj, "ypos", ypos);
        js_register_double(window_obj, "width", width);
        js_register_double(window_obj, "height", height);
        js_register_double(window_obj, "frame", frame++);
        js_register_double(window_obj, "milliseconds", (te.tv_sec * 1000 + te.tv_usec / 1000) - start);

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        glfwSwapBuffers(window);
    }
    jerry_release_value(window_obj);

    pthread_join(init_thread, NULL);

    nk_glfw3_shutdown();
    glfwTerminate();
    jerry_cleanup();
    return 0;
}