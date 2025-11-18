#include <mesh/mesh.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <math/la.h>

void load_obj(const char *path, mesh *out)
__attribute__((alias("at_load_obj")));

typedef struct {
  uint32_t position_index;
  uint32_t texcoord_index;
  uint32_t normal_index;
} vertex_key;

typedef struct {
  vertex_key key;
  uint64_t   hash;
  uint32_t   index;
} hash_entry;

static uint64_t fnv1a_64(const void *data, size_t length) {
  const uint8_t *bytes = (const uint8_t *)data;
  uint64_t       h     = 14695981039346656037ULL;
  const uint64_t prime = 1099511628211ULL;
  for (size_t i = 0; i < length; i++) {
    h ^= (uint64_t)bytes[i];
    h *= prime;
  }

  return h + 1;
}

static bool ht_insert(hash_entry *table,
                      size_t        capacity,
                      vertex_key    key,
                      uint32_t     *out_index,
                      uint32_t     *unique_count)
{
  uint64_t hash = fnv1a_64(&key, sizeof(key));
  size_t   idx  = hash % capacity;
  size_t   step = 1;

  while (table[idx].hash) {
    if (table[idx].hash == hash
      && table[idx].key.position_index == key.position_index
      && table[idx].key.texcoord_index  == key.texcoord_index
      && table[idx].key.normal_index    == key.normal_index)
    {
      *out_index = table[idx].index;
      return false;
    }

    idx = (idx + step * step) % capacity;
    step++;
  }

  table[idx].hash  = hash;
  table[idx].key   = key;
  table[idx].index = *unique_count;
  *out_index       = (*unique_count)++;
  return true;
}

static void parse_face_token(const char *token,
                             size_t     *out_pindex,
                             size_t     *out_tindex,
                             size_t     *out_nindex)
{
  *out_pindex = 0;
  *out_tindex = 0;
  *out_nindex = 0;

  const char *slash1 = strchr(token, '/');
  if (!slash1) {
    *out_pindex = atoi(token);
    return;
  }

  *out_pindex = atoi(token);
  const char *slash2 = strchr(slash1 + 1, '/');

  if (!slash2) {
    *out_tindex = atoi(slash1 + 1);
  } else if (slash2 == slash1 + 1) {
    *out_nindex = atoi(slash2 + 1);
  } else {
    *out_tindex = atoi(slash1 + 1);
    *out_nindex = atoi(slash2 + 1);
  }
}

