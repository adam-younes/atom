#include <wayland-client.h>
#include <wayland-cursor.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <window/xdg-shell-client-protocol.h>
#include <window/pointer-constraints-unstable-v1-client-protocol.h>
#include <window/relative-pointer-unstable-v1-client-protocol.h>
#include <linux/input-event-codes.h>
#include <atom/input.h>

static bool running = true;

// globals for Wayland
static struct wl_display        *wl_display    = NULL;
static struct wl_registry       *wl_registry   = NULL;
static struct wl_compositor     *wl_compositor = NULL;
static struct wl_shm            *wl_shm        = NULL;
static struct xdg_wm_base       *xdg_wm_base   = NULL;
static struct wl_surface        *wl_surface    = NULL;
static struct xdg_surface       *xdg_surface   = NULL;
static struct xdg_toplevel      *xdg_toplevel  = NULL;
static struct wl_seat           *wl_seat       = NULL;
static struct wl_keyboard       *wl_keyboard   = NULL;
static struct wl_pointer        *wl_pointer    = NULL;
static struct wl_cursor_theme   *cursor_theme  = NULL;
static struct wl_cursor         *cursor        = NULL;
static struct wl_surface        *cursor_surface = NULL;
static struct zwp_pointer_constraints_v1 *pointer_constraints = NULL;
static struct zwp_relative_pointer_manager_v1 *relative_pointer_manager = NULL;
static struct zwp_relative_pointer_v1 *relative_pointer = NULL;
static struct zwp_locked_pointer_v1 *locked_pointer = NULL;
static struct zwp_confined_pointer_v1 *confined_pointer = NULL;
static double last_mouse_x = 0.0;
static double last_mouse_y = 0.0;
static bool mouse_initialized = false;

// wayland listeners
static key_code linux_keycode_to_key_code(uint32_t key) {
  switch (key) {
    case 17: return ATOM_KEY_W;
    case 30: return ATOM_KEY_A;
    case 31: return ATOM_KEY_S;
    case 32: return ATOM_KEY_D;
    case 16: return ATOM_KEY_Q;
    case 18: return ATOM_KEY_E;
    case 57: return ATOM_KEY_SPACE;
    case 42:
    case 54: return ATOM_KEY_SHIFT;
    case 1: return ATOM_KEY_ESC;
    default: return ATOM_KEY_COUNT;
  }
}

static void keyboard_key(void *data, struct wl_keyboard *kb, uint32_t serial,
                         uint32_t time, uint32_t key, uint32_t state) {
  (void)data; (void)kb; (void)serial; (void)time;
  key_code code = linux_keycode_to_key_code(key);
  if (code != ATOM_KEY_COUNT) {
    input_set_key_state(code, state == WL_KEYBOARD_KEY_STATE_PRESSED);
  }
}

static void keyboard_keymap(void *data, struct wl_keyboard *kb, uint32_t format,
                            int32_t fd, uint32_t size) {
  (void)data; (void)kb; (void)format; (void)fd; (void)size;
}

static void keyboard_enter(void *data, struct wl_keyboard *kb, uint32_t serial,
                          struct wl_surface *surface, struct wl_array *keys) {
  (void)data; (void)kb; (void)serial; (void)surface; (void)keys;
}

static void keyboard_leave(void *data, struct wl_keyboard *kb, uint32_t serial,
                          struct wl_surface *surface) {
  (void)data; (void)kb; (void)serial; (void)surface;
}

static void keyboard_modifiers(void *data, struct wl_keyboard *kb, uint32_t serial,
                               uint32_t mods_depressed, uint32_t mods_latched,
                               uint32_t mods_locked, uint32_t group) {
  (void)data; (void)kb; (void)serial; (void)mods_depressed;
  (void)mods_latched; (void)mods_locked; (void)group;
}

static void keyboard_repeat_info(void *data, struct wl_keyboard *kb,
                                 int32_t rate, int32_t delay) {
  (void)data; (void)kb; (void)rate; (void)delay;
}

static const struct wl_keyboard_listener keyboard_listener = {
  .keymap = keyboard_keymap,
  .enter = keyboard_enter,
  .leave = keyboard_leave,
  .key = keyboard_key,
  .modifiers = keyboard_modifiers,
  .repeat_info = keyboard_repeat_info,
};

