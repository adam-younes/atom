#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>

#include "window.h"
#include "wayland-logo.h"

// @main
int main() {
    struct timeval tv = {0};
    assert(gettimeofday(&tv, NULL) != -1);
    srand(tv.tv_sec * 1000 * 1000 + tv.tv_usec);

    int fd = wayland_display_connect();

    state_t state = {
        .wl_registry = wayland_wl_display_get_registry(fd),
        .w = 117,
        .h = 150,
        .stride = 117 * color_channels,
    };

    // Single buffering.
    state.shm_pool_size = state.h * state.stride;
    create_shared_memory_file(state.shm_pool_size, &state);

    while (1) {
        char read_buf[4096] = "";
        int64_t read_bytes = recv(fd, read_buf, sizeof(read_buf), 0);
        if (read_bytes == -1)
            exit(errno);

        char *msg = read_buf;
        uint64_t msg_len = (uint64_t)read_bytes;

        while (msg_len > 0)
            wayland_handle_message(fd, &state, &msg, &msg_len);

        if (state.wl_compositor != 0 && state.wl_shm != 0 &&
            state.xdg_wm_base != 0 &&
            state.wl_surface == 0) { // Bind phase complete, need to create surface.
            assert(state.state == STATE_NONE);

            state.wl_surface = wayland_wl_compositor_create_surface(fd, &state);
            state.xdg_surface = wayland_xdg_wm_base_get_xdg_surface(fd, &state);
            state.xdg_toplevel = wayland_xdg_surface_get_toplevel(fd, &state);
            wayland_wl_surface_commit(fd, &state);
        }

        if (state.state == STATE_SURFACE_ACKED_CONFIGURE) {
            // Render a frame.
            assert(state.wl_surface != 0);
            assert(state.xdg_surface != 0);
            assert(state.xdg_toplevel != 0);

            if (state.wl_shm_pool == 0)
                state.wl_shm_pool = wayland_wl_shm_create_pool(fd, &state);
            if (state.wl_buffer == 0)
                state.wl_buffer = wayland_wl_shm_pool_create_buffer(fd, &state);

            assert(state.shm_pool_data != 0);
            assert(state.shm_pool_size != 0);

            uint32_t *pixels = (uint32_t *)state.shm_pool_data;
            for (uint32_t i = 0; i < state.w * state.h; i++) {
                uint8_t r = wayland_logo[i * 3 + 0];
                uint8_t g = wayland_logo[i * 3 + 1];
                uint8_t b = wayland_logo[i * 3 + 2];
                pixels[i] = (r << 16) | (g << 8) | b;
            }
            wayland_wl_surface_attach(fd, &state);
            wayland_wl_surface_commit(fd, &state);

            state.state = STATE_SURFACE_ATTACHED;
        }
    }
}
