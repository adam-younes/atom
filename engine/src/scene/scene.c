#include <opengl/glad.h>
#include <scene/scene.h>
#include <components/transform.h>
#include <components/mesh_renderer.h>
#include <components/light.h>
#include <components/camera.h>
#include <lib/trig.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

extern int width, height;
extern GLint model_loc, view_loc, proj_loc, normal_loc;

void scene_init(scene *s) {
  memset(s, 0, sizeof(scene));
  s->transform_capacity = 256;
  s->transforms = calloc(s->transform_capacity, sizeof(transform_component));

  s->mesh_renderer_capacity = 256;
  s->mesh_renderers = calloc(s->mesh_renderer_capacity, sizeof(mesh_renderer_component));

  s->light_capacity = 64;
  s->lights = calloc(s->light_capacity, sizeof(light_component));

  s->camera_capacity = 16;
  s->cameras = calloc(s->camera_capacity, sizeof(camera_component));

  s->controller_capacity = 64;
  s->controllers = calloc(s->controller_capacity, sizeof(controller_component));

  s->active_camera = ENTITY_NULL;
}

void scene_destroy(scene *s) {
  for (size_t i = 0; i < s->mesh_renderer_count; i++) {
    mesh_renderer_component_cleanup(&s->mesh_renderers[i]);
  }

  free(s->transforms);
  free(s->mesh_renderers);
  free(s->lights);
  free(s->cameras);
  free(s->controllers);
}

entity_id scene_create_entity(scene *s) {
  (void)s;
  return entity_create();
}

transform_component* scene_add_transform(scene *s, entity_id id) {
  if (s->transform_count >= s->transform_capacity) {
    s->transform_capacity *= 2;
    s->transforms = realloc(s->transforms, s->transform_capacity * sizeof(transform_component));
  }

  transform_component *t = &s->transforms[s->transform_count++];
  transform_component_init(t, id);
  return t;
}

mesh_renderer_component* scene_add_mesh_renderer(scene *s, entity_id id) {
  if (s->mesh_renderer_count >= s->mesh_renderer_capacity) {
    s->mesh_renderer_capacity *= 2;
    s->mesh_renderers = realloc(s->mesh_renderers, s->mesh_renderer_capacity * sizeof(mesh_renderer_component));
  }

  mesh_renderer_component *m = &s->mesh_renderers[s->mesh_renderer_count++];
  mesh_renderer_component_init(m, id);
  return m;
}

light_component* scene_add_light(scene *s, entity_id id) {
  if (s->light_count >= s->light_capacity) {
    s->light_capacity *= 2;
    s->lights = realloc(s->lights, s->light_capacity * sizeof(light_component));
  }

  light_component *l = &s->lights[s->light_count++];
  light_component_init(l, id);
  return l;
}

camera_component* scene_add_camera(scene *s, entity_id id) {
  if (s->camera_count >= s->camera_capacity) {
    s->camera_capacity *= 2;
    s->cameras = realloc(s->cameras, s->camera_capacity * sizeof(camera_component));
  }

  camera_component *c = &s->cameras[s->camera_count++];
  camera_component_init(c, id, to_radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
  return c;
}

controller_component* scene_add_controller(scene *s, entity_id id, entity_id target) {
  if (s->controller_count >= s->controller_capacity) {
    s->controller_capacity *= 2;
    s->controllers = realloc(s->controllers, s->controller_capacity * sizeof(controller_component));
  }

  controller_component *c = &s->controllers[s->controller_count++];
  controller_component_init(c, id, target);
  return c;
}

transform_component* scene_get_transform(scene *s, entity_id id) {
  for (size_t i = 0; i < s->transform_count; i++) {
    if (s->transforms[i].entity == id) {
      return &s->transforms[i];
    }
  }
  return NULL;
}

mesh_renderer_component* scene_get_mesh_renderer(scene *s, entity_id id) {
  for (size_t i = 0; i < s->mesh_renderer_count; i++) {
    if (s->mesh_renderers[i].entity == id) {
      return &s->mesh_renderers[i];
    }
  }
  return NULL;
}

light_component* scene_get_light(scene *s, entity_id id) {
  for (size_t i = 0; i < s->light_count; i++) {
    if (s->lights[i].entity == id) {
      return &s->lights[i];
    }
  }
  return NULL;
}

camera_component* scene_get_camera(scene *s, entity_id id) {
  for (size_t i = 0; i < s->camera_count; i++) {
    if (s->cameras[i].entity == id) {
      return &s->cameras[i];
    }
  }
  return NULL;
}

controller_component* scene_get_controller(scene *s, entity_id id) {
  for (size_t i = 0; i < s->controller_count; i++) {
    if (s->controllers[i].entity == id) {
      return &s->controllers[i];
    }
  }
  return NULL;
}

void scene_update_transforms(scene *s) {
  for (size_t i = 0; i < s->transform_count; i++) {
    transform_component *t = &s->transforms[i];
    transform_component *parent = NULL;

    if (t->parent != ENTITY_NULL) {
      parent = scene_get_transform(s, t->parent);
    }

    transform_component_update(t, parent);
  }
}

void scene_render(scene *s) {
  camera_component *cam = scene_get_camera(s, s->active_camera);
  if (!cam) return;

  for (size_t i = 0; i < s->mesh_renderer_count; i++) {
    mesh_renderer_component *mr = &s->mesh_renderers[i];
    if (!mr->mesh_data || !mr->initialized) continue;

    transform_component *t = scene_get_transform(s, mr->entity);
    if (!t) continue;

    mat4 model = t->world_matrix;
    mat4 normal_mat = mat4_transpose(mat4_inverse(model));

    glUniformMatrix4fv(model_loc, 1, GL_TRUE, &model.m[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_TRUE, &cam->view_matrix.m[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &cam->projection_matrix.m[0][0]);
    glUniformMatrix4fv(normal_loc, 1, GL_TRUE, &normal_mat.m[0][0]);

    glBindVertexArray(mr->vao);
    glDrawElements(GL_TRIANGLES, (GLsizei)*mr->mesh_data->idx_count, GL_UNSIGNED_INT, 0);
  }
}
