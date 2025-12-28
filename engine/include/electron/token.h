#ifndef ELECTRON_TOKEN_H
#define ELECTRON_TOKEN_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
  // Special tokens
  TOK_EOF = 0,
  TOK_ERROR,
  TOK_NEWLINE,

  // Literals
  TOK_IDENTIFIER,
  TOK_INT_LITERAL,
  TOK_FLOAT_LITERAL,
  TOK_STRING_LITERAL,
  TOK_RAW_STRING_LITERAL,
  TOK_MULTILINE_STRING_LITERAL,
  TOK_BOOL_LITERAL,

  // Keywords - Core types
  TOK_KW_BOOL,
  TOK_KW_INT,
  TOK_KW_FLOAT,
  TOK_KW_STRING,
  TOK_KW_VOID,

  // Keywords - Vector/Matrix types
  TOK_KW_VEC2,
  TOK_KW_VEC3,
  TOK_KW_VEC4,
  TOK_KW_MAT2,
  TOK_KW_MAT3,
  TOK_KW_MAT4,

  // Keywords - Collection types
  TOK_KW_LIST,
  TOK_KW_DICT,
  TOK_KW_RANGE,
  TOK_KW_TUPLE,
  TOK_KW_RESULT,
  TOK_KW_CORO,
  TOK_KW_FUNC,

  // Keywords - User-defined types
  TOK_KW_TYPE,
  TOK_KW_STRUCT,
  TOK_KW_CORE,
  TOK_KW_SHELL,
  TOK_KW_ENUM,
  TOK_KW_ENTITY,
  TOK_KW_NEVER,

  // Keywords - Declarations
  TOK_KW_CONST,
  TOK_KW_DEF,
  TOK_KW_MUT,
  TOK_KW_READONLY,
  TOK_KW_PUBLIC,
  TOK_KW_LOCAL,
  TOK_KW_EXPORT,
  TOK_KW_IMPORT,
  TOK_KW_FROM,

  // Keywords - Control flow
  TOK_KW_IF,
  TOK_KW_ELSE,
  TOK_KW_THEN,
  TOK_KW_WHILE,
  TOK_KW_DO,
  TOK_KW_LOOP,
  TOK_KW_FOR,
  TOK_KW_IN,
  TOK_KW_BREAK,
  TOK_KW_CONTINUE,
  TOK_KW_RETURN,
  TOK_KW_MATCH,
  TOK_KW_DEFER,

  // Keywords - Logical/Type operators
  TOK_KW_IS,
  TOK_KW_NOT,
  TOK_KW_AS,
  TOK_KW_AND,
  TOK_KW_OR,

  // Keywords - Error handling
  TOK_KW_SUCCESS,
  TOK_KW_ERROR,
  TOK_KW_ALLOWS,

  // Keywords - Coroutines
  TOK_KW_SPAWN,
  TOK_KW_YIELD,
  TOK_KW_YIELD_REQUEST,

  // Keywords - Component system
  TOK_KW_INNER,
  TOK_KW_OUTER,
  TOK_KW_SIBLING,
  TOK_KW_SIBLING_ALL,
  TOK_KW_REQUIRES,
  TOK_KW_ON_CANCEL,
  TOK_KW_SEALED,
  TOK_KW_UNIQUE,
  TOK_KW_STRICT,

  // Keywords - Reserved for future
  TOK_KW_ASYNC,
  TOK_KW_AWAIT,
  TOK_KW_CLASS,
  TOK_KW_EXTENDS,
  TOK_KW_IMPLEMENTS,
  TOK_KW_NEW,
  TOK_KW_PRIVATE,
  TOK_KW_PROTECTED,
  TOK_KW_STATIC,
  TOK_KW_SUPER,
  TOK_KW_THIS,
  TOK_KW_THROW,
  TOK_KW_TRY,
  TOK_KW_CATCH,
  TOK_KW_FINALLY,
  TOK_KW_WITH,
  TOK_KW_SIZEOF,

  // Boolean literals
  TOK_KW_TRUE,
  TOK_KW_FALSE,
  TOK_KW_NULL,

  // Operators - Arithmetic
  TOK_PLUS,           // +
  TOK_MINUS,          // -
  TOK_STAR,           // *
  TOK_SLASH,          // /
  TOK_PERCENT,        // %

  // Operators - Comparison
  TOK_EQ_EQ,          // ==
  TOK_BANG_EQ,        // !=
  TOK_LT,             // <
  TOK_GT,             // >
  TOK_LT_EQ,          // <=
  TOK_GT_EQ,          // >=

  // Operators - Logical
  TOK_AMP_AMP,        // &&
  TOK_PIPE_PIPE,      // ||
  TOK_BANG,           // !

  // Operators - Bitwise
  TOK_AMP,            // &
  TOK_PIPE,           // |
  TOK_CARET,          // ^
  TOK_TILDE,          // ~
  TOK_LT_LT,          // <<
  TOK_GT_GT,          // >>

  // Operators - Assignment
  TOK_EQ,             // =
  TOK_PLUS_EQ,        // +=
  TOK_MINUS_EQ,       // -=
  TOK_STAR_EQ,        // *=
  TOK_SLASH_EQ,       // /=
  TOK_PERCENT_EQ,     // %=
  TOK_AMP_EQ,         // &=
  TOK_PIPE_EQ,        // |=
  TOK_CARET_EQ,       // ^=
  TOK_LT_LT_EQ,       // <<=
  TOK_GT_GT_EQ,       // >>=

  // Operators - Shell operations
  TOK_LT_MINUS,       // <- (push shell)
  TOK_MINUS_GT,       // -> (remove shell)
  TOK_LT_LT_MINUS,    // <<- (force push shell)
  TOK_MINUS_GT_GT,    // ->> (remove all instances)
  TOK_GT_MINUS,       // >- (query shell)

  // Operators - Special
  TOK_QUESTION_DOT,   // ?. (safe access)
  TOK_QUESTION_QUESTION, // ?? (null coalescing)
  TOK_QUESTION,       // ? (error propagation / nullable)
  TOK_DOT,            // .
  TOK_DOT_DOT,        // .. (exclusive range)
  TOK_DOT_DOT_EQ,     // ..= (inclusive range)
  TOK_ARROW,          // => (match arm / lambda)
  TOK_THIN_ARROW,     // -> (type arrow, same as MINUS_GT but context differs)
  TOK_COLON,          // :
  TOK_COLON_COLON,    // :: (path separator)
  TOK_SEMICOLON,      // ;
  TOK_COMMA,          // ,
  TOK_UNDERSCORE,     // _ (wildcard pattern)

  // Delimiters
  TOK_LPAREN,         // (
  TOK_RPAREN,         // )
  TOK_LBRACKET,       // [
  TOK_RBRACKET,       // ]
  TOK_LBRACE,         // {
  TOK_RBRACE,         // }

  // Annotations
  TOK_AT,             // @
  TOK_AT_COMPTIME,    // @comptime

  // Comments (if preserved)
  TOK_COMMENT_LINE,   // // ...
  TOK_COMMENT_BLOCK,  // /* ... */

  TOK_COUNT
} token_type;

typedef struct {
  token_type type;
  const char *start;      // Pointer to start of token in source
  size_t length;          // Length of token text
  uint32_t line;          // 1-based line number
  uint32_t column;        // 1-based column number

  // value storage for literals
  union {
    int64_t int_value;
    double float_value;
    bool bool_value;
    struct {
      char *data;     // Heap-allocated, processed string (escapes resolved)
      size_t len;
    } string_value;
  } value;
  bool has_value;
} token;

typedef struct {
  token *tokens;
  size_t count;
  size_t capacity;
} token_list;

const char *token_type_str(token_type type);

void token_list_init(token_list *list);
void token_list_push(token_list *list, token tok);
void token_list_free(token_list *list);

void token_print(const token *tok);
void token_list_print(const token_list *list);

#endif // ELECTRON_TOKEN_H
