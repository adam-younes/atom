#ifndef ATOM_ENGINE_H
#define ATOM_ENGINE_H

typedef struct {
  void (*init)(void);
  void (*update)(float dt);
  void (*render)(void);
  void (*cleanup)(void);
} atom_callbacks;

typedef struct {
  const char *title;
  int width;
  int height;
} atom_config;

int atom_run(atom_config *config, atom_callbacks *callbacks);

#endif
