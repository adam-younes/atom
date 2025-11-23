#ifndef ATOM_MOVEMENT_SYSTEM_H
#define ATOM_MOVEMENT_SYSTEM_H

#include <scene/scene.h>

void movement_system_move_forward(scene *s, controller_component *cc, float dt);
void movement_system_move_backward(scene *s, controller_component *cc, float dt);
void movement_system_move_left(scene *s, controller_component *cc, float dt);
void movement_system_move_right(scene *s, controller_component *cc, float dt);
void movement_system_move_up(scene *s, controller_component *cc, float dt);
void movement_system_move_down(scene *s, controller_component *cc, float dt);

#endif
