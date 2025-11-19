#ifndef ATOM_CAMERA_CONTROLLER_H
#define ATOM_CAMERA_CONTROLLER_H

#include <atom/scene.h>

typedef struct {
    scene *target_scene;
    entity_id camera_entity;
    float move_speed;
    float sprint_multiplier;
    float mouse_sensitivity;
    float pitch;
    float yaw;
} camera_controller;

void camera_controller_init(camera_controller *cc, scene *s, entity_id cam_entity);
void camera_controller_move_forward(camera_controller *cc, float dt);
void camera_controller_move_backward(camera_controller *cc, float dt);
void camera_controller_move_left(camera_controller *cc, float dt);
void camera_controller_move_right(camera_controller *cc, float dt);
void camera_controller_move_up(camera_controller *cc, float dt);
void camera_controller_move_down(camera_controller *cc, float dt);
void camera_controller_rotate(camera_controller *cc, float dx, float dy);
vec3 camera_controller_get_forward(camera_controller *cc);
vec3 camera_controller_get_right(camera_controller *cc);

#endif
