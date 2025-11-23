#ifndef ATOM_CONTROLLER_COMPONENT_H
#define ATOM_CONTROLLER_COMPONENT_H

#include <scene/entity.h>
#include <lib/la.h>
#include <stdbool.h>

typedef struct {
  entity_id entity;
  entity_id target_entity;
  float move_speed;
  float sprint_multiplier;
  float mouse_sensitivity;
  float pitch;
  float yaw;
  bool use_pitch_yaw;
} controller_component;

void controller_component_init(controller_component *cc, entity_id id, entity_id target);
vec3 controller_get_forward(controller_component *cc);
vec3 controller_get_right(controller_component *cc);
void controller_rotate(controller_component *cc, float dx, float dy);

#endif
