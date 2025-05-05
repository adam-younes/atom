#include <mesh/mesh.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

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

void destroy_mesh(mesh *m) {
    free(m->positions);
    free(m->normals);
    free(m->texcoords);
    free(m->indices);
    free(m->vert_count);
    free(m->idx_count);
}
