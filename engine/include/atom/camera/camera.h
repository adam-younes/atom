#ifndef ATOM_CAMERA_H
#define ATOM_CAMERA_H

#include <atom/lib/la.h>

typedef struct {
  vec3    position;
  vec3    target; 
  vec3    up;
  float   fov_y;
  float   aspect;
  float   znear, zfar;
} camera;

static inline mat4 look_at(camera *cam); 
mat4 perspective(camera *cam);

#endif
