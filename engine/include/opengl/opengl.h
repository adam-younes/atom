#ifndef ATOM_GRAPHICS_H
#define ATOM_GRAPHICS_H 

#include <EGL/egl.h>
#include <stdlib.h> 
#include <stdio.h>
#include <opengl/glad.h>
#include <wayland-egl.h>

static struct wl_egl_window *egl_window    = NULL;

// EGL / GL ES globals
static EGLDisplay  egl_display  = EGL_NO_DISPLAY;
static EGLContext  egl_context  = EGL_NO_CONTEXT;
static EGLSurface  egl_surface  = EGL_NO_SURFACE;

// window size
static int width  = 1080;
static int height = 1080;

static char *load_shader_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Failed to open shader: %s\n", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = malloc(len + 1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

static void init_glad(void) {
    if (!gladLoadGL((GLADloadfunc) eglGetProcAddress)) {
        fprintf(stderr, "ERROR: failed to initialize GLAD\n");
        exit(1);
    }
}

// utility: compile a shader and check for errors
static GLuint compile_shader(GLenum type, const char *src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, NULL);
    glCompileShader(sh);

    GLint ok;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetShaderInfoLog(sh, 512, NULL, buf);
        fprintf(stderr, "Shader compile error: %s\n", buf);
        exit(1);
    }
    return sh;
}

static GLuint make_program_from_files(const char *vs_path, const char *fs_path) {
    char *vs_source = load_shader_file(vs_path);
    char *fs_source = load_shader_file(fs_path);
    if (!vs_source || !fs_source) {
        exit(1);
    }

    GLuint v = compile_shader(GL_VERTEX_SHADER, vs_source);
    GLuint f = compile_shader(GL_FRAGMENT_SHADER, fs_source);

    free(vs_source);
    free(fs_source);

    GLuint p = glCreateProgram();
    glAttachShader(p, v);
    glAttachShader(p, f);
    glBindAttribLocation(p, 0, "aPos");
    glLinkProgram(p);

    GLint ok;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char buf[512];
        glGetProgramInfoLog(p, 512, NULL, buf);
        fprintf(stderr, "Program link error: %s\n", buf);
        exit(1);
    }
    glDeleteShader(v);
    glDeleteShader(f);
    return p;
}

#endif
