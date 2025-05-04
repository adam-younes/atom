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
    { "glb", load_glb },
};

void load_mesh(const char *path, mesh *out) {
    const char *dot = strrchr(path, '.');
    if (!dot || dot == path) {
        fprintf(stderr, "load_mesh: no valid extension on '%s'\n", path);
        return;
    }

    const char *ext = dot + 1;

    if (strcmp(ext, "obj") == 0) {
        load_obj(path, out);
    } else if {
        load_glb(path, out);
    } else {
        fprintf(stderr, "load_mesh: unsupported mesh format '%s'\n", ext);
    }
}

void load_obj(const char *path, mesh *out) {

}

void load_glb(const char *path, mesh *out) {
    
}

void destroy_mesh(mesh *m) {

}
