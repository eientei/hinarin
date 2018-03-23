#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string.h>

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

#include <xsAll.h>
#include <xs.h>

void hinarin_error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

void hinarin_init(xsMachine *the) {
    glfwSetErrorCallback(hinarin_error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to setup GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(900, 600, "Hinarin", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    xsSlot window_obj = xsNewHostObject(NULL);
    xsSetHostData(window_obj, window);
    xsSet(xsGlobal, xsID("window"), window_obj);
}

void hinarin_loop(xsMachine *the) {
    struct timeval te;
    gettimeofday(&te, NULL);
    time_t start = (te.tv_sec * 1000 + te.tv_usec / 1000);
    uint32_t frame = 0;

    xsSlot window_obj = xsGet(xsGlobal, xsID("window"));
    
    GLFWwindow *window = xsGetHostData(window_obj);

    struct nk_context *ctx = nk_glfw3_init(window, NK_GLFW3_INSTALL_CALLBACKS);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 16, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(ctx, &font->handle);

    int width, height, xpos, ypos;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        nk_glfw3_new_frame();

        glfwGetWindowPos(window, &xpos, &ypos);
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1.0);

        gettimeofday(&te, NULL);

        //hinarin_set_name_number(window, "xpos", xpos);
        //hinarin_set_name_number(window, "ypos", ypos);
        //hinarin_set_name_number(window, "width", width);
        //hinarin_set_name_number(window, "height", height);
        //hinarin_set_name_number(window, "frame", frame++);
        //hinarin_set_name_number(window, "milliseconds", (te.tv_sec * 1000 + te.tv_usec / 1000) - start);

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        glfwSwapBuffers(window);
        frame++;
    }
}

void hinarin_deinit(xsMachine *the) {
    nk_glfw3_shutdown();
    glfwTerminate();
}

void hinarin_incontext(xsMachine *the) {
    hinarin_init(the);
    hinarin_loop(the);
    hinarin_deinit(the);
}

int main(int argc, char **argv) {
    	xsCreation _creation = {
		128 * 1024 * 1024, /* initialChunkSize */
		16 * 1024 * 1024,  /* incrementalChunkSize */
		8 * 1024 * 1024,   /* initialHeapCount */
		1 * 1024 * 1024,   /* incrementalHeapCount */
		4096,              /* stackCount */
		4096*3,            /* keyCount */
		1993,              /* nameModulo */
		127                /* symbolModulo */
	};
    xsCreation* creation = &_creation;
    fxInitializeSharedCluster();
    xsMachine* machine = xsCreateMachine(creation, "hinarin", NULL);
    xsBeginHost(machine);
            xsTry { hinarin_incontext(the); } xsCatch { printf("GLOBAL ERROR: %s\n", xsToString(xsException)); }
    xsEndHost(the);
    xsDeleteMachine(machine);
    fxTerminateSharedCluster();
    return 0;
}