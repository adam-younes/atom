// include/window.h
#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>

#define wayland_display_object_id 1
#define wayland_wl_registry_event_global 0
#define wayland_shm_pool_event_format 0
#define wayland_wl_buffer_event_release 0
#define wayland_xdg_wm_base_event_ping 0
#define wayland_xdg_toplevel_event_configure 0
#define wayland_xdg_toplevel_event_close 1
#define wayland_xdg_surface_event_configure 0
#define wayland_wl_display_get_registry_opcode 1
#define wayland_wl_registry_bind_opcode 0
#define wayland_wl_compositor_create_surface_opcode 0
#define wayland_xdg_wm_base_pong_opcode 3
#define wayland_xdg_surface_ack_configure_opcode 4
#define wayland_wl_shm_create_pool_opcode 0
#define wayland_xdg_wm_base_get_xdg_surface_opcode 2
#define wayland_wl_shm_pool_create_buffer_opcode 0
#define wayland_wl_surface_attach_opcode 1
#define wayland_xdg_surface_get_toplevel_opcode 1
#define wayland_wl_surface_commit_opcode 6
#define wayland_wl_display_error_event 0
#define wayland_format_xrgb8888 1
#define wayland_header_size 8
#define color_channels 4

#define cstring_len(s) (sizeof(s) - 1)

#define roundup_4(n) (((n) + 3) & -4)

typedef enum state_state_t state_state_t;
enum state_state_t {
    STATE_NONE,
    STATE_SURFACE_ACKED_CONFIGURE,
    STATE_SURFACE_ATTACHED,
};

typedef struct state_t state_t;
struct state_t {
    uint32_t wl_registry;
    uint32_t wl_shm;
    uint32_t wl_shm_pool;
    uint32_t wl_buffer;
    uint32_t xdg_wm_base;
    uint32_t xdg_surface;
    uint32_t wl_compositor;
    uint32_t wl_surface;
    uint32_t xdg_toplevel;
    uint32_t stride;
    uint32_t w;
    uint32_t h;
    uint32_t shm_pool_size;
    uint32_t shm_fd;
    uint8_t *shm_pool_data;

    state_state_t state;
};

// connect to the Wayland socket
int wayland_display_connect(void);
// send a “get_registry” request
uint32_t wayland_wl_display_get_registry(int fd);
// create a new surface
uint32_t wayland_wl_compositor_create_surface(int fd, state_t *state);
// allocate & map your shm pool
void create_shared_memory_file(uint64_t size, state_t *state);
// send a “create_pool” request
uint32_t wayland_wl_shm_create_pool(int fd, state_t *state);
// send a “get_xdg_surface” request
uint32_t wayland_xdg_wm_base_get_xdg_surface(int fd, state_t *state);
// send a “create_buffer” request
uint32_t wayland_wl_shm_pool_create_buffer(int fd, state_t *state);
// attach your buffer to the surface
void wayland_wl_surface_attach(int fd, state_t *state);
// tell the compositor “commit that attach”
void wayland_wl_surface_commit(int fd, state_t *state);
// send a “get_toplevel” request
uint32_t wayland_xdg_surface_get_toplevel(int fd, state_t *state);
// read & dispatch an incoming event
void wayland_handle_message(int fd, state_t *state, char **msg, uint64_t *msg_len);

#endif // WINDOW_H
