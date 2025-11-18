#include <stdio.h>
#include <float.h>

#include <GLES2/gl2.h>
#include <atom/engine.h>
#include <math/la.h>
#include <math/trig.h>
#include <mesh/mesh.h>
#include <opengl/opengl.h>

static mesh teapot;
static GLuint program;
static GLuint vao, vbo_pos, vbo_norm, ebo;
static GLint model_loc, view_loc, proj_loc, normal_loc;
static GLint light_pos_loc, view_pos_loc, light_color_loc, object_color_loc;

static mat4 view, proj;
static vec3 center;
static float cam_d;
static float angle = 0.0f;

extern int width, height;

void game_init(void) {
    load_mesh("./test/models/obj/teapot.obj", &teapot);
    generate_normals(&teapot);

    fprintf(stderr, "Loaded mesh: %zu vertices, %zu indices\n",
            *teapot.vert_count, *teapot.idx_count);

    vec3 bb_min = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
    vec3 bb_max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (size_t i = 0; i < *teapot.vert_count; i++) {
        vec3 p = {
            teapot.positions[3*i + 0],
            teapot.positions[3*i + 1],
            teapot.positions[3*i + 2]
        };
        bb_min = vec3_min(bb_min, p);
        bb_max = vec3_max(bb_max, p);
    }

    center = vec_scale(vec_sum(bb_min, bb_max), 0.5f);
    vec3 diag = vec_sum(bb_max, vec_negate(bb_min));
    float radius = 0.5f * vec_length(diag);

    float fov = to_radians(45.0f);
    cam_d = (radius * 1.5f) / tanf(fov * 0.5f);

    view = look_at(
        (vec3){ 0.0f, 0.0f,  cam_d },
        (vec3){ 0.0f, 0.0f,  0.0f },
        (vec3){ 0.0f, 1.0f,  0.0f }
    );
    proj = perspective_mat4(
        fov,
        (float)width / (float)height,
        0.1f,
        cam_d + radius * 2.0f
    );

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

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo_pos);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
    glBufferData(GL_ARRAY_BUFFER,
                 3 * (*teapot.vert_count) * sizeof(float),
                 teapot.positions,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &vbo_norm);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_norm);
    glBufferData(GL_ARRAY_BUFFER,
                 3 * (*teapot.vert_count) * sizeof(float),
                 teapot.normals,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (*teapot.idx_count) * sizeof(uint32_t),
                 teapot.indices,
                 GL_STATIC_DRAW);

    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    vec3 light_pos    = { -5.0f, 5.0f, 5.0f };
    vec3 camera_pos   = { 0.0f, 0.0f, cam_d };
    vec3 light_color  = { 3.0f, 3.0f, 3.0f };
    vec3 object_color = { 1.0f, 0.75f, 0.2f };

    glUseProgram(program);
    glUniform3fv(light_pos_loc, 1, &light_pos.x);
    glUniform3fv(view_pos_loc, 1, &camera_pos.x);
    glUniform3fv(light_color_loc, 1, &light_color.x);
    glUniform3fv(object_color_loc, 1, &object_color.x);
}

void game_update(float dt) {
    angle += dt;
}

void game_render(void) {
    mat4 model = mat4_identity();
    model = translate_mat4(model, vec_negate(center));
    model = rotate_mat4(model, angle, (vec3){ 0.0f, 1.0f, 0.0f });

    mat4 normal_mat = mat4_transpose(mat4_inverse(model));

    glViewport(0, 0, width, height);
    glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);
    glUniformMatrix4fv(model_loc, 1, GL_TRUE, &model.m[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_TRUE, &view.m[0][0]);
    glUniformMatrix4fv(proj_loc, 1, GL_TRUE, &proj.m[0][0]);
    glUniformMatrix4fv(normal_loc, 1, GL_TRUE, &normal_mat.m[0][0]);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, (GLsizei) *teapot.idx_count, GL_UNSIGNED_INT, 0);
}

void game_cleanup(void) {
    destroy_mesh(&teapot);
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo_norm);
    glDeleteBuffers(1, &vbo_pos);
    glDeleteVertexArrays(1, &vao);
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
