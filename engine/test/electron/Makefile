# Electron Lexer Test Suite Makefile

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -g -O0
INCLUDES = -I../../include
LDFLAGS =

# Source files
LEXER_SRC = ../../src/electron/lexer.c
ERROR_SRC = ../../src/electron/error.c
TEST_SRC = lexer_test.c test_lexer_main.c

# Object files
OBJ_DIR = obj
LEXER_OBJ = $(OBJ_DIR)/lexer.o
ERROR_OBJ = $(OBJ_DIR)/error.o
TEST_OBJ = $(OBJ_DIR)/lexer_test.o $(OBJ_DIR)/test_lexer_main.o

# Output
TEST_BIN = test_lexer

# Script directories
SCRIPT_DIR = scripts/lexer
KEYWORD_SCRIPTS = $(wildcard $(SCRIPT_DIR)/keywords/*.e)
OPERATOR_SCRIPTS = $(wildcard $(SCRIPT_DIR)/operators/*.e)
LITERAL_SCRIPTS = $(wildcard $(SCRIPT_DIR)/literals/*.e)
COMMENT_SCRIPTS = $(wildcard $(SCRIPT_DIR)/comments/*.e)
IDENTIFIER_SCRIPTS = $(wildcard $(SCRIPT_DIR)/identifiers/*.e)
EDGE_SCRIPTS = $(wildcard $(SCRIPT_DIR)/edge-cases/*.e)
ERROR_SCRIPTS = $(wildcard $(SCRIPT_DIR)/errors/*.e)
INTEGRATION_SCRIPTS = $(wildcard $(SCRIPT_DIR)/integration/*.e)

ALL_SCRIPTS = $(KEYWORD_SCRIPTS) $(OPERATOR_SCRIPTS) $(LITERAL_SCRIPTS) \
              $(COMMENT_SCRIPTS) $(IDENTIFIER_SCRIPTS) $(EDGE_SCRIPTS) \
              $(ERROR_SCRIPTS) $(INTEGRATION_SCRIPTS)

.PHONY: all clean test test-scripts run help

all: $(OBJ_DIR) $(TEST_BIN)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/lexer.o: $(LEXER_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/error.o: $(ERROR_SRC)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/lexer_test.o: lexer_test.c lexer_test.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/test_lexer_main.o: test_lexer_main.c lexer_test.h
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TEST_BIN): $(LEXER_OBJ) $(ERROR_OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: $(TEST_BIN)
	./$(TEST_BIN)

test: run

# Run tests on individual script files
test-scripts: $(TEST_BIN)
	@echo "Testing script files..."
	@for script in $(ALL_SCRIPTS); do \
		echo "Testing: $$script"; \
		./$(TEST_BIN) --file "$$script" || exit 1; \
	done
	@echo "All script tests passed!"

# Test categories
test-keywords: $(TEST_BIN)
	@echo "Testing keyword scripts..."
	@for script in $(KEYWORD_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

test-operators: $(TEST_BIN)
	@echo "Testing operator scripts..."
	@for script in $(OPERATOR_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

test-literals: $(TEST_BIN)
	@echo "Testing literal scripts..."
	@for script in $(LITERAL_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

test-comments: $(TEST_BIN)
	@echo "Testing comment scripts..."
	@for script in $(COMMENT_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

test-identifiers: $(TEST_BIN)
	@echo "Testing identifier scripts..."
	@for script in $(IDENTIFIER_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

test-edge-cases: $(TEST_BIN)
	@echo "Testing edge case scripts..."
	@for script in $(EDGE_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

test-errors: $(TEST_BIN)
	@echo "Testing error scripts (expecting errors)..."
	@for script in $(ERROR_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script" --expect-error; \
	done

test-integration: $(TEST_BIN)
	@echo "Testing integration scripts..."
	@for script in $(INTEGRATION_SCRIPTS); do \
		echo "  $$script"; \
		./$(TEST_BIN) --file "$$script"; \
	done

# Verbose test run
test-verbose: $(TEST_BIN)
	./$(TEST_BIN) --verbose

# List all test scripts
list-scripts:
	@echo "=== Keyword Scripts ==="
	@for f in $(KEYWORD_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Operator Scripts ==="
	@for f in $(OPERATOR_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Literal Scripts ==="
	@for f in $(LITERAL_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Comment Scripts ==="
	@for f in $(COMMENT_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Identifier Scripts ==="
	@for f in $(IDENTIFIER_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Edge Case Scripts ==="
	@for f in $(EDGE_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Error Scripts ==="
	@for f in $(ERROR_SCRIPTS); do echo "  $$f"; done
	@echo ""
	@echo "=== Integration Scripts ==="
	@for f in $(INTEGRATION_SCRIPTS); do echo "  $$f"; done

count-scripts:
	@echo "Script counts:"
	@echo "  Keywords:     $$(ls $(SCRIPT_DIR)/keywords/*.e 2>/dev/null | wc -l)"
	@echo "  Operators:    $$(ls $(SCRIPT_DIR)/operators/*.e 2>/dev/null | wc -l)"
	@echo "  Literals:     $$(ls $(SCRIPT_DIR)/literals/*.e 2>/dev/null | wc -l)"
	@echo "  Comments:     $$(ls $(SCRIPT_DIR)/comments/*.e 2>/dev/null | wc -l)"
	@echo "  Identifiers:  $$(ls $(SCRIPT_DIR)/identifiers/*.e 2>/dev/null | wc -l)"
	@echo "  Edge cases:   $$(ls $(SCRIPT_DIR)/edge-cases/*.e 2>/dev/null | wc -l)"
	@echo "  Errors:       $$(ls $(SCRIPT_DIR)/errors/*.e 2>/dev/null | wc -l)"
	@echo "  Integration:  $$(ls $(SCRIPT_DIR)/integration/*.e 2>/dev/null | wc -l)"
	@echo "  Total:        $$(find $(SCRIPT_DIR) -name '*.e' | wc -l)"

clean:
	rm -rf $(OBJ_DIR) $(TEST_BIN)

help:
	@echo "Electron Lexer Test Suite"
	@echo ""
	@echo "Targets:"
	@echo "  all            - Build the test binary"
	@echo "  run/test       - Run the inline test suite"
	@echo "  test-scripts   - Run tests on all script files"
	@echo "  test-verbose   - Run tests with verbose output"
	@echo ""
	@echo "Category tests:"
	@echo "  test-keywords     - Test keyword scripts"
	@echo "  test-operators    - Test operator scripts"
	@echo "  test-literals     - Test literal scripts"
	@echo "  test-comments     - Test comment scripts"
	@echo "  test-identifiers  - Test identifier scripts"
	@echo "  test-edge-cases   - Test edge case scripts"
	@echo "  test-errors       - Test error case scripts"
	@echo "  test-integration  - Test integration scripts"
	@echo ""
	@echo "Utilities:"
	@echo "  list-scripts   - List all test scripts"
	@echo "  count-scripts  - Count scripts by category"
	@echo "  clean          - Remove build artifacts"
	@echo "  help           - Show this help"
