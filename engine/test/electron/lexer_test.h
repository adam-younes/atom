#ifndef ELECTRON_LEXER_TEST_H
#define ELECTRON_LEXER_TEST_H

#include <electron/token.h>
#include <electron/error.h>
#include <stdbool.h>
#include <stddef.h>

// Test result status
typedef enum {
    TEST_PASS,
    TEST_FAIL,
    TEST_SKIP,
    TEST_ERROR
} test_status;

// Individual test case
typedef struct {
    const char *name;
    const char *input;
    token_type *expected_types;
    size_t expected_count;
    bool expect_error;
    const char *expected_error_msg;  // Substring to match in error
} lexer_test_case;

// Test result
typedef struct {
    test_status status;
    const char *name;
    const char *message;
    size_t expected_count;
    size_t actual_count;
    size_t first_mismatch;  // Index of first token mismatch
} test_result;

// Test suite
typedef struct {
    const char *name;
    lexer_test_case *cases;
    size_t case_count;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t errors;
} test_suite;

// Test statistics
typedef struct {
    size_t total_suites;
    size_t total_cases;
    size_t passed;
    size_t failed;
    size_t skipped;
    size_t errors;
} test_stats;

// Core test runner functions
test_result run_lexer_test(const lexer_test_case *test);
void run_test_suite(test_suite *suite);
void print_test_result(const test_result *result, bool verbose);
void print_suite_summary(const test_suite *suite);
void print_final_stats(const test_stats *stats);

// Test from file
test_result run_lexer_test_file(const char *script_path, const char *expect_path);

// Utility macros for defining test cases
#define LEXER_TEST(test_name, test_input, ...) \
    { \
        .name = test_name, \
        .input = test_input, \
        .expected_types = (token_type[]){__VA_ARGS__, TOK_EOF}, \
        .expected_count = sizeof((token_type[]){__VA_ARGS__, TOK_EOF}) / sizeof(token_type), \
        .expect_error = false, \
        .expected_error_msg = NULL \
    }

#define LEXER_TEST_ERROR(test_name, test_input, err_msg) \
    { \
        .name = test_name, \
        .input = test_input, \
        .expected_types = NULL, \
        .expected_count = 0, \
        .expect_error = true, \
        .expected_error_msg = err_msg \
    }

#define TEST_SUITE(suite_name, ...) \
    { \
        .name = suite_name, \
        .cases = (lexer_test_case[]){__VA_ARGS__}, \
        .case_count = sizeof((lexer_test_case[]){__VA_ARGS__}) / sizeof(lexer_test_case), \
        .passed = 0, .failed = 0, .skipped = 0, .errors = 0 \
    }

// Color output
#define ANSI_RED     "\x1b[31m"
#define ANSI_GREEN   "\x1b[32m"
#define ANSI_YELLOW  "\x1b[33m"
#define ANSI_BLUE    "\x1b[34m"
#define ANSI_MAGENTA "\x1b[35m"
#define ANSI_CYAN    "\x1b[36m"
#define ANSI_RESET   "\x1b[0m"
#define ANSI_BOLD    "\x1b[1m"

#endif // ELECTRON_LEXER_TEST_H
