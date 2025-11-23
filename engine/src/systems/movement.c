#include <systems/movement.h>
#include <input/input.h>
#include <lib/la.h>

void movement_system_move_forward(scene *s, controller_component *cc, float dt) {
  transform_component *t = scene_get_transform(s, cc->target_entity);
  if (!t) return;

  float speed = cc->move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
    speed *= cc->sprint_multiplier;
  }

  vec3 forward = controller_get_forward(cc);
  t->position = vec_sum(t->position, vec_scale(forward, speed));
  t->dirty = true;
}

void movement_system_move_backward(scene *s, controller_component *cc, float dt) {
  transform_component *t = scene_get_transform(s, cc->target_entity);
  if (!t) return;

  float speed = cc->move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
    speed *= cc->sprint_multiplier;
  }

  vec3 forward = controller_get_forward(cc);
  t->position = vec_sum(t->position, vec_scale(forward, -speed));
  t->dirty = true;
}

void movement_system_move_left(scene *s, controller_component *cc, float dt) {
  transform_component *t = scene_get_transform(s, cc->target_entity);
  if (!t) return;

  float speed = cc->move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
    speed *= cc->sprint_multiplier;
  }

  vec3 right = controller_get_right(cc);
  t->position = vec_sum(t->position, vec_scale(right, -speed));
  t->dirty = true;
}

void movement_system_move_right(scene *s, controller_component *cc, float dt) {
  transform_component *t = scene_get_transform(s, cc->target_entity);
  if (!t) return;

  float speed = cc->move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
    speed *= cc->sprint_multiplier;
  }

  vec3 right = controller_get_right(cc);
  t->position = vec_sum(t->position, vec_scale(right, speed));
  t->dirty = true;
}

void movement_system_move_up(scene *s, controller_component *cc, float dt) {
  transform_component *t = scene_get_transform(s, cc->target_entity);
  if (!t) return;

  float speed = cc->move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
    speed *= cc->sprint_multiplier;
  }

  t->position.y += speed;
  t->dirty = true;
}

void movement_system_move_down(scene *s, controller_component *cc, float dt) {
  transform_component *t = scene_get_transform(s, cc->target_entity);
  if (!t) return;

  float speed = cc->move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) {
    speed *= cc->sprint_multiplier;
  }

  t->position.y -= speed;
  t->dirty = true;
}
