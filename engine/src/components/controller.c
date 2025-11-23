#include <components/controller.h>
#include <lib/trig.h>
#include <string.h>
#include <math.h>

void controller_component_init(controller_component *cc, entity_id id, entity_id target) {
  memset(cc, 0, sizeof(controller_component));
  cc->entity = id;
  cc->target_entity = target;
  cc->move_speed = 5.0f;
  cc->sprint_multiplier = 2.0f;
  cc->mouse_sensitivity = 0.002f;
  cc->pitch = 0.0f;
  cc->yaw = 0.0f;
  cc->use_pitch_yaw = true;
}

vec3 controller_get_forward(controller_component *cc) {
  if (!cc->use_pitch_yaw) {
    return (vec3){0, 0, -1};
  }

  float cy = cosf(cc->yaw);
  float sy = sinf(cc->yaw);
  float cp = cosf(cc->pitch);
  float sp = sinf(cc->pitch);
  return (vec3){ sy * cp, sp, -cy * cp };
}

vec3 controller_get_right(controller_component *cc) {
  if (!cc->use_pitch_yaw) {
    return (vec3){1, 0, 0};
  }

  float cy = cosf(cc->yaw);
  float sy = sinf(cc->yaw);
  return (vec3){ cy, 0.0f, sy };
}

void controller_rotate(controller_component *cc, float dx, float dy) {
  cc->yaw += dx * cc->mouse_sensitivity;
  cc->pitch -= dy * cc->mouse_sensitivity;

  if (cc->pitch > to_radians(89.0f)) cc->pitch = to_radians(89.0f);
  if (cc->pitch < to_radians(-89.0f)) cc->pitch = to_radians(-89.0f);
}
