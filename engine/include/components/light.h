#ifndef ATOM_LIGHT_COMPONENT_H
#define ATOM_LIGHT_COMPONENT_H

#include <scene/entity.h>
#include <lib/la.h>

typedef struct {
  entity_id entity;
  vec3 position;
  vec3 color;
  float intensity;
} light_component;

void light_component_init(light_component *l, entity_id id);

#endif
