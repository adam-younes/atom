#include <atom/camera_controller.h>
#include <atom/input.h>
#include <atom/trig.h>
#include <stdio.h>
#include <math.h>

void camera_controller_init(camera_controller *cc, scene *s, entity_id cam_entity) {
    cc->target_scene = s;
    cc->camera_entity = cam_entity;
    cc->move_speed = 5.0f;
    cc->sprint_multiplier = 2.0f;
    cc->mouse_sensitivity = 0.002f;
    cc->pitch = 0.0f;
    cc->yaw = 0.0f;
}

vec3 camera_controller_get_forward(camera_controller *cc) {
    float cy = cosf(cc->yaw);
    float sy = sinf(cc->yaw);
    float cp = cosf(cc->pitch);
    float sp = sinf(cc->pitch);
    return (vec3){ sy * cp, sp, -cy * cp };
}

vec3 camera_controller_get_right(camera_controller *cc) {
    float cy = cosf(cc->yaw);
    float sy = sinf(cc->yaw);
    return (vec3){ cy, 0.0f, sy };
}

void camera_controller_rotate(camera_controller *cc, float dx, float dy) {
    cc->yaw += dx * cc->mouse_sensitivity;
    cc->pitch -= dy * cc->mouse_sensitivity;

    if (cc->pitch > to_radians(89.0f)) cc->pitch = to_radians(89.0f);
    if (cc->pitch < to_radians(-89.0f)) cc->pitch = to_radians(-89.0f);
}

void camera_controller_move_forward(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    vec3 forward = camera_controller_get_forward(cc);
    t->position = vec_sum(t->position, vec_scale(forward, speed));
    t->dirty = true;
}

void camera_controller_move_backward(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    vec3 forward = camera_controller_get_forward(cc);
    t->position = vec_sum(t->position, vec_scale(forward, -speed));
    t->dirty = true;
}

void camera_controller_move_left(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    vec3 right = camera_controller_get_right(cc);
    t->position = vec_sum(t->position, vec_scale(right, -speed));
    t->dirty = true;
}

void camera_controller_move_right(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    vec3 right = camera_controller_get_right(cc);
    t->position = vec_sum(t->position, vec_scale(right, speed));
    t->dirty = true;
}

void camera_controller_move_up(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    t->position.y += speed;
    t->dirty = true;
}

void camera_controller_move_down(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    t->position.y -= speed;
    t->dirty = true;
}
