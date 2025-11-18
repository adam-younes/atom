#include <math/la.h>
#include <camera/camera.h>

static inline mat4 look_at(camera *cam) {
  // forward = normalize(center − eye)
  vec3 forward = vec_normalize((vec3) {
    cam->position.x - cam->target.x,
    cam->position.y - cam->target.y,
    cam->position.z - cam->target.z
  });

  // side = normalize(cross(forward, up))
  vec3 side = vec3_normalize(vec_cross(forward, up));

  // true up = cross(side, forward)
  vec3 camera_up = vec_cross(side, forward);

  // start from identity
  mat4 result = mat4_identity();

  // rotation part
  result.m[0][0] = side.x;
  result.m[0][1] = side.y;
  result.m[0][2] = side.z;

  result.m[1][0] = camera_up.x;
  result.m[1][1] = camera_up.y;
  result.m[1][2] = camera_up.z;

  result.m[2][0] = -forward.x;
  result.m[2][1] = -forward.y;
  result.m[2][2] = -forward.z;

  // translation part (dot of eye with axes)
  result.m[0][3] = -vec_dot(side, eye);
  result.m[1][3] = -vec_dot(camera_up, eye);
  result.m[2][3] =  vec_dot(forward, eye);

  return result;
}