static void relative_pointer_motion(void *data,
                                   struct zwp_relative_pointer_v1 *pointer,
                                   uint32_t utime_hi, uint32_t utime_lo,
                                   wl_fixed_t dx, wl_fixed_t dy,
                                   wl_fixed_t dx_unaccel, wl_fixed_t dy_unaccel) {
  (void)data; (void)pointer; (void)utime_hi; (void)utime_lo;
  (void)dx_unaccel; (void)dy_unaccel;

  float fdx = wl_fixed_to_double(dx);
  float fdy = wl_fixed_to_double(dy);
  input_process_mouse_motion(fdx, fdy);
}

static const struct zwp_relative_pointer_v1_listener relative_pointer_listener = {
  .relative_motion = relative_pointer_motion,
};

static void set_cursor_visible(bool visible) {
  if (!wl_pointer) return;

  if (visible) {
    if (!cursor_theme) {
      cursor_theme = wl_cursor_theme_load(NULL, 24, wl_shm);
      if (cursor_theme) {
        cursor = wl_cursor_theme_get_cursor(cursor_theme, "left_ptr");
      }
    }
  } else {
    wl_pointer_set_cursor(wl_pointer, 0, NULL, 0, 0);
  }
}

static void pointer_enter(void *data, struct wl_pointer *pointer, uint32_t serial,
                         struct wl_surface *surface, wl_fixed_t sx, wl_fixed_t sy) {
  (void)data; (void)surface; (void)sx; (void)sy;

  if (input_is_mouse_locked()) {
    wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
  }
}

static void pointer_leave(void *data, struct wl_pointer *pointer, uint32_t serial,
                         struct wl_surface *surface) {
  (void)data; (void)pointer; (void)serial; (void)surface;
}

static void pointer_motion(void *data, struct wl_pointer *pointer, uint32_t time,
                          wl_fixed_t sx, wl_fixed_t sy) {
  (void)data; (void)pointer; (void)time;

  double x = wl_fixed_to_double(sx);
  double y = wl_fixed_to_double(sy);

  if (mouse_initialized) {
    float dx = (float)(x - last_mouse_x);
    float dy = (float)(y - last_mouse_y);
    input_process_mouse_motion(dx, dy);
  }

  last_mouse_x = x;
  last_mouse_y = y;
  mouse_initialized = true;
}

void window_lock_pointer(void) {
  if (!pointer_constraints || !wl_pointer || locked_pointer) return;

  if (relative_pointer_manager && !relative_pointer) {
    relative_pointer = zwp_relative_pointer_manager_v1_get_relative_pointer(
      relative_pointer_manager, wl_pointer
    );
    zwp_relative_pointer_v1_add_listener(relative_pointer, &relative_pointer_listener, NULL);
  }

  locked_pointer = zwp_pointer_constraints_v1_lock_pointer(
    pointer_constraints, wl_surface, wl_pointer, NULL,
    ZWP_POINTER_CONSTRAINTS_V1_LIFETIME_PERSISTENT
  );
  wl_pointer_set_cursor(wl_pointer, 0, NULL, 0, 0);
}

void window_unlock_pointer(void) {
  if (locked_pointer) {
    zwp_locked_pointer_v1_destroy(locked_pointer);
    locked_pointer = NULL;
  }
  if (relative_pointer) {
    zwp_relative_pointer_v1_destroy(relative_pointer);
    relative_pointer = NULL;
  }
}

static void pointer_button(void *data, struct wl_pointer *pointer, uint32_t serial,
                          uint32_t time, uint32_t button, uint32_t state) {
  (void)data; (void)time; (void)button;
  if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
    bool locked = input_is_mouse_locked();
    input_set_mouse_locked(!locked);

    if (!locked) {
      wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
      window_lock_pointer();
    } else {
      window_unlock_pointer();
      if (cursor && cursor->images[0]) {
        struct wl_cursor_image *image = cursor->images[0];
        if (!cursor_surface) {
          cursor_surface = wl_compositor_create_surface(wl_compositor);
        }
        wl_pointer_set_cursor(pointer, serial, cursor_surface,
                             image->hotspot_x, image->hotspot_y);
        wl_surface_attach(cursor_surface, wl_cursor_image_get_buffer(image), 0, 0);
        wl_surface_damage(cursor_surface, 0, 0, image->width, image->height);
        wl_surface_commit(cursor_surface);
      }
    }
  }
}

