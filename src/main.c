#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <time.h>
#include <math/la.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <window/window.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <opengl/opengl.h>
#include <window/xdg-shell-client-protocol.h>

int main(void) {
    // connect to wayland
    wl_display = wl_display_connect(NULL);
    if (!wl_display) {
        fprintf(stderr, "Failed to connect to Wayland\n");
        return 1;
    }

    // get the registry and bind globals
    wl_registry = wl_display_get_registry(wl_display);
    wl_registry_add_listener(wl_registry, &registry_listener, NULL);
    wl_display_roundtrip(wl_display);

    if (!wl_compositor || !xdg_wm_base) {
        fprintf(stderr, "Missing compositor or xdg_wm_base\n");
        return 1;
    }

    xdg_wm_base_add_listener(xdg_wm_base, &wm_base_listener, NULL);

    wl_surface   = wl_compositor_create_surface(wl_compositor);
    xdg_surface  = xdg_wm_base_get_xdg_surface(xdg_wm_base, wl_surface);
    xdg_toplevel = xdg_surface_get_toplevel(xdg_surface);

    xdg_toplevel_add_listener(xdg_toplevel,
                              &toplevel_listener, NULL);
    xdg_surface_add_listener(xdg_surface,
                             &xdg_surface_listener, NULL);
    wl_surface_commit(wl_surface);

    wl_display_roundtrip(wl_display);  // get initial configure

    // createa wl_egl_window of fixed size
    egl_window = wl_egl_window_create(wl_surface, width, height);

    eglBindAPI(EGL_OPENGL_API);

    // init egl
    egl_display = eglGetDisplay((EGLNativeDisplayType)wl_display);
    eglInitialize(egl_display, NULL, NULL);

    // choose a config
    const EGLint cfg_attribs[] = {
        EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
        EGL_RED_SIZE,        8,
        EGL_GREEN_SIZE,      8,
        EGL_BLUE_SIZE,       8,
        EGL_DEPTH_SIZE,     24,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };
    EGLConfig cfg;
    EGLint num_cfg;
    eglChooseConfig(egl_display, cfg_attribs, &cfg, 1, &num_cfg);

    // create context
    const EGLint ctx_attribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 5,
        EGL_CONTEXT_OPENGL_PROFILE_MASK,
        EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE
    };
    egl_context = eglCreateContext(
        egl_display,
        cfg,
        EGL_NO_CONTEXT,
        ctx_attribs
    );
    // create window surface
    egl_surface = eglCreateWindowSurface(
        egl_display, cfg, (EGLNativeWindowType)egl_window, NULL);

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    init_glad();

    // prepare gl state
    GLuint program = make_program();
    glUseProgram(program);

    GLuint vao; 
    glGenVertexArrays(1, &vao); 
    glBindVertexArray(vao);

    // triangle vertex data
    const GLfloat verts[] = {
        0.0f,  0.6f, 0.0f,
        -0.5f, -0.4f, 0.0f,
        0.5f, -0.4f, 0.0f
    };
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    GLint angle_loc = glGetUniformLocation(program, "uAngle");

    // enter mainloop 
    struct timespec last_t; clock_gettime(CLOCK_MONOTONIC, &last_t);
    float angle = 0.0f;

    while (wl_display_dispatch_pending(wl_display) != -1 && running) {
        // compute delta
        struct timespec now; clock_gettime(CLOCK_MONOTONIC, &now);
        float dt = (now.tv_sec - last_t.tv_sec)
            + (now.tv_nsec - last_t.tv_nsec) * 1e-9f;
        last_t = now;
        angle += dt;

        // render
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUniform1f(angle_loc, angle);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        eglSwapBuffers(egl_display, egl_surface);

        wl_display_flush(wl_display);
    }

    // cleanup
    eglDestroySurface(egl_display, egl_surface);
    eglDestroyContext(egl_display, egl_context);
    eglTerminate(egl_display);
    wl_egl_window_destroy(egl_window);
    xdg_toplevel_destroy(xdg_toplevel);
    xdg_surface_destroy(xdg_surface);
    wl_surface_destroy(wl_surface);
    xdg_wm_base_destroy(xdg_wm_base);
    wl_compositor_destroy(wl_compositor);
    wl_registry_destroy(wl_registry);
    wl_display_disconnect(wl_display);
    return 0;
}

