#include <atom/input/input.h>
#include <string.h>
#include <stdio.h>

static key_binding key_bindings[ATOM_KEY_COUNT];
static mouse_move_handler mouse_handler = NULL;
static bool mouse_locked = false;

void input_init(void) {
    memset(key_bindings, 0, sizeof(key_bindings));
    for (int i = 0; i < ATOM_KEY_COUNT; i++) {
        key_bindings[i].code = i;
        key_bindings[i].is_pressed = false;
    }
}

void input_update(float dt) {
    for (int i = 0; i < ATOM_KEY_COUNT; i++) {
        if (key_bindings[i].is_pressed && key_bindings[i].on_hold) {
            key_bindings[i].on_hold(dt);
        }
    }
}

void input_bind_key(key_code code, key_handler on_press, key_handler on_hold, key_handler on_release) {
    if (code >= ATOM_KEY_COUNT) return;
    key_bindings[code].on_press = on_press;
    key_bindings[code].on_hold = on_hold;
    key_bindings[code].on_release = on_release;
}

void input_set_key_state(key_code code, bool pressed) {
    if (code >= ATOM_KEY_COUNT) return;

    bool was_pressed = key_bindings[code].is_pressed;
    key_bindings[code].is_pressed = pressed;

    if (pressed && !was_pressed && key_bindings[code].on_press) {
        key_bindings[code].on_press(0.0f);
    } else if (!pressed && was_pressed && key_bindings[code].on_release) {
        key_bindings[code].on_release(0.0f);
    }
}

bool input_is_key_pressed(key_code code) {
    if (code >= ATOM_KEY_COUNT) return false;
    return key_bindings[code].is_pressed;
}

void input_set_mouse_handler(mouse_move_handler handler) {
    mouse_handler = handler;
}

void input_process_mouse_motion(float dx, float dy) {
    if (mouse_locked && mouse_handler) {
        mouse_handler(dx, dy);
    }
}

#include <window/cursor.h>

void input_set_mouse_locked(bool locked) {
    mouse_locked = locked;
    if (locked) {
        window_lock_pointer();
    } else {
        window_unlock_pointer();
    }
}

bool input_is_mouse_locked(void) {
    return mouse_locked;
}
