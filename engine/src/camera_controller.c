#include <atom/camera_controller.h>
#include <atom/input.h>
#include <stdio.h>

void camera_controller_init(camera_controller *cc, scene *s, entity_id cam_entity) {
    cc->target_scene = s;
    cc->camera_entity = cam_entity;
    cc->move_speed = 5.0f;
    cc->sprint_multiplier = 2.0f;
}

void camera_controller_move_forward(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    t->position.z -= speed;
    t->dirty = true;
}

void camera_controller_move_backward(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    t->position.z += speed;
    t->dirty = true;
}

void camera_controller_move_left(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    t->position.x -= speed;
    t->dirty = true;
}

void camera_controller_move_right(camera_controller *cc, float dt) {
    transform_component *t = scene_get_transform(cc->target_scene, cc->camera_entity);
    if (!t) return;

    float speed = cc->move_speed * dt;
    if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
        speed *= cc->sprint_multiplier;
    }

    t->position.x += speed;
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
