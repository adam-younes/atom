#include "lexer_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declaration - to be implemented in lexer.c
extern token_list lex_string(const char *source, const char *filename, error_context *err_ctx);

const char *test_status_str(test_status status) {
    switch (status) {
        case TEST_PASS:  return ANSI_GREEN "PASS" ANSI_RESET;
        case TEST_FAIL:  return ANSI_RED "FAIL" ANSI_RESET;
        case TEST_SKIP:  return ANSI_YELLOW "SKIP" ANSI_RESET;
        case TEST_ERROR: return ANSI_MAGENTA "ERROR" ANSI_RESET;
        default:         return "UNKNOWN";
    }
}

test_result run_lexer_test(const lexer_test_case *test) {
    test_result result = {
        .status = TEST_PASS,
        .name = test->name,
        .message = NULL,
        .expected_count = test->expected_count,
        .actual_count = 0,
        .first_mismatch = 0
    };

    error_context err_ctx;
    error_context_init(&err_ctx, test->input, test->name);

    token_list tokens = lex_string(test->input, test->name, &err_ctx);
    result.actual_count = tokens.count;

    if (test->expect_error) {
        if (!error_has_errors(&err_ctx)) {
            result.status = TEST_FAIL;
            result.message = "Expected error but lexing succeeded";
            goto cleanup;
        }
        if (test->expected_error_msg != NULL) {
            bool found = false;
            for (size_t i = 0; i < err_ctx.count; i++) {
                if (strstr(err_ctx.errors[i].message, test->expected_error_msg)) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                result.status = TEST_FAIL;
                result.message = "Error message mismatch";
            }
        }
        goto cleanup;
    }

    // Check for unexpected errors
    if (error_has_errors(&err_ctx)) {
        result.status = TEST_FAIL;
        result.message = "Unexpected lexer error";
        goto cleanup;
    }

    // Check token count
    if (tokens.count != test->expected_count) {
        result.status = TEST_FAIL;
        result.message = "Token count mismatch";
        goto cleanup;
    }

    // Check each token type
    for (size_t i = 0; i < test->expected_count; i++) {
        if (tokens.tokens[i].type != test->expected_types[i]) {
            result.status = TEST_FAIL;
            result.message = "Token type mismatch";
            result.first_mismatch = i;
            goto cleanup;
        }
    }

cleanup:
    token_list_free(&tokens);
    error_context_free(&err_ctx);
    return result;
}

void run_test_suite(test_suite *suite) {
    printf("\n" ANSI_BOLD "Running suite: %s" ANSI_RESET " (%zu tests)\n",
           suite->name, suite->case_count);
    printf("─────────────────────────────────────────\n");

    suite->passed = suite->failed = suite->skipped = suite->errors = 0;

    for (size_t i = 0; i < suite->case_count; i++) {
        test_result result = run_lexer_test(&suite->cases[i]);
        print_test_result(&result, false);

        switch (result.status) {
            case TEST_PASS:  suite->passed++;  break;
            case TEST_FAIL:  suite->failed++;  break;
            case TEST_SKIP:  suite->skipped++; break;
            case TEST_ERROR: suite->errors++;  break;
        }
    }

    print_suite_summary(suite);
}

void print_test_result(const test_result *result, bool verbose) {
    printf("  [%s] %s", test_status_str(result->status), result->name);

    if (result->status != TEST_PASS && result->message) {
        printf(" - %s", result->message);
        if (result->status == TEST_FAIL) {
            if (result->expected_count != result->actual_count) {
                printf(" (expected %zu tokens, got %zu)",
                       result->expected_count, result->actual_count);
            } else if (result->first_mismatch > 0) {
                printf(" (at token %zu)", result->first_mismatch);
            }
        }
    }
    printf("\n");

    if (verbose && result->status == TEST_FAIL) {
        // Print detailed token comparison
        printf("    Expected tokens: %zu\n", result->expected_count);
        printf("    Actual tokens:   %zu\n", result->actual_count);
    }
}

void print_suite_summary(const test_suite *suite) {
    printf("─────────────────────────────────────────\n");
    printf("Suite %s: ", suite->name);
    printf(ANSI_GREEN "%zu passed" ANSI_RESET, suite->passed);
    if (suite->failed > 0) {
        printf(", " ANSI_RED "%zu failed" ANSI_RESET, suite->failed);
    }
    if (suite->skipped > 0) {
        printf(", " ANSI_YELLOW "%zu skipped" ANSI_RESET, suite->skipped);
    }
    if (suite->errors > 0) {
        printf(", " ANSI_MAGENTA "%zu errors" ANSI_RESET, suite->errors);
    }
    printf("\n");
}

void print_final_stats(const test_stats *stats) {
    printf("\n");
    printf("═══════════════════════════════════════════\n");
    printf(ANSI_BOLD "          TEST SUMMARY" ANSI_RESET "\n");
    printf("═══════════════════════════════════════════\n");
    printf("  Suites:  %zu\n", stats->total_suites);
    printf("  Cases:   %zu\n", stats->total_cases);
    printf("───────────────────────────────────────────\n");
    printf("  " ANSI_GREEN "Passed:  %zu" ANSI_RESET "\n", stats->passed);
    if (stats->failed > 0) {
        printf("  " ANSI_RED "Failed:  %zu" ANSI_RESET "\n", stats->failed);
    }
    if (stats->skipped > 0) {
        printf("  " ANSI_YELLOW "Skipped: %zu" ANSI_RESET "\n", stats->skipped);
    }
    if (stats->errors > 0) {
        printf("  " ANSI_MAGENTA "Errors:  %zu" ANSI_RESET "\n", stats->errors);
    }
    printf("═══════════════════════════════════════════\n");

    if (stats->failed == 0 && stats->errors == 0) {
        printf(ANSI_GREEN ANSI_BOLD "  ALL TESTS PASSED!" ANSI_RESET "\n");
    } else {
        printf(ANSI_RED ANSI_BOLD "  SOME TESTS FAILED" ANSI_RESET "\n");
    }
    printf("═══════════════════════════════════════════\n\n");
}

// Load file contents
static char *load_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buf = malloc(len + 1);
    if (!buf) {
        fclose(f);
        return NULL;
    }

    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

test_result run_lexer_test_file(const char *script_path, const char *expect_path) {
    test_result result = {
        .status = TEST_ERROR,
        .name = script_path,
        .message = NULL,
        .expected_count = 0,
        .actual_count = 0,
        .first_mismatch = 0
    };

    char *source = load_file(script_path);
    if (!source) {
        result.message = "Failed to load script file";
        return result;
    }

    error_context err_ctx;
    error_context_init(&err_ctx, source, script_path);

    token_list tokens = lex_string(source, script_path, &err_ctx);
    result.actual_count = tokens.count;

    // If expect file provided, compare against it
    if (expect_path) {
        char *expected = load_file(expect_path);
        if (!expected) {
            result.message = "Failed to load expect file";
            goto cleanup;
        }
        // TODO: Parse expect file and compare
        free(expected);
    }

    // Print tokens for inspection
    printf("Tokens from %s:\n", script_path);
    token_list_print(&tokens);

    if (error_has_errors(&err_ctx)) {
        printf("\nErrors:\n");
        error_print_all(&err_ctx);
        result.status = TEST_FAIL;
        result.message = "Lexer errors occurred";
    } else {
        result.status = TEST_PASS;
    }

cleanup:
    token_list_free(&tokens);
    error_context_free(&err_ctx);
    free(source);
    return result;
}
