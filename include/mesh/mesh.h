#ifndef ATOM_MESH_H
#define ATOM_MESH_H
#include <stdint.h>
#include <stdlib.h>

// struct for mesh data
typedef struct {
    float     *positions;
    float     *normals; 
    float     *texcoords; 
    uint32_t  *indices; 
    size_t    *vert_count;
    size_t    *idx_count;
} mesh;

void load_mesh(const char *path, mesh *out);

extern void load_obj(const char *path, mesh *out);

void destroy_mesh(mesh *m);

#endif
