#include <components/transform.h>
#include <lib/la.h>
#include <string.h>
#include <math.h>

void transform_component_init(transform_component *t, entity_id id) {
  memset(t, 0, sizeof(transform_component));
  t->entity = id;
  t->parent = ENTITY_NULL;
  t->position = (vec3){0, 0, 0};
  t->rotation = (vec3){0, 0, 0};
  t->scale = (vec3){1, 1, 1};
  t->local_matrix = mat4_identity();
  t->world_matrix = mat4_identity();
  t->dirty = true;
}

void transform_component_update(transform_component *t, transform_component *parent) {
  if (!t->dirty) return;

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

  if (parent) {
    t->world_matrix = mat_mul(parent->world_matrix, t->local_matrix);
  } else {
    t->world_matrix = t->local_matrix;
  }

  t->dirty = false;
}
