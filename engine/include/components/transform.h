#ifndef ATOM_TRANSFORM_COMPONENT_H
#define ATOM_TRANSFORM_COMPONENT_H

#include <scene/entity.h>
#include <lib/la.h>
#include <stdbool.h>

typedef struct {
  entity_id entity;
  entity_id parent;
  vec3 position;
  vec3 rotation;
  vec3 scale;
  mat4 local_matrix;
  mat4 world_matrix;
  bool dirty;
} transform_component;

void transform_component_init(transform_component *t, entity_id id);
void transform_component_update(transform_component *t, transform_component *parent);

#endif
