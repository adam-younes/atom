#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <time.h>
#include <float.h>

#include <wayland-client.h>
#include <wayland-egl.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <math/la.h>
#include <math/trig.h>
#include <window/window.h>
#include <window/xdg-shell-client-protocol.h>
#include <mesh/mesh.h>
#include <opengl/opengl.h>


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
    egl_context = eglCreateContext( egl_display, cfg,
        EGL_NO_CONTEXT,
        ctx_attribs
    );
    // create window surface
    egl_surface = eglCreateWindowSurface(
        egl_display, cfg, (EGLNativeWindowType)egl_window, NULL);

    eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

    init_glad();

    mesh teapot = { 0 };
    load_mesh("../test/models/obj/teapot.obj", &teapot);
    generate_normals(&teapot);

    fprintf(stderr, "Loaded mesh: %zu vertices, %zu indices\n",
            *teapot.vert_count, *teapot.idx_count);

    // bounding box initialization
    vec3 bb_min = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
    vec3 bb_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (size_t i = 0; i < *teapot.vert_count; i++) {
        vec3 p = {
            teapot.positions[3*i + 0],
            teapot.positions[3*i + 1],
            teapot.positions[3*i + 2]
        };
        bb_min = vec3_min(bb_min, p);
        bb_max = vec3_max(bb_max, p);
    }
    fprintf(stderr, "<%f, %f, %f>, <%f, %f, %f>",
            bb_min.x, 
            bb_min.y, 
            bb_min.z, 
            bb_max.x, 
            bb_max.y, 
            bb_max.z);

    vec3 center = vec_scale(vec_sum(bb_min, bb_max), 0.5f);

    vec3 diag = vec_sum(bb_max, vec_negate(bb_min));

    float radius = 0.5f * vec_length(diag);

    float fov   = to_radians(45.0f);
    // Add 1.5x padding factor to ensure entire object is visible
    float cam_d = (radius * 1.5f) / tanf(fov * 0.5f);

    // Position camera to look at the centered teapot
    mat4 view = look_at(
        (vec3){ 0.0f, 0.0f,  cam_d },   // camera position (back from origin)
        (vec3){ 0.0f, 0.0f,  0.0f },     // looking at origin
        (vec3){ 0.0f, 1.0f,  0.0f }      // up vector
    );
    mat4 proj = perspective_mat4(
        fov,
        (float)width / (float)height,
        0.1f,
        cam_d + radius * 2.0f        // far plane beyond camera + object
    );

    // prepare gl state
    GLuint program = make_program();
    glUseProgram(program);

    GLint model_loc   = glGetUniformLocation(program, "uModel");
    GLint view_loc    = glGetUniformLocation(program, "uView");
    GLint proj_loc    = glGetUniformLocation(program, "uProj");
    GLint normal_loc  = glGetUniformLocation(program, "uNormalMat");
    GLint light_pos_loc   = glGetUniformLocation(program, "uLightPos");
    GLint view_pos_loc    = glGetUniformLocation(program, "uViewPos");
    GLint light_color_loc = glGetUniformLocation(program, "uLightColor");
    GLint object_color_loc = glGetUniformLocation(program, "uObjectColor");

    GLuint vao, vbo_pos, vbo_norm, ebo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER,
                 3 * (*teapot.vert_count) * sizeof(float),
                 teapot.positions,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
    glBufferData(GL_ARRAY_BUFFER,
                 3 * (*teapot.vert_count) * sizeof(float),
                 teapot.normals,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (*teapot.idx_count) * sizeof(uint32_t),
                 teapot.indices,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

    // enable depth testing for proper 3D rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    vec3 light_pos    = { -5.0f, 5.0f, 5.0f };
    vec3 camera_pos   = { 0.0f, 0.0f, cam_d };
    vec3 light_color  = { 3.0f, 3.0f, 3.0f };
    vec3 object_color = { 1.0f, 0.75f, 0.2f };

    glUseProgram(program);
    glUniform3fv(light_pos_loc, 1, &light_pos.x);
    glUniform3fv(view_pos_loc, 1, &camera_pos.x);
    glUniform3fv(light_color_loc, 1, &light_color.x);
    glUniform3fv(object_color_loc, 1, &object_color.x);

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

        mat4 model = mat4_identity();
        model = translate_mat4(model, vec_negate(center));
        model = rotate_mat4(model, angle, (vec3){ 0.0f, 1.0f, 0.0f });

        mat4 normal_mat = mat4_transpose(mat4_inverse(model));

        // render
        glViewport(0, 0, width, height);
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(model_loc, 1, GL_TRUE, &model.m[0][0]);
        glUniformMatrix4fv(view_loc, 1, GL_TRUE, &view.m[0][0]);
        glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &proj.m[0][0]);
        glUniformMatrix4fv(normal_loc, 1, GL_TRUE, &normal_mat.m[0][0]);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, (GLsizei) *teapot.idx_count, GL_UNSIGNED_INT, 0);

        eglSwapBuffers(egl_display, egl_surface);
        wl_display_flush(wl_display);
    }

    destroy_mesh(&teapot);

    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo_norm);
    glDeleteBuffers(1, &vbo_pos);
    glDeleteVertexArrays(1, &vao);

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

