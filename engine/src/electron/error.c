#include <electron/error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define INITIAL_CAPACITY 8

void error_context_init(error_context *ctx, const char *source, const char *filename) {
    ctx->errors = NULL;
    ctx->count = 0;
    ctx->capacity = 0;
    ctx->source = source;
    ctx->filename = filename;
    ctx->error_count = 0;
    ctx->warning_count = 0;
}

void error_context_free(error_context *ctx) {
    for (size_t i = 0; i < ctx->count; i++) {
        free((void *)ctx->errors[i].message);
    }
    free(ctx->errors);
    ctx->errors = NULL;
    ctx->count = 0;
    ctx->capacity = 0;
}

static const char *find_line_start(const char *source, uint32_t line) {
    const char *p = source;
    uint32_t current_line = 1;

    while (*p && current_line < line) {
        if (*p == '\n') current_line++;
        p++;
    }

    return p;
}

static size_t line_length(const char *line_start) {
    const char *end = line_start;
    while (*end && *end != '\n') end++;
    return end - line_start;
}

void error_emit(error_context *ctx, error_severity severity,
                uint32_t line, uint32_t column, uint32_t length,
                const char *fmt, ...) {
    if (ctx->count >= ctx->capacity) {
        ctx->capacity = ctx->capacity == 0 ? INITIAL_CAPACITY : ctx->capacity * 2;
        ctx->errors = realloc(ctx->errors, ctx->capacity * sizeof(electron_error));
    }

    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(NULL, 0, fmt, args);
    va_end(args);

    char *message = malloc(len + 1);
    va_start(args, fmt);
    vsnprintf(message, len + 1, fmt, args);
    va_end(args);

    electron_error *err = &ctx->errors[ctx->count++];
    err->severity = severity;
    err->line = line;
    err->column = column;
    err->length = length > 0 ? length : 1;
    err->message = message;
    err->source_line = find_line_start(ctx->source, line);

    if (severity == ERR_ERROR) ctx->error_count++;
    else if (severity == ERR_WARNING) ctx->warning_count++;
}

static const char *severity_str(error_severity s) {
    switch (s) {
        case ERR_NOTE:    return "note";
        case ERR_WARNING: return "warning";
        case ERR_ERROR:   return "error";
    }
    return "unknown";
}

static const char *severity_color(error_severity s) {
    switch (s) {
        case ERR_NOTE:    return "\033[36m";  // Cyan
        case ERR_WARNING: return "\033[33m";  // Yellow
        case ERR_ERROR:   return "\033[31m";  // Red
    }
    return "\033[0m";
}

void error_print_all(error_context *ctx) {
    const char *reset = "\033[0m";
    const char *bold = "\033[1m";

    for (size_t i = 0; i < ctx->count; i++) {
        electron_error *err = &ctx->errors[i];
        const char *color = severity_color(err->severity);

        fprintf(stderr, "%s%s:%u:%u: %s%s%s:%s %s\n",
                bold,
                ctx->filename ? ctx->filename : "<source>",
                err->line,
                err->column,
                color,
                severity_str(err->severity),
                reset,
                bold,
                err->message);
        fprintf(stderr, "%s", reset);

        if (err->source_line) {
            size_t len = line_length(err->source_line);
            fprintf(stderr, " %5u | %.*s\n", err->line, (int)len, err->source_line);

            fprintf(stderr, "       | ");
            for (uint32_t j = 1; j < err->column; j++) {
                char c = err->source_line[j - 1];
                fputc(c == '\t' ? '\t' : ' ', stderr);
            }

            fprintf(stderr, "%s", color);
            fputc('^', stderr);
            for (uint32_t j = 1; j < err->length; j++) {
                fputc('~', stderr);
            }
            fprintf(stderr, "%s\n", reset);
        }
    }

    if (ctx->error_count > 0 || ctx->warning_count > 0) {
        fprintf(stderr, "\n");
        if (ctx->error_count > 0) {
            fprintf(stderr, "%s%zu error%s%s",
                    severity_color(ERR_ERROR),
                    ctx->error_count,
                    ctx->error_count == 1 ? "" : "s",
                    reset);
        }
        if (ctx->error_count > 0 && ctx->warning_count > 0) {
            fprintf(stderr, " and ");
        }
        if (ctx->warning_count > 0) {
            fprintf(stderr, "%s%zu warning%s%s",
                    severity_color(ERR_WARNING),
                    ctx->warning_count,
                    ctx->warning_count == 1 ? "" : "s",
                    reset);
        }
        fprintf(stderr, " generated.\n");
    }
}

bool error_has_errors(error_context *ctx) {
    return ctx->error_count > 0;
}
