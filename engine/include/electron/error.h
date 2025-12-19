#ifndef ELECTRON_ERROR_H
#define ELECTRON_ERROR_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    ERR_NOTE,
    ERR_WARNING,
    ERR_ERROR,
} error_severity;

typedef struct {
    error_severity severity;
    uint32_t line;
    uint32_t column;
    uint32_t length;         // Length of the offending token/span
    const char *message;     // Owned, must be freed
    const char *source_line; // Pointer into source (not owned)
} electron_error;

typedef struct {
    electron_error *errors;
    size_t count;
    size_t capacity;

    const char *source;      // Full source text (not owned)
    const char *filename;    // Source filename (not owned)

    size_t error_count;      // Count of ERR_ERROR severity
    size_t warning_count;    // Count of ERR_WARNING severity
} error_context;

void error_context_init(error_context *ctx, const char *source, const char *filename);
void error_context_free(error_context *ctx);

void error_emit(error_context *ctx, error_severity severity,
                uint32_t line, uint32_t column, uint32_t length,
                const char *fmt, ...);

void error_print_all(error_context *ctx);
bool error_has_errors(error_context *ctx);

#endif
