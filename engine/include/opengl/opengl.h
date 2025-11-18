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
extern int width;
extern int height;

void init_glad(void);
GLuint make_program_from_files(const char *vs_path, const char *fs_path);

#endif
