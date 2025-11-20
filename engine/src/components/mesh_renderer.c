#include <components/mesh_renderer.h>
#include <opengl/glad.h>
#include <string.h>

void mesh_renderer_component_init(mesh_renderer_component *mr, entity_id id) {
  memset(mr, 0, sizeof(mesh_renderer_component));
  mr->entity = id;
  mr->initialized = false;
}

void mesh_renderer_component_cleanup(mesh_renderer_component *mr) {
  if (mr->initialized) {
    glDeleteBuffers(1, &mr->ebo);
    glDeleteBuffers(1, &mr->vbo_norm);
    glDeleteBuffers(1, &mr->vbo_pos);
    glDeleteVertexArrays(1, &mr->vao);
    mr->initialized = false;
  }
}
