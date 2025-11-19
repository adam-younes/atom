#ifndef ATOM_COMPONENTS_H
#define ATOM_COMPONENTS_H

#include <stdint.h>
#include <stdbool.h>
#include <atom/entity.h>
#include <atom/la.h>
#include <atom/mesh.h>

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

typedef struct {
  entity_id entity;
  vec3 position;
  vec3 color;
  float intensity;
} light_component;

typedef struct {
  entity_id entity;
  float fov;
  float aspect;
  float near_plane;
  float far_plane;
  mat4 view_matrix;
  mat4 projection_matrix;
} camera_component;

#endif
