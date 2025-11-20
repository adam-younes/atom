#ifndef ATOM_CAMERA_COMPONENT_H
#define ATOM_CAMERA_COMPONENT_H

#include <scene/entity.h>
#include <lib/la.h>

typedef struct {
  entity_id entity;
  float fov;
  float aspect;
  float near_plane;
  float far_plane;
  mat4 view_matrix;
  mat4 projection_matrix;
} camera_component;

void camera_component_init(camera_component *c, entity_id id, float fov, float aspect, float near_p, float far_p);
void camera_component_update_view(camera_component *c, vec3 position, vec3 target, vec3 up);
void camera_component_update_projection(camera_component *c);

#endif
