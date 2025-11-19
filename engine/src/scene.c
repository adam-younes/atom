#include <opengl/glad.h>
#include <atom/scene.h>
#include <atom/trig.h>
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

  s->active_camera = ENTITY_NULL;
}

void scene_destroy(scene *s) {
  for (size_t i = 0; i < s->mesh_renderer_count; i++) {
    if (s->mesh_renderers[i].initialized) {
      glDeleteBuffers(1, &s->mesh_renderers[i].ebo);
      glDeleteBuffers(1, &s->mesh_renderers[i].vbo_norm);
      glDeleteBuffers(1, &s->mesh_renderers[i].vbo_pos);
      glDeleteVertexArrays(1, &s->mesh_renderers[i].vao);
    }
  }

  free(s->transforms);
  free(s->mesh_renderers);
  free(s->lights);
  free(s->cameras);
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
  memset(t, 0, sizeof(transform_component));
  t->entity = id;
  t->parent = ENTITY_NULL;
  t->position = (vec3){0, 0, 0};
  t->rotation = (vec3){0, 0, 0};
  t->scale = (vec3){1, 1, 1};
  t->local_matrix = mat4_identity();
  t->world_matrix = mat4_identity();
  t->dirty = true;
  return t;
}

mesh_renderer_component* scene_add_mesh_renderer(scene *s, entity_id id) {
  if (s->mesh_renderer_count >= s->mesh_renderer_capacity) {
    s->mesh_renderer_capacity *= 2;
    s->mesh_renderers = realloc(s->mesh_renderers, s->mesh_renderer_capacity * sizeof(mesh_renderer_component));
  }

  mesh_renderer_component *m = &s->mesh_renderers[s->mesh_renderer_count++];
  memset(m, 0, sizeof(mesh_renderer_component));
  m->entity = id;
  m->initialized = false;
  return m;
}

light_component* scene_add_light(scene *s, entity_id id) {
  if (s->light_count >= s->light_capacity) {
    s->light_capacity *= 2;
    s->lights = realloc(s->lights, s->light_capacity * sizeof(light_component));
  }

  light_component *l = &s->lights[s->light_count++];
  memset(l, 0, sizeof(light_component));
  l->entity = id;
  l->color = (vec3){1, 1, 1};
  l->intensity = 1.0f;
  return l;
}

camera_component* scene_add_camera(scene *s, entity_id id) {
  if (s->camera_count >= s->camera_capacity) {
    s->camera_capacity *= 2;
    s->cameras = realloc(s->cameras, s->camera_capacity * sizeof(camera_component));
  }

  camera_component *c = &s->cameras[s->camera_count++];
  memset(c, 0, sizeof(camera_component));
  c->entity = id;
  c->fov = to_radians(45.0f);
  c->aspect = (float)width / (float)height;
  c->near_plane = 0.1f;
  c->far_plane = 1000.0f;
  c->view_matrix = mat4_identity();
  c->projection_matrix = mat4_identity();
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

void scene_update_transforms(scene *s) {
  for (size_t i = 0; i < s->transform_count; i++) {
    transform_component *t = &s->transforms[i];

    if (!t->dirty) continue;

    mat4 translation = mat4_identity();
    translation.m[0][3] = t->position.x;
    translation.m[1][3] = t->position.y;
    translation.m[2][3] = t->position.z;

    mat4 rotation_x = mat4_identity();
    float cx = cosf(t->rotation.x);
    float sx = sinf(t->rotation.x);
    rotation_x.m[1][1] = cx;
    rotation_x.m[1][2] = -sx;
    rotation_x.m[2][1] = sx;
    rotation_x.m[2][2] = cx;

    mat4 rotation_y = mat4_identity();
    float cy = cosf(t->rotation.y);
    float sy = sinf(t->rotation.y);
    rotation_y.m[0][0] = cy;
    rotation_y.m[0][2] = sy;
    rotation_y.m[2][0] = -sy;
    rotation_y.m[2][2] = cy;

    mat4 rotation_z = mat4_identity();
    float cz = cosf(t->rotation.z);
    float sz = sinf(t->rotation.z);
    rotation_z.m[0][0] = cz;
    rotation_z.m[0][1] = -sz;
    rotation_z.m[1][0] = sz;
    rotation_z.m[1][1] = cz;

    mat4 scale_mat = mat4_identity();
    scale_mat.m[0][0] = t->scale.x;
    scale_mat.m[1][1] = t->scale.y;
    scale_mat.m[2][2] = t->scale.z;

    t->local_matrix = mat_mul(translation, mat_mul(mat_mul(rotation_z, mat_mul(rotation_y, rotation_x)), scale_mat));

    if (t->parent != ENTITY_NULL) {
      transform_component *parent = scene_get_transform(s, t->parent);
      if (parent) {
        t->world_matrix = mat_mul(parent->world_matrix, t->local_matrix);
      } else {
        t->world_matrix = t->local_matrix;
      }
    } else {
      t->world_matrix = t->local_matrix;
    }

    t->dirty = false;
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
