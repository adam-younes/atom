#ifndef ATOM_GRAPHICS_H
#define ATOM_GRAPHICS_H 

#include <EGL/egl.h>
#include <glad/gl.h>
#include <stdlib.h> 
#include <stdio.h>
#include <wayland-egl.h>

static struct wl_egl_window *egl_window    = NULL;

// EGL / GL ES globals
static EGLDisplay  egl_display  = EGL_NO_DISPLAY;
static EGLContext  egl_context  = EGL_NO_CONTEXT;
static EGLSurface  egl_surface  = EGL_NO_SURFACE;

// window size
static int width  = 1080;
static int height = 1080;

// Shader sources
static const char *vs_source =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform float uAngle;\n"
    "void main() {\n"
    "  float c = cos(uAngle), s = sin(uAngle);\n"
    "  mat3 rot = mat3(\n"
    "    c, -s, 0,\n"
    "    s,  c, 0,\n"
    "    0,  0, 1\n"
    "  );\n"
    "  gl_Position = vec4(rot * aPos, 1.0);\n"
    "}\n";

static const char *fs_source =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "  FragColor = vec4(1.0, 0.75, 0.2, 1.0);\n"
    "}\n";


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

// build the program once
static GLuint make_program(void) {
    GLuint v = compile_shader(GL_VERTEX_SHADER,   vs_source);
    GLuint f = compile_shader(GL_FRAGMENT_SHADER, fs_source);
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
