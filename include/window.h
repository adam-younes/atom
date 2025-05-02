#include <wayland-client.h>
#include <string.h>
#include <stdbool.h>
#include <xdg-shell-client-protocol.h>

static bool running = true;

// globals for Wayland
static struct wl_display        *wl_display    = NULL;
static struct wl_registry       *wl_registry   = NULL;
static struct wl_compositor     *wl_compositor = NULL;
static struct xdg_wm_base       *xdg_wm_base   = NULL;
static struct wl_surface        *wl_surface    = NULL;
static struct xdg_surface       *xdg_surface   = NULL;
static struct xdg_toplevel      *xdg_toplevel  = NULL;

// wayland listeners
static void registry_global(void *data,
                            struct wl_registry *reg,
                            uint32_t name,
                            const char *iface,
                            uint32_t version)
{
    if (strcmp(iface, wl_compositor_interface.name) == 0) {
        wl_compositor = wl_registry_bind(reg, name,
                                         &wl_compositor_interface, 1);
    }
    else if (strcmp(iface, xdg_wm_base_interface.name) == 0) {
        xdg_wm_base = wl_registry_bind(reg, name,
                                       &xdg_wm_base_interface, 1);
    }
}

static const struct wl_registry_listener registry_listener = {
    .global = registry_global,
    .global_remove = NULL,
};

static void xdg_wm_base_ping(void *d, struct xdg_wm_base *base, uint32_t serial) {
    xdg_wm_base_pong(base, serial);
}
static const struct xdg_wm_base_listener wm_base_listener = {
    .ping = xdg_wm_base_ping
};

static void xdg_surface_configure(void *data,
                                  struct xdg_surface *surf,
                                  uint32_t serial)
{
    // ack configure and redraw
    xdg_surface_ack_configure(surf, serial);
    wl_surface_commit(wl_surface);
}
static const struct xdg_surface_listener xdg_surface_listener = {
    .configure = xdg_surface_configure
};

// forward‐declare your configure handler if you already have one:
static void handle_toplevel_configure(void *data,
                               struct xdg_toplevel *toplevel,
                               int32_t width, int32_t height,
                               struct wl_array *states) {
}

// close handler
static void handle_toplevel_close(void *data,
                                  struct xdg_toplevel *toplevel)
{
    running = false;
}

// bind both handlers:
static const struct xdg_toplevel_listener toplevel_listener = {
    .configure = handle_toplevel_configure,
    .close     = handle_toplevel_close,
};
