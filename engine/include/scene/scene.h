#ifndef ATOM_SCENE_H
#define ATOM_SCENE_H

#include <scene/entity.h>
#include <scene/components.h>
#include <stddef.h>

typedef struct {
  transform_component *transforms;
  size_t transform_count;
  size_t transform_capacity;

  mesh_renderer_component *mesh_renderers;
  size_t mesh_renderer_count;
  size_t mesh_renderer_capacity;

  light_component *lights;
  size_t light_count;
  size_t light_capacity;

  camera_component *cameras;
  size_t camera_count;
  size_t camera_capacity;

  entity_id active_camera;
} scene;

void scene_init(scene *s);
void scene_destroy(scene *s);

entity_id scene_create_entity(scene *s);

transform_component* scene_add_transform(scene *s, entity_id id);
mesh_renderer_component* scene_add_mesh_renderer(scene *s, entity_id id);
light_component* scene_add_light(scene *s, entity_id id);
camera_component* scene_add_camera(scene *s, entity_id id);

transform_component* scene_get_transform(scene *s, entity_id id);
mesh_renderer_component* scene_get_mesh_renderer(scene *s, entity_id id);
light_component* scene_get_light(scene *s, entity_id id);
camera_component* scene_get_camera(scene *s, entity_id id);

void scene_update_transforms(scene *s);
void scene_render(scene *s);

#endif
