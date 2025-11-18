#ifndef ATOM_INPUT_H
#define ATOM_INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ATOM_KEY_W = 0,
    ATOM_KEY_A,
    ATOM_KEY_S,
    ATOM_KEY_D,
    ATOM_KEY_Q,
    ATOM_KEY_E,
    ATOM_KEY_SPACE,
    ATOM_KEY_SHIFT,
    ATOM_KEY_ESC,
    ATOM_KEY_COUNT
} key_code;

typedef void (*key_handler)(float dt);

typedef struct {
    key_code code;
    key_handler on_press;
    key_handler on_release;
    key_handler on_hold;
    bool is_pressed;
} key_binding;

void input_init(void);
void input_update(float dt);
void input_bind_key(key_code code, key_handler on_press, key_handler on_hold, key_handler on_release);
void input_set_key_state(key_code code, bool pressed);
bool input_is_key_pressed(key_code code);

#endif
