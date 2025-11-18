#include <stdio.h>
#include <float.h>

#include <GLES2/gl2.h>
#include <atom/engine.h>
#include <atom/scene.h>
#include <atom/input.h>
#include <atom/camera_controller.h>
#include <math/la.h>
#include <math/trig.h>
#include <mesh/mesh.h>
#include <opengl/opengl.h>

static scene game_scene;
static mesh teapot_mesh;
static entity_id teapot_entity;
static entity_id camera_entity;
static entity_id light_entity;
static camera_controller cam_controller;
static vec3 camera_target;

static GLuint program;
GLint model_loc, view_loc, proj_loc, normal_loc;
static GLint light_pos_loc, view_pos_loc, light_color_loc, object_color_loc;

extern int width, height;

static void cam_move_forward(float dt);
static void cam_move_backward(float dt);
static void cam_move_left(float dt);
static void cam_move_right(float dt);
static void cam_move_up(float dt);
static void cam_move_down(float dt);

void game_init(void) {
  load_mesh("./test/models/obj/teapot.obj", &teapot_mesh);
  generate_normals(&teapot_mesh);

  fprintf(stderr, "Loaded mesh: %zu vertices, %zu indices\n",
          *teapot_mesh.vert_count, *teapot_mesh.idx_count);

  program = make_program_from_files(
    "./game/assets/shaders/phong.vert",
    "./game/assets/shaders/phong.frag"
  );
  glUseProgram(program);

  model_loc = glGetUniformLocation(program, "uModel");
  view_loc = glGetUniformLocation(program, "uView");
  proj_loc = glGetUniformLocation(program, "uProj");
  normal_loc = glGetUniformLocation(program, "uNormalMat");
  light_pos_loc = glGetUniformLocation(program, "uLightPos");
  view_pos_loc = glGetUniformLocation(program, "uViewPos");
  light_color_loc = glGetUniformLocation(program, "uLightColor");
  object_color_loc = glGetUniformLocation(program, "uObjectColor");

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  scene_init(&game_scene);

  teapot_entity = scene_create_entity(&game_scene);
  transform_component *t = scene_add_transform(&game_scene, teapot_entity);
  t->position = (vec3){0, 0, 0};
  t->rotation = (vec3){0, 0, 0};
  t->scale = (vec3){1, 1, 1};
  t->dirty = true;

  mesh_renderer_component *mr = scene_add_mesh_renderer(&game_scene, teapot_entity);
  mr->mesh_data = &teapot_mesh;

  glGenVertexArrays(1, &mr->vao);
  glBindVertexArray(mr->vao);

  glGenBuffers(1, &mr->vbo_pos);
  glBindBuffer(GL_ARRAY_BUFFER, mr->vbo_pos);
  glBufferData(GL_ARRAY_BUFFER,
               3 * (*teapot_mesh.vert_count) * sizeof(float),
               teapot_mesh.positions,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glGenBuffers(1, &mr->vbo_norm);
  glBindBuffer(GL_ARRAY_BUFFER, mr->vbo_norm);
  glBufferData(GL_ARRAY_BUFFER,
               3 * (*teapot_mesh.vert_count) * sizeof(float),
               teapot_mesh.normals,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  glGenBuffers(1, &mr->ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mr->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               (*teapot_mesh.idx_count) * sizeof(uint32_t),
               teapot_mesh.indices,
               GL_STATIC_DRAW);

  glBindVertexArray(0);
  mr->initialized = true;

  vec3 bb_min = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
  vec3 bb_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

  for (size_t i = 0; i < *teapot_mesh.vert_count; i++) {
    vec3 p = {
      teapot_mesh.positions[3*i + 0],
      teapot_mesh.positions[3*i + 1],
      teapot_mesh.positions[3*i + 2]
    };
    bb_min = vec3_min(bb_min, p);
    bb_max = vec3_max(bb_max, p);
  }

  vec3 center = vec_scale(vec_sum(bb_min, bb_max), 0.5f);
  vec3 diag = vec_sum(bb_max, vec_negate(bb_min));
  float radius = 0.5f * vec_length(diag);

  float fov = to_radians(45.0f);
  float cam_d = (radius * 1.5f) / tanf(fov * 0.5f);

  camera_entity = scene_create_entity(&game_scene);
  transform_component *cam_t = scene_add_transform(&game_scene, camera_entity);
  cam_t->position = (vec3){0, 0, cam_d};
  cam_t->dirty = true;

  camera_component *cam = scene_add_camera(&game_scene, camera_entity);
  cam->fov = fov;
  cam->aspect = (float)width / (float)height;
  cam->near_plane = 0.1f;
  cam->far_plane = cam_d + radius * 2.0f;
  cam->view_matrix = look_at(
    (vec3){ 0.0f, 0.0f, cam_d },
    (vec3){ 0.0f, 0.0f, 0.0f },
    (vec3){ 0.0f, 1.0f, 0.0f }
  );
  cam->projection_matrix = perspective_mat4(
    fov,
    (float)width / (float)height,
    0.1f,
    cam_d + radius * 2.0f
  );

  game_scene.active_camera = camera_entity;

  light_entity = scene_create_entity(&game_scene);
  light_component *light = scene_add_light(&game_scene, light_entity);
  light->position = (vec3){-5.0f, 5.0f, 5.0f};
  light->color = (vec3){3.0f, 3.0f, 3.0f};
  light->intensity = 1.0f;

  vec3 object_color = { 1.0f, 0.75f, 0.2f };
  glUseProgram(program);
  glUniform3fv(light_pos_loc, 1, &light->position.x);
  glUniform3fv(view_pos_loc, 1, &cam_t->position.x);
  glUniform3fv(light_color_loc, 1, &light->color.x);
  glUniform3fv(object_color_loc, 1, &object_color.x);

  camera_target = (vec3){0, 0, 0};
  camera_controller_init(&cam_controller, &game_scene, camera_entity);

  input_bind_key(ATOM_KEY_W, NULL, cam_move_forward, NULL);
  input_bind_key(ATOM_KEY_S, NULL, cam_move_backward, NULL);
  input_bind_key(ATOM_KEY_A, NULL, cam_move_left, NULL);
  input_bind_key(ATOM_KEY_D, NULL, cam_move_right, NULL);
  input_bind_key(ATOM_KEY_SPACE, NULL, cam_move_up, NULL);
  input_bind_key(ATOM_KEY_Q, NULL, cam_move_down, NULL);
}

static void cam_move_forward(float dt) {
  camera_controller_move_forward(&cam_controller, dt);
  float speed = cam_controller.move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) speed *= cam_controller.sprint_multiplier;
  camera_target.z -= speed;
}

static void cam_move_backward(float dt) {
  camera_controller_move_backward(&cam_controller, dt);
  float speed = cam_controller.move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) speed *= cam_controller.sprint_multiplier;
  camera_target.z += speed;
}

