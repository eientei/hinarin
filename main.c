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
    char homepath[1024];
    hinarin_homedir(homepath, sizeof(homepath));
    char initpath[1024];
    snprintf(initpath, sizeof(initpath), "%s/.hinarin/modules/init.js", homepath);
    jerry_value_t value = hinarin_download("file://init.js", NULL, NULL, NULL, NULL, NULL, NULL);
    double length = hinarin_get_name_number(value, "processed");
    jerry_release_value(value);
    if (length == 0) {
        jerry_release_value(hinarin_download_to_file("https://raw.githubusercontent.com/eientei/hinarin/master/base/init.js", initpath, NULL, NULL, NULL, NULL));
    }

    hinarin_load_module((const char *) "file://init.js");
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
    GLFWwindow *glfwWindow = glfwCreateWindow(900, 600, "Hinarin", NULL, NULL);
    glfwMakeContextCurrent(glfwWindow);
    glfwGetWindowSize(glfwWindow, &width, &height);
    glViewport(0, 0, width, height);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    jerry_init(JERRY_INIT_EMPTY);

    hinarin_system_bind();
    hinarin_modules_bind();
    jerry_value_t window = hinarin_nuklear_binding(glfwWindow);

    pthread_t init_thread;
    pthread_create(&init_thread, NULL, init_start, NULL);

    struct timeval te;
    gettimeofday(&te, NULL);
    time_t start = (te.tv_sec * 1000 + te.tv_usec / 1000);
    uint32_t frame = 0;

    jerry_value_t global = jerry_get_global_object();
    jerry_value_t modules = hinarin_get_name(global, "modules");

    modules_t *mods = NULL;
    jerry_get_object_native_pointer(modules, (void **) &mods, NULL);
    
    while (!glfwWindowShouldClose(glfwWindow)) {
        glfwPollEvents();
        nk_glfw3_new_frame();

        glfwGetWindowPos(glfwWindow, &xpos, &ypos);
        glfwGetWindowSize(glfwWindow, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1.0);

        gettimeofday(&te, NULL);

        hinarin_set_name_number(window, "xpos", xpos);
        hinarin_set_name_number(window, "ypos", ypos);
        hinarin_set_name_number(window, "width", width);
        hinarin_set_name_number(window, "height", height);
        hinarin_set_name_number(window, "frame", frame++);
        hinarin_set_name_number(window, "milliseconds", (te.tv_sec * 1000 + te.tv_usec / 1000) - start);
        
        for (uint32_t i = 0; i < jerry_get_array_length(mods->modules_list); i++) {
            jerry_value_t module = jerry_get_property_by_index(mods->modules_list, i);
            if (hinarin_has_key(module, "render")) {
                jerry_value_t render = hinarin_get_name(module, "render");
                jerry_value_t null = jerry_create_null();
                jerry_value_t result = jerry_call_function(render, null, &window, 1);
                if (jerry_value_has_error_flag(result)) {
                    hinarin_print(result);
                }
                jerry_release_value(result);
                jerry_release_value(null);
                jerry_release_value(render);
            }
            jerry_release_value(module);
        }

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        glfwSwapBuffers(glfwWindow);
    }
    jerry_release_value(window);
    jerry_release_value(modules);
    jerry_release_value(global);

    pthread_join(init_thread, NULL);

    nk_glfw3_shutdown();
    glfwTerminate();
    jerry_cleanup();
    return 0;
}