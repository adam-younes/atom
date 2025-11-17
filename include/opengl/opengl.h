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

// Shader sources
static const char *vs_source =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec3 aNormal;\n"
    "uniform mat4 uModel;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProj;\n"
    "uniform mat4 uNormalMat;\n"
    "out vec3 FragPos;\n"
    "out vec3 Normal;\n"
    "void main() {\n"
    "  FragPos = vec3(uModel * vec4(aPos, 1.0));\n"
    "  Normal = mat3(uNormalMat) * aNormal;\n"
    "  gl_Position = uProj * uView * vec4(FragPos, 1.0);\n"
    "}\n";

static const char *fs_source =
    "#version 330 core\n"
    "in vec3 FragPos;\n"
    "in vec3 Normal;\n"
    "out vec4 FragColor;\n"
    "uniform vec3 uLightPos;\n"
    "uniform vec3 uViewPos;\n"
    "uniform vec3 uLightColor;\n"
    "uniform vec3 uObjectColor;\n"
    "void main() {\n"
    "  vec3 ambient = 0.3 * uLightColor;\n"
    "  vec3 norm = normalize(Normal);\n"
    "  vec3 lightDir = normalize(uLightPos - FragPos);\n"
    "  float diff = max(dot(norm, lightDir), 0.0);\n"
    "  vec3 diffuse = diff * uLightColor;\n"
    "  vec3 viewDir = normalize(uViewPos - FragPos);\n"
    "  vec3 reflectDir = reflect(-lightDir, norm);\n"
    "  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);\n"
    "  vec3 specular = 0.8 * spec * uLightColor;\n"
    "  vec3 result = (ambient + diffuse + specular) * uObjectColor;\n"
    "  FragColor = vec4(result, 1.0);\n"
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
