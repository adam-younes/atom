#ifndef ATOM_LEXER
#define ATOM_LEXER 

#include <script/lexer.h>
#include <stdio.h>
#include <stdlib.h>

static char *load_electron_script(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) {
      fprintf(stderr, "Failed to open script: %s\n", path);
      return NULL;
  }
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *buf = malloc(len + 1);
  fread(buf, 1, len, f);
  buf[len] = '\0';
  fclose(f);
  return buf;
}

char **lex(const char *path) {

}

#endif // ATOM_LEXER
