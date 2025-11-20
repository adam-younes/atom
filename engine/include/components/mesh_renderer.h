#ifndef ATOM_MESH_RENDERER_COMPONENT_H
#define ATOM_MESH_RENDERER_COMPONENT_H

#include <scene/entity.h>
#include <assets/mesh.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  entity_id entity;
  mesh *mesh_data;
  uint32_t material_id;
  uint32_t vao;
  uint32_t vbo_pos;
  uint32_t vbo_norm;
  uint32_t ebo;
  bool initialized;
} mesh_renderer_component;

void mesh_renderer_component_init(mesh_renderer_component *mr, entity_id id);
void mesh_renderer_component_cleanup(mesh_renderer_component *mr);

#endif
