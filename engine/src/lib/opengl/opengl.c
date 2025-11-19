#include <opengl/glad.h>
#include <EGL/egl.h>
#include <stdio.h>
#include <stdlib.h>

void init_glad(void) {
    if (!gladLoadGL((GLADloadfunc) eglGetProcAddress)) {
        fprintf(stderr, "ERROR: failed to initialize GLAD\n");
        exit(1);
    }
}