static void pointer_axis(void *data, struct wl_pointer *pointer, uint32_t time,
                        uint32_t axis, wl_fixed_t value) {
  (void)data; (void)pointer; (void)time; (void)axis; (void)value;
}

static const struct wl_pointer_listener pointer_listener = {
  .enter = pointer_enter,
  .leave = pointer_leave,
  .motion = pointer_motion,
  .button = pointer_button,
  .axis = pointer_axis,
};

static void seat_capabilities(void *data, struct wl_seat *seat, uint32_t caps) {
  (void)data;
  if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
    wl_keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_add_listener(wl_keyboard, &keyboard_listener, NULL);
  }
  if (caps & WL_SEAT_CAPABILITY_POINTER) {
    wl_pointer = wl_seat_get_pointer(seat);
    wl_pointer_add_listener(wl_pointer, &pointer_listener, NULL);

    if (wl_shm) {
      cursor_theme = wl_cursor_theme_load(NULL, 24, wl_shm);
      if (cursor_theme) {
        cursor = wl_cursor_theme_get_cursor(cursor_theme, "left_ptr");
      }
    }
  }
}

static void seat_name(void *data, struct wl_seat *seat, const char *name) {
  (void)data; (void)seat; (void)name;
}

static const struct wl_seat_listener seat_listener = {
  .capabilities = seat_capabilities,
  .name = seat_name,
};

static void registry_global(void *data,
                            struct wl_registry *reg,
                            uint32_t name,
                            const char *iface,
                            uint32_t version)
{
  (void)data; (void)version;
  if (strcmp(iface, wl_compositor_interface.name) == 0) {
    wl_compositor = wl_registry_bind(reg, name, &wl_compositor_interface, 1);
  }
  else if (strcmp(iface, wl_shm_interface.name) == 0) {
    wl_shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
  }
  else if (strcmp(iface, xdg_wm_base_interface.name) == 0) {
    xdg_wm_base = wl_registry_bind(reg, name, &xdg_wm_base_interface, 1);
  }
  else if (strcmp(iface, wl_seat_interface.name) == 0) {
    wl_seat = wl_registry_bind(reg, name, &wl_seat_interface, 1);
    wl_seat_add_listener(wl_seat, &seat_listener, NULL);
  }
  else if (strcmp(iface, zwp_pointer_constraints_v1_interface.name) == 0) {
    pointer_constraints = wl_registry_bind(reg, name, &zwp_pointer_constraints_v1_interface, 1);
  }
  else if (strcmp(iface, zwp_relative_pointer_manager_v1_interface.name) == 0) {
    relative_pointer_manager = wl_registry_bind(reg, name, &zwp_relative_pointer_manager_v1_interface, 1);
  }
}

static const struct wl_registry_listener registry_listener = {
  .global = registry_global,
  .global_remove = NULL,
};

static void xdg_wm_base_ping(void *d, struct xdg_wm_base *base, uint32_t serial) {
  (void)d;
  xdg_wm_base_pong(base, serial);
}

static const struct xdg_wm_base_listener wm_base_listener = {
  .ping = xdg_wm_base_ping
};

static void xdg_surface_configure(void *data, struct xdg_surface *surf, uint32_t serial) {
  (void)data;
  xdg_surface_ack_configure(surf, serial);
  wl_surface_commit(wl_surface);
}

static const struct xdg_surface_listener xdg_surface_listener = {
  .configure = xdg_surface_configure
};

static void handle_toplevel_configure(void *data, struct xdg_toplevel *toplevel,
                                      int32_t width, int32_t height,
                                      struct wl_array *states) {
  (void)data; (void)toplevel; (void)width; (void)height; (void)states;
}

static void handle_toplevel_close(void *data, struct xdg_toplevel *toplevel) {
  (void)data; (void)toplevel;
  running = false;
}

static const struct xdg_toplevel_listener toplevel_listener = {
  .configure = handle_toplevel_configure,
  .close = handle_toplevel_close,
};
