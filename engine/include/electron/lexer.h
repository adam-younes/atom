#ifndef ELECTRON_LEXER_H 
#define ELECTRON_LEXER_H

#include <electron/token.h>
#include <electron/error.h>

typedef struct { 
  token_list tokens;
} lexer;

void lexer_init(lexer *lexer); 
token_list lex_string(const char *source, const char *filename, error_context *err_ctx);
void lexer_free(lexer *lexer);

#endif // ELECTRON_LEXER_H