static void cam_move_left(float dt) {
  camera_controller_move_left(&cam_controller, dt);
  float speed = cam_controller.move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) speed *= cam_controller.sprint_multiplier;
  camera_target.x -= speed;
}

static void cam_move_right(float dt) {
  camera_controller_move_right(&cam_controller, dt);
  float speed = cam_controller.move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) speed *= cam_controller.sprint_multiplier;
  camera_target.x += speed;
}

static void cam_move_up(float dt) {
  camera_controller_move_up(&cam_controller, dt);
  float speed = cam_controller.move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) speed *= cam_controller.sprint_multiplier;
  camera_target.y += speed;
}

static void cam_move_down(float dt) {
  camera_controller_move_down(&cam_controller, dt);
  float speed = cam_controller.move_speed * dt;
  if (input_is_key_pressed(ATOM_KEY_SHIFT)) speed *= cam_controller.sprint_multiplier;
  camera_target.y -= speed;
}

void game_update(float dt) {

  transform_component *t = scene_get_transform(&game_scene, teapot_entity);
  if (t) {
    t->rotation.y += dt;
    t->dirty = true;
  }

  transform_component *cam_t = scene_get_transform(&game_scene, camera_entity);
  camera_component *cam = scene_get_camera(&game_scene, camera_entity);
  if (cam_t && cam) {
    cam->view_matrix = look_at(
      cam_t->position,
      camera_target,
      (vec3){0, 1, 0}
    );
  }

  scene_update_transforms(&game_scene);
}

void game_render(void) {
  glViewport(0, 0, width, height);
  glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);
  scene_render(&game_scene);
}

void game_cleanup(void) {
  scene_destroy(&game_scene);
  destroy_mesh(&teapot_mesh);
}

int main(void) {
  atom_config config = {
    .title = "Atom Game",
    .width = 1080,
    .height = 1080
  };

  atom_callbacks callbacks = {
    .init = game_init,
    .update = game_update,
    .render = game_render,
    .cleanup = game_cleanup
  };

  return atom_run(&config, &callbacks);
}
