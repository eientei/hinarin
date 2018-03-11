#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <jerryscript.h>

#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include <nuklear.h>
#include <nuklear_glfw_gl3.h>

#define CURL_STATICLIB
#include <curl/curl.h>
#include <sys/stat.h>

#include "nuklear_binding.h"
#include "system_binding.h"
#include "util.h"
#include "modules_binding.h"

static void error_callback(int e, const char *d)  {
    printf("Error %d: %s\n", e, d);
}

/*
static bool deleter(const jerry_value_t property_name, const jerry_value_t property_value, void *user_data_p) {
    jerry_value_t *modules = user_data_p;
    js_define_string(property_name, text);
    jerry_value_t str = jerry_value_to_string(property_value);
    js_define_string(str, value);
    printf("deleting %s = %s\n", text, value);
    jerry_release_value(str);
    jerry_delete_property(*modules, property_name);
    return true;
}

function(fullreset) {
    printf("Full reset issued!\n");
    jerry_value_t *modules;
    jerry_get_object_native_pointer(function_obj, (void **) &modules, NULL);
    jerry_foreach_object_property(*modules, deleter, modules);

    nuklear_reset();

    //modules_load(*modules);

    return jerry_create_undefined();
}
*/

static size_t writer_to_length(void *ptr, size_t size, size_t nmemb, int *len) {
    *len += size*nmemb;
    return size*nmemb;
}

static size_t writer_to_file(void *ptr, size_t size, size_t nmemb, FILE *f) {
    return fwrite(ptr, size, nmemb, f) * size;
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

    char initpath[1024];
    int homepath_len = get_homepath(initpath);
    memmove(initpath+homepath_len, "init.js", 8);
    void *curl = curl_easy_init();
    int len = 0;
    curl_easy_setopt(curl, CURLOPT_URL, initpath);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer_to_length);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &len);
    curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (len == 0) {
        curl = curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/eientei/hinarin/master/base/init.js");
        size_t init_len = strlen(initpath);
        char dirname[init_len-24+1];
        memmove(dirname, initpath+17, init_len-24);
        dirname[init_len-24] = '\0';

#if defined(_WIN32)
        _mkdir(dirname);
#else
        mkdir(dirname, 0700);
#endif
        FILE *f = fopen(initpath+17, "w+");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer_to_file);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, f);
        CURLcode err = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(f);

        if (err != 0) {
            printf("Could not fetch initialization script, error %d\n", err);
        }
    }


    jerry_value_t global = jerry_get_global_object();
    jerry_value_t modules = js_prop(global, "modules");
    jerry_value_t add = js_prop(modules, "add");
    jerry_value_t null = jerry_create_null();
    jerry_value_t str = jerry_create_string((const jerry_char_t *) "file://init.js");
    jerry_value_t ret = jerry_call_function(add, null, &str, 1);
    jerry_release_value(ret);
    jerry_release_value(str);
    jerry_release_value(null);
    jerry_release_value(add);
    jerry_release_value(modules);
    jerry_release_value(global);

    //bind_gumbo();
    //bind_curl();

    /*
    modules_init();
    js_register_function(global, "print", print, NULL);
    jerry_value_t modules_obj = jerry_create_object();
    js_register_prop_name(global, "modules", modules_obj);

    jerry_value_t system_obj = jerry_create_object();
    js_register_function(system_obj, "fullreset", fullreset, &modules_obj);
    js_register_function(system_obj, "addsource", addsource, NULL);
    js_register_function(system_obj, "removesource", addsource, NULL);
    js_register_prop_name(global, "system", system_obj);
    jerry_release_value(system_obj);
    */

    /*
    char *script = "system.addsource('https://home.eientei.org/static/core.js'); system.fullreset()";
    jerry_value_t value = jerry_eval((const jerry_char_t *) script, strlen(script), true);
    if (jerry_value_has_error_flag(value)) {
        jerry_value_clear_error_flag(&value);
        js_print(value);
        jerry_release_value(value);
    } else {
        jerry_release_value(value);
    }
    */


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

        //nuklear_draw();

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        glfwSwapBuffers(window);
    }
    jerry_release_value(window_obj);

    nk_glfw3_shutdown();
    glfwTerminate();
    jerry_cleanup();
    return 0;
}