#include <mesh/mesh.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math/la.h>

typedef void (*mesh_loader)(const char *, mesh *); 

static const struct {
  const char  *ext;
  mesh_loader fun;
} loaders[] = {
  { "obj", load_obj },
  { NULL , NULL     }
};

void load_mesh(const char *path, mesh *out) {
  const char *dot = strrchr(path, '.');
  if (!dot || dot == path) {
    fprintf(stderr, "load_mesh: no valid extension on '%s'\n", path);
    return;
  }

  const char *ext = dot + 1;

  uint8_t i;
  for (i = 0 ; loaders[i].ext ; i++) {
    if (strcmp(ext, loaders[i].ext) == 0) {
      loaders[i].fun(path, out); 
      return;
    }
  }

  fprintf(stderr, "Unsupported file format '.%s'\n", ext);
}

void generate_normals_smooth(mesh *m) {
  if (!m->positions || !m->indices || !m->vert_count || !m->idx_count) {
    return;
  }

  size_t vc = *m->vert_count;
  size_t ic = *m->idx_count;

  if (m->normals) free(m->normals);
  m->normals = calloc(vc * 3, sizeof(float));

  for (size_t i = 0; i < ic; i += 3) {
    uint32_t i0 = m->indices[i + 0];
    uint32_t i1 = m->indices[i + 1];
    uint32_t i2 = m->indices[i + 2];

    vec3 v0 = { m->positions[3*i0+0], m->positions[3*i0+1], m->positions[3*i0+2] };
    vec3 v1 = { m->positions[3*i1+0], m->positions[3*i1+1], m->positions[3*i1+2] };
    vec3 v2 = { m->positions[3*i2+0], m->positions[3*i2+1], m->positions[3*i2+2] };

    vec3 e1 = vec_sum(v1, vec_negate(v0));
    vec3 e2 = vec_sum(v2, vec_negate(v0));
    vec3 n = vec_cross(e1, e2);

    m->normals[3*i0+0] += n.x; m->normals[3*i0+1] += n.y; m->normals[3*i0+2] += n.z;
    m->normals[3*i1+0] += n.x; m->normals[3*i1+1] += n.y; m->normals[3*i1+2] += n.z;
    m->normals[3*i2+0] += n.x; m->normals[3*i2+1] += n.y; m->normals[3*i2+2] += n.z;
  }

  float *averaged_normals = calloc(vc * 3, sizeof(float));

  for (size_t i = 0; i < vc; i++) {
    vec3 pos_i = { m->positions[3*i+0], m->positions[3*i+1], m->positions[3*i+2] };
    vec3 normal_sum = { m->normals[3*i+0], m->normals[3*i+1], m->normals[3*i+2] };

    for (size_t j = 0; j < vc; j++) {
      if (i == j) continue;

      vec3 pos_j = { m->positions[3*j+0], m->positions[3*j+1], m->positions[3*j+2] };
      vec3 diff = vec_sum(pos_i, vec_negate(pos_j));
      float dist_sq = vec_dot(diff, diff);

      if (dist_sq < 1e-6f) {
        normal_sum.x += m->normals[3*j+0];
        normal_sum.y += m->normals[3*j+1];
        normal_sum.z += m->normals[3*j+2];
      }
    }

    averaged_normals[3*i+0] = normal_sum.x;
    averaged_normals[3*i+1] = normal_sum.y;
    averaged_normals[3*i+2] = normal_sum.z;
  }

  for (size_t i = 0; i < vc; i++) {
    vec3 n = { averaged_normals[3*i+0], averaged_normals[3*i+1], averaged_normals[3*i+2] };
    n = vec_normalize(n);
    m->normals[3*i+0] = n.x;
    m->normals[3*i+1] = n.y;
    m->normals[3*i+2] = n.z;
  }

  free(averaged_normals);
}

void generate_normals_flat(mesh *m) {
  if (!m->positions || !m->indices || !m->vert_count || !m->idx_count) {
    return;
  }

  size_t vc = *m->vert_count;
  size_t ic = *m->idx_count;

  if (m->normals) free(m->normals);
  m->normals = calloc(vc * 3, sizeof(float));

  for (size_t i = 0; i < ic; i += 3) {
    uint32_t i0 = m->indices[i + 0];
    uint32_t i1 = m->indices[i + 1];
    uint32_t i2 = m->indices[i + 2];

    vec3 v0 = { m->positions[3*i0+0], m->positions[3*i0+1], m->positions[3*i0+2] };
    vec3 v1 = { m->positions[3*i1+0], m->positions[3*i1+1], m->positions[3*i1+2] };
    vec3 v2 = { m->positions[3*i2+0], m->positions[3*i2+1], m->positions[3*i2+2] };

    vec3 e1 = vec_sum(v1, vec_negate(v0));
    vec3 e2 = vec_sum(v2, vec_negate(v0));
    vec3 n = vec_normalize(vec_cross(e1, e2));

    m->normals[3*i0+0] = n.x; m->normals[3*i0+1] = n.y; m->normals[3*i0+2] = n.z;
    m->normals[3*i1+0] = n.x; m->normals[3*i1+1] = n.y; m->normals[3*i1+2] = n.z;
    m->normals[3*i2+0] = n.x; m->normals[3*i2+1] = n.y; m->normals[3*i2+2] = n.z;
  }
}

void generate_normals(mesh *m) {
  generate_normals_smooth(m);
}

void destroy_mesh(mesh *m) {
  free(m->positions);
  free(m->normals);
  free(m->texcoords);
  free(m->indices);
  free(m->vert_count);
  free(m->idx_count);
}
