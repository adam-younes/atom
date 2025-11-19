#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <time.h>
#include <wayland-client.h>
#include <wayland-egl.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <atom/engine.h>
#include <atom/input/input.h>
#include <window/window.h>
#include <window/xdg-shell-client-protocol.h>
#include <atom/lib/graphics.h>

int width = 1080;
int height = 1080;

int atom_run(atom_config *config, atom_callbacks *callbacks) {
  wl_display = wl_display_connect(NULL);
  if (!wl_display) {
    fprintf(stderr, "Failed to connect to Wayland\n");
    return 1;
  }

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

  xdg_toplevel_add_listener(xdg_toplevel, &toplevel_listener, NULL);
  xdg_surface_add_listener(xdg_surface, &xdg_surface_listener, NULL);
  wl_surface_commit(wl_surface);

  wl_display_roundtrip(wl_display);

  width = config->width;
  height = config->height;
  egl_window = wl_egl_window_create(wl_surface, width, height);

  eglBindAPI(EGL_OPENGL_API);

  egl_display = eglGetDisplay((EGLNativeDisplayType)wl_display);
  eglInitialize(egl_display, NULL, NULL);

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

  const EGLint ctx_attribs[] = {
    EGL_CONTEXT_MAJOR_VERSION, 4,
    EGL_CONTEXT_MINOR_VERSION, 5,
    EGL_CONTEXT_OPENGL_PROFILE_MASK,
    EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
    EGL_NONE
  };
  egl_context = eglCreateContext(egl_display, cfg, EGL_NO_CONTEXT, ctx_attribs);
  egl_surface = eglCreateWindowSurface(egl_display, cfg, (EGLNativeWindowType)egl_window, NULL);

  eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);

  init_glad();

  input_init();

  if (callbacks->init) {
    callbacks->init();
  }

  struct timespec last_t;
  clock_gettime(CLOCK_MONOTONIC, &last_t);

  while (wl_display_dispatch_pending(wl_display) != -1 && running) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    float dt = (now.tv_sec - last_t.tv_sec) + (now.tv_nsec - last_t.tv_nsec) * 1e-9f;
    last_t = now;

    input_update(dt);

    if (callbacks->update) {
      callbacks->update(dt);
    }

    if (callbacks->render) {
      callbacks->render();
    }

    eglSwapBuffers(egl_display, egl_surface);
    wl_display_flush(wl_display);
  }

  if (callbacks->cleanup) {
    callbacks->cleanup();
  }

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