void at_load_obj(const char *path, mesh *out) {
  FILE   *file = fopen(path, "r");
  if (!file) {
    fprintf(stderr, "load_obj: cannot open '%s'\n", path);
    return;
  }

  // original vertex arrays
  size_t pos_capacity = 256, pos_count = 0;
  vec3  *positions    = malloc(pos_capacity * sizeof(vec3));

  size_t tc_capacity = 256, tc_count = 0;
  vec2  *texcoords   = malloc(tc_capacity * sizeof(vec2));

  size_t nm_capacity = 256, nm_count = 0;
  vec3  *normals     = malloc(nm_capacity * sizeof(vec3));

  // hash table and output arrays
  size_t     ht_capacity     = 1024;
  hash_entry *hash_table     = calloc(ht_capacity, sizeof(hash_entry));
  uint32_t    unique_vertices = 0;

  size_t out_capacity     = 256;
  size_t out_vertex_count = 0;
  float  *out_positions   = malloc(out_capacity * 3 * sizeof(float));
  float  *out_texcoords   = NULL;
  float  *out_normals     = NULL;

  size_t idx_capacity = 256, idx_count = 0;
  uint32_t *out_indices = malloc(idx_capacity * sizeof(uint32_t));

  char line[1024];
  while (fgets(line, sizeof(line), file)) {
    if (strncmp(line, "v ", 2) == 0) {
      // position
      if (pos_count >= pos_capacity) {
        pos_capacity *= 2;
        positions    = realloc(positions, pos_capacity * sizeof(vec3));
      }
      sscanf(line + 2, "%f %f %f",
             &positions[pos_count].x,
             &positions[pos_count].y,
             &positions[pos_count].z);
      pos_count++;

    } else if (strncmp(line, "vt ", 3) == 0) {
      // texcoord
      if (tc_count >= tc_capacity) {
        tc_capacity *= 2;
        texcoords    = realloc(texcoords, tc_capacity * sizeof(vec2));
      }
      sscanf(line + 3, "%f %f",
             &texcoords[tc_count].x,
             &texcoords[tc_count].y);
      tc_count++;

    } else if (strncmp(line, "vn ", 3) == 0) {
      // normal
      if (nm_count >= nm_capacity) {
        nm_capacity *= 2;
        normals     = realloc(normals, nm_capacity * sizeof(vec3));
      }
      sscanf(line + 3, "%f %f %f",
             &normals[nm_count].x,
             &normals[nm_count].y,
             &normals[nm_count].z);
      nm_count++;

    } else if (strncmp(line, "f ", 2) == 0) {
      // face
      char *token = strtok(line + 2, " \r\n");
      while (token) {
        size_t pidx, tidx, nidx;
        parse_face_token(token, &pidx, &tidx, &nidx);

        // handle negative indices
        if ((int)pidx < 0) pidx = pos_count + pidx + 1;
        if ((int)tidx < 0) tidx = tc_count  + tidx + 1;
        if ((int)nidx < 0) nidx = nm_count  + nidx + 1;

        vertex_key key = {
          .position_index = (uint32_t)pidx,
          .texcoord_index = (uint32_t)tidx,
          .normal_index   = (uint32_t)nidx
        };

        uint32_t vertex_index;
        if (unique_vertices * 2 > ht_capacity) {
          // save old entries
          size_t old_capacity = ht_capacity;
          hash_entry *old_table = hash_table;

          // allocate new table
          ht_capacity *= 2;
          hash_table = calloc(ht_capacity, sizeof(hash_entry));

          // rehash all existing entries
          for (size_t i = 0; i < old_capacity; i++) {
            if (old_table[i].hash) {
              uint64_t hash = old_table[i].hash;
              size_t idx = hash % ht_capacity;
              size_t step = 1;

              while (hash_table[idx].hash) {
                idx = (idx + step * step) % ht_capacity;
                step++;
              }

              hash_table[idx] = old_table[i];
            }
          }

          free(old_table);
        }

        bool is_new = ht_insert(hash_table,
                                ht_capacity,
                                key,
                                &vertex_index,
                                &unique_vertices);

        if (is_new) {
          // emit new vertex
          if (out_vertex_count >= out_capacity) {
            out_capacity   *= 2;
            out_positions  = realloc(out_positions,
                                     out_capacity * 3 * sizeof(float));
          }
          out_positions[3 * vertex_index + 0] =
            positions[key.position_index - 1].x;
          out_positions[3 * vertex_index + 1] =
            positions[key.position_index - 1].y;
          out_positions[3 * vertex_index + 2] =
            positions[key.position_index - 1].z;

          if (key.texcoord_index) {
            if (!out_texcoords) {
              out_texcoords = malloc(out_capacity * 2 * sizeof(float));
            }
            if (out_vertex_count >= out_capacity) {
              out_texcoords = realloc(out_texcoords,
                                      out_capacity * 2 * sizeof(float));
            }
            out_texcoords[2 * vertex_index + 0] =
              texcoords[key.texcoord_index - 1].x;
            out_texcoords[2 * vertex_index + 1] =
              texcoords[key.texcoord_index - 1].y;
          }

          if (key.normal_index) {
            if (!out_normals) {
              out_normals = malloc(out_capacity * 3 * sizeof(float));
            }
            if (out_vertex_count >= out_capacity) {
              out_normals = realloc(out_normals,
                                    out_capacity * 3 * sizeof(float));
            }
            out_normals[3 * vertex_index + 0] =
              normals[key.normal_index - 1].x;
            out_normals[3 * vertex_index + 1] =
              normals[key.normal_index - 1].y;
            out_normals[3 * vertex_index + 2] =
              normals[key.normal_index - 1].z;
          }

          out_vertex_count++;
        }

        // record index
        if (idx_count >= idx_capacity) {
          idx_capacity = idx_capacity * 2;
          out_indices  = realloc(out_indices,
                                 idx_capacity * sizeof(uint32_t));
        }
        out_indices[idx_count++] = vertex_index;

        token = strtok(NULL, " \r\n");
      }
    }
  }

  fclose(file);

  // populate mesh
  out->positions  = out_positions;
  out->texcoords  = out_texcoords;
  out->normals    = out_normals;
  out->indices    = out_indices;
  out->vert_count = malloc(sizeof(size_t));
  out->idx_count  = malloc(sizeof(size_t));
  *out->vert_count = out_vertex_count;
  *out->idx_count  = idx_count;

  // cleanup
  free(positions);
  free(texcoords);
  free(normals);
  free(hash_table);
}

