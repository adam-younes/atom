#include <components/camera.h>
#include <lib/trig.h>
#include <string.h>
#include <math.h>

extern int width, height;

void camera_component_init(camera_component *c, entity_id id, float fov, float aspect, float near_p, float far_p) {
  memset(c, 0, sizeof(camera_component));
  c->entity = id;
  c->fov = fov;
  c->aspect = aspect;
  c->near_plane = near_p;
  c->far_plane = far_p;
  c->view_matrix = mat4_identity();
  c->projection_matrix = mat4_identity();
}

void camera_component_update_view(camera_component *c, vec3 position, vec3 target, vec3 up) {
  vec3 forward = vec_normalize(vec_sum(target, vec_negate(position)));
  vec3 side = vec_normalize(vec_cross(forward, up));
  vec3 camera_up = vec_cross(side, forward);

  mat4 result = mat4_identity();

  result.m[0][0] = side.x;
  result.m[0][1] = side.y;
  result.m[0][2] = side.z;

  result.m[1][0] = camera_up.x;
  result.m[1][1] = camera_up.y;
  result.m[1][2] = camera_up.z;

  result.m[2][0] = -forward.x;
  result.m[2][1] = -forward.y;
  result.m[2][2] = -forward.z;

  result.m[0][3] = -vec_dot(side, position);
  result.m[1][3] = -vec_dot(camera_up, position);
  result.m[2][3] = vec_dot(forward, position);

  c->view_matrix = result;
}

void camera_component_update_projection(camera_component *c) {
  float f = 1.0f / tanf(c->fov * 0.5f);

  mat4 result = {0};
  result.m[0][0] = f / c->aspect;
  result.m[1][1] = f;
  result.m[2][2] = (c->far_plane + c->near_plane) / (c->near_plane - c->far_plane);
  result.m[2][3] = (2.0f * c->far_plane * c->near_plane) / (c->near_plane - c->far_plane);
  result.m[3][2] = -1.0f;

  c->projection_matrix = result;
}
