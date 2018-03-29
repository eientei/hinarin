#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#include "hinarin.h"

#include <string.h>
#include <sys/time.h>

#include "bind_root.h"
#include "bind_nuklear.h"

#include "file.h"
#include "download.h"

void hinarin_error_callback(int e, const char *d) {
    printf("Error %d: %s\n", e, d);
}

void *hinarin_init_thread(void *data) {
    xsMachine *the = data;
    hinarin_string *path = hinarin_string_new();
    hinarin_homedir(path);
    hinarin_string_append(path, "/.hinarin/modules/init.js");
    FILE *f = fopen(path->data, "a");
    fseek(f, 0, SEEK_END);
    if (ftell(f) == 0) {
        hinarin_download_free_result(hinarin_download_to_file(hinarin_download_request_new("https://eientei.github.io/hinarin/init.js", NULL, NULL), path->data));
    }
    fclose(f);
    hinarin_string_free(path);
    xsTry { fxRunModule(the, "init"); } xsCatch { printf("%s\n", xsToString(xsException)); }
}

void hinarin_init(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    
    glfwSetErrorCallback(hinarin_error_callback);
    if (!glfwInit()) {
        fprintf(stderr, "Failed to setup GLFW\n");
        exit(1);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    hinarin->window = glfwCreateWindow(900, 600, "Hinarin", NULL, NULL);
    glfwMakeContextCurrent(hinarin->window);
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to setup GLEW\n");
        exit(1);
    }

    hinarin->nuklear = nk_glfw3_init(hinarin->window, NK_GLFW3_INSTALL_CALLBACKS);

    struct nk_font_atlas *atlas;
    nk_glfw3_font_stash_begin(&atlas);
    struct nk_font *font = nk_font_atlas_add_default(atlas, 16, 0);
    nk_glfw3_font_stash_end();
    nk_style_set_font(hinarin->nuklear, &font->handle);

    hinarin_bind_root(the);
    hinarin_bind_nuklear(the);
    
    pthread_create(&hinarin->init_thread, NULL, hinarin_init_thread, the);
}

void hinarin_loop(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    
    struct timeval te;
    gettimeofday(&te, NULL);
    time_t start = (te.tv_sec * 1000 + te.tv_usec / 1000);
    uint32_t frame = 0;

    while (!glfwWindowShouldClose(hinarin->window)) {
        glfwPollEvents();
        nk_glfw3_new_frame();

        glfwGetWindowPos(hinarin->window, &hinarin->xpos, &hinarin->ypos);
        glfwGetWindowSize(hinarin->window, &hinarin->width, &hinarin->height);
        glViewport(0, 0, hinarin->width, hinarin->height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2, 0.2, 0.2, 1.0);

        gettimeofday(&te, NULL);

        hinarin->frame++;
        hinarin->milliseconds = (te.tv_sec * 1000 + te.tv_usec / 1000) - start;

        for (linked_slot *next = hinarin->renderers; next != NULL; next = next->next) {
            xsCallFunction0(xsGet(next->slot, xsID("render")), next->slot);
        }

        nk_glfw3_render(NK_ANTI_ALIASING_ON, 512 * 1024, 128 * 1024);
        glfwSwapBuffers(hinarin->window);
        frame++;
    }
}

void hinarin_deinit(xsMachine *the) {
    hinarin_context *hinarin = xsGetContext(the);
    linked_slot *tail = hinarin->renderers;
    while (tail != NULL) {
        linked_slot *next = tail->next;
        free(tail);
        tail = next;
    }

    pthread_join(hinarin->init_thread, NULL);
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
    hinarin_context hinarin = {0};
    xsMachine* machine = xsCreateMachine(creation, "hinarin", &hinarin);
    xsBeginHost(machine);
            xsTry { hinarin_incontext(the); } xsCatch { printf("GLOBAL ERROR: %s\n", xsToString(xsException)); }
    xsEndHost(the);
    xsDeleteMachine(machine);
    fxTerminateSharedCluster();
    return 0;
}