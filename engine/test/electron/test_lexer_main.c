#include "lexer_test.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//=============================================================================
// KEYWORD TEST SUITES
//=============================================================================

static test_suite suite_keywords_types = TEST_SUITE("Keywords - Types",
    // Primitive types
    LEXER_TEST("bool keyword", "bool", TOK_KW_BOOL),
    LEXER_TEST("int keyword", "int", TOK_KW_INT),
    LEXER_TEST("float keyword", "float", TOK_KW_FLOAT),
    LEXER_TEST("string keyword", "string", TOK_KW_STRING),
    LEXER_TEST("void keyword", "void", TOK_KW_VOID),

    // Vector types
    LEXER_TEST("vec2 keyword", "vec2", TOK_KW_VEC2),
    LEXER_TEST("vec3 keyword", "vec3", TOK_KW_VEC3),
    LEXER_TEST("vec4 keyword", "vec4", TOK_KW_VEC4),

    // Matrix types
    LEXER_TEST("mat2 keyword", "mat2", TOK_KW_MAT2),
    LEXER_TEST("mat3 keyword", "mat3", TOK_KW_MAT3),
    LEXER_TEST("mat4 keyword", "mat4", TOK_KW_MAT4),

    // Collection types
    LEXER_TEST("list keyword", "list", TOK_KW_LIST),
    LEXER_TEST("dict keyword", "dict", TOK_KW_DICT),
    LEXER_TEST("range keyword", "range", TOK_KW_RANGE),
    LEXER_TEST("tuple keyword", "tuple", TOK_KW_TUPLE),
    LEXER_TEST("result keyword", "result", TOK_KW_RESULT),
    LEXER_TEST("coro keyword", "coro", TOK_KW_CORO),
    LEXER_TEST("func keyword", "func", TOK_KW_FUNC),

    // User-defined type keywords
    LEXER_TEST("type keyword", "type", TOK_KW_TYPE),
    LEXER_TEST("struct keyword", "struct", TOK_KW_STRUCT),
    LEXER_TEST("core keyword", "core", TOK_KW_CORE),
    LEXER_TEST("shell keyword", "shell", TOK_KW_SHELL),
    LEXER_TEST("enum keyword", "enum", TOK_KW_ENUM),
    LEXER_TEST("entity keyword", "entity", TOK_KW_ENTITY),
    LEXER_TEST("never keyword", "never", TOK_KW_NEVER)
);

static test_suite suite_keywords_declarations = TEST_SUITE("Keywords - Declarations",
    LEXER_TEST("const keyword", "const", TOK_KW_CONST),
    LEXER_TEST("def keyword", "def", TOK_KW_DEF),
    LEXER_TEST("mut keyword", "mut", TOK_KW_MUT),
    LEXER_TEST("readonly keyword", "readonly", TOK_KW_READONLY),
    LEXER_TEST("public keyword", "public", TOK_KW_PUBLIC),
    LEXER_TEST("local keyword", "local", TOK_KW_LOCAL),
    LEXER_TEST("export keyword", "export", TOK_KW_EXPORT),
    LEXER_TEST("import keyword", "import", TOK_KW_IMPORT),
    LEXER_TEST("from keyword", "from", TOK_KW_FROM)
);

static test_suite suite_keywords_control = TEST_SUITE("Keywords - Control Flow",
    LEXER_TEST("if keyword", "if", TOK_KW_IF),
    LEXER_TEST("else keyword", "else", TOK_KW_ELSE),
    LEXER_TEST("then keyword", "then", TOK_KW_THEN),
    LEXER_TEST("while keyword", "while", TOK_KW_WHILE),
    LEXER_TEST("do keyword", "do", TOK_KW_DO),
    LEXER_TEST("loop keyword", "loop", TOK_KW_LOOP),
    LEXER_TEST("for keyword", "for", TOK_KW_FOR),
    LEXER_TEST("in keyword", "in", TOK_KW_IN),
    LEXER_TEST("break keyword", "break", TOK_KW_BREAK),
    LEXER_TEST("continue keyword", "continue", TOK_KW_CONTINUE),
    LEXER_TEST("return keyword", "return", TOK_KW_RETURN),
    LEXER_TEST("match keyword", "match", TOK_KW_MATCH),
    LEXER_TEST("defer keyword", "defer", TOK_KW_DEFER)
);

static test_suite suite_keywords_operators = TEST_SUITE("Keywords - Logical Operators",
    LEXER_TEST("is keyword", "is", TOK_KW_IS),
    LEXER_TEST("not keyword", "not", TOK_KW_NOT),
    LEXER_TEST("as keyword", "as", TOK_KW_AS),
    LEXER_TEST("and keyword", "and", TOK_KW_AND),
    LEXER_TEST("or keyword", "or", TOK_KW_OR)
);

static test_suite suite_keywords_error = TEST_SUITE("Keywords - Error Handling",
    LEXER_TEST("success keyword", "success", TOK_KW_SUCCESS),
    LEXER_TEST("error keyword", "error", TOK_KW_ERROR),
    LEXER_TEST("allows keyword", "allows", TOK_KW_ALLOWS)
);

static test_suite suite_keywords_coro = TEST_SUITE("Keywords - Coroutines",
    LEXER_TEST("spawn keyword", "spawn", TOK_KW_SPAWN),
    LEXER_TEST("yield keyword", "yield", TOK_KW_YIELD),
    LEXER_TEST("yield_request keyword", "yield_request", TOK_KW_YIELD_REQUEST)
);

static test_suite suite_keywords_component = TEST_SUITE("Keywords - Component System",
    LEXER_TEST("inner keyword", "inner", TOK_KW_INNER),
    LEXER_TEST("outer keyword", "outer", TOK_KW_OUTER),
    LEXER_TEST("sibling keyword", "sibling", TOK_KW_SIBLING),
    LEXER_TEST("sibling_all keyword", "sibling_all", TOK_KW_SIBLING_ALL),
    LEXER_TEST("requires keyword", "requires", TOK_KW_REQUIRES),
    LEXER_TEST("on_cancel keyword", "on_cancel", TOK_KW_ON_CANCEL),
    LEXER_TEST("sealed keyword", "sealed", TOK_KW_SEALED),
    LEXER_TEST("unique keyword", "unique", TOK_KW_UNIQUE),
    LEXER_TEST("strict keyword", "strict", TOK_KW_STRICT)
);

static test_suite suite_keywords_reserved = TEST_SUITE("Keywords - Reserved",
    LEXER_TEST("async keyword", "async", TOK_KW_ASYNC),
    LEXER_TEST("await keyword", "await", TOK_KW_AWAIT),
    LEXER_TEST("class keyword", "class", TOK_KW_CLASS),
    LEXER_TEST("extends keyword", "extends", TOK_KW_EXTENDS),
    LEXER_TEST("implements keyword", "implements", TOK_KW_IMPLEMENTS),
    LEXER_TEST("new keyword", "new", TOK_KW_NEW),
    LEXER_TEST("private keyword", "private", TOK_KW_PRIVATE),
    LEXER_TEST("protected keyword", "protected", TOK_KW_PROTECTED),
    LEXER_TEST("static keyword", "static", TOK_KW_STATIC),
    LEXER_TEST("super keyword", "super", TOK_KW_SUPER),
    LEXER_TEST("this keyword", "this", TOK_KW_THIS),
    LEXER_TEST("throw keyword", "throw", TOK_KW_THROW),
    LEXER_TEST("try keyword", "try", TOK_KW_TRY),
    LEXER_TEST("catch keyword", "catch", TOK_KW_CATCH),
    LEXER_TEST("finally keyword", "finally", TOK_KW_FINALLY),
    LEXER_TEST("with keyword", "with", TOK_KW_WITH),
    LEXER_TEST("sizeof keyword", "sizeof", TOK_KW_SIZEOF)
);

static test_suite suite_keywords_literals = TEST_SUITE("Keywords - Literal Keywords",
    LEXER_TEST("true keyword", "true", TOK_KW_TRUE),
    LEXER_TEST("false keyword", "false", TOK_KW_FALSE),
    LEXER_TEST("null keyword", "null", TOK_KW_NULL)
);

//=============================================================================
// OPERATOR TEST SUITES
//=============================================================================

static test_suite suite_ops_arithmetic = TEST_SUITE("Operators - Arithmetic",
    LEXER_TEST("plus", "+", TOK_PLUS),
    LEXER_TEST("minus", "-", TOK_MINUS),
    LEXER_TEST("star", "*", TOK_STAR),
    LEXER_TEST("slash", "/", TOK_SLASH),
    LEXER_TEST("percent", "%", TOK_PERCENT),

    // Multiple arithmetic operators
    LEXER_TEST("1 + 2", "1 + 2", TOK_INT_LITERAL, TOK_PLUS, TOK_INT_LITERAL),
    LEXER_TEST("a - b", "a - b", TOK_IDENTIFIER, TOK_MINUS, TOK_IDENTIFIER),
    LEXER_TEST("x * y", "x * y", TOK_IDENTIFIER, TOK_STAR, TOK_IDENTIFIER),
    LEXER_TEST("10 / 5", "10 / 5", TOK_INT_LITERAL, TOK_SLASH, TOK_INT_LITERAL),
    LEXER_TEST("7 % 3", "7 % 3", TOK_INT_LITERAL, TOK_PERCENT, TOK_INT_LITERAL),
    LEXER_TEST("complex expr", "a + b * c - d",
        TOK_IDENTIFIER, TOK_PLUS, TOK_IDENTIFIER, TOK_STAR,
        TOK_IDENTIFIER, TOK_MINUS, TOK_IDENTIFIER)
);

static test_suite suite_ops_comparison = TEST_SUITE("Operators - Comparison",
    LEXER_TEST("equal equal", "==", TOK_EQ_EQ),
    LEXER_TEST("not equal", "!=", TOK_BANG_EQ),
    LEXER_TEST("less than", "<", TOK_LT),
    LEXER_TEST("greater than", ">", TOK_GT),
    LEXER_TEST("less equal", "<=", TOK_LT_EQ),
    LEXER_TEST("greater equal", ">=", TOK_GT_EQ),

    // In context
    LEXER_TEST("a == b", "a == b", TOK_IDENTIFIER, TOK_EQ_EQ, TOK_IDENTIFIER),
    LEXER_TEST("x != y", "x != y", TOK_IDENTIFIER, TOK_BANG_EQ, TOK_IDENTIFIER),
    LEXER_TEST("1 < 2", "1 < 2", TOK_INT_LITERAL, TOK_LT, TOK_INT_LITERAL),
    LEXER_TEST("a > b", "a > b", TOK_IDENTIFIER, TOK_GT, TOK_IDENTIFIER),
    LEXER_TEST("x <= y", "x <= y", TOK_IDENTIFIER, TOK_LT_EQ, TOK_IDENTIFIER),
    LEXER_TEST("a >= b", "a >= b", TOK_IDENTIFIER, TOK_GT_EQ, TOK_IDENTIFIER)
);

static test_suite suite_ops_logical = TEST_SUITE("Operators - Logical",
    LEXER_TEST("logical and", "&&", TOK_AMP_AMP),
    LEXER_TEST("logical or", "||", TOK_PIPE_PIPE),
    LEXER_TEST("logical not", "!", TOK_BANG),

    // In context
    LEXER_TEST("a && b", "a && b", TOK_IDENTIFIER, TOK_AMP_AMP, TOK_IDENTIFIER),
    LEXER_TEST("x || y", "x || y", TOK_IDENTIFIER, TOK_PIPE_PIPE, TOK_IDENTIFIER),
    LEXER_TEST("!flag", "!flag", TOK_BANG, TOK_IDENTIFIER),
    LEXER_TEST("complex logic", "a && b || !c",
        TOK_IDENTIFIER, TOK_AMP_AMP, TOK_IDENTIFIER, TOK_PIPE_PIPE,
        TOK_BANG, TOK_IDENTIFIER)
);

static test_suite suite_ops_bitwise = TEST_SUITE("Operators - Bitwise",
    LEXER_TEST("bitwise and", "&", TOK_AMP),
    LEXER_TEST("bitwise or", "|", TOK_PIPE),
    LEXER_TEST("bitwise xor", "^", TOK_CARET),
    LEXER_TEST("bitwise not", "~", TOK_TILDE),
    LEXER_TEST("left shift", "<<", TOK_LT_LT),
    LEXER_TEST("right shift", ">>", TOK_GT_GT),

    // In context
    LEXER_TEST("a & b", "a & b", TOK_IDENTIFIER, TOK_AMP, TOK_IDENTIFIER),
    LEXER_TEST("x | y", "x | y", TOK_IDENTIFIER, TOK_PIPE, TOK_IDENTIFIER),
    LEXER_TEST("a ^ b", "a ^ b", TOK_IDENTIFIER, TOK_CARET, TOK_IDENTIFIER),
    LEXER_TEST("~mask", "~mask", TOK_TILDE, TOK_IDENTIFIER),
    LEXER_TEST("1 << 4", "1 << 4", TOK_INT_LITERAL, TOK_LT_LT, TOK_INT_LITERAL),
    LEXER_TEST("x >> 2", "x >> 2", TOK_IDENTIFIER, TOK_GT_GT, TOK_INT_LITERAL)
);

static test_suite suite_ops_assignment = TEST_SUITE("Operators - Assignment",
    LEXER_TEST("assign", "=", TOK_EQ),
    LEXER_TEST("plus assign", "+=", TOK_PLUS_EQ),
    LEXER_TEST("minus assign", "-=", TOK_MINUS_EQ),
    LEXER_TEST("star assign", "*=", TOK_STAR_EQ),
    LEXER_TEST("slash assign", "/=", TOK_SLASH_EQ),
    LEXER_TEST("percent assign", "%=", TOK_PERCENT_EQ),
    LEXER_TEST("and assign", "&=", TOK_AMP_EQ),
    LEXER_TEST("or assign", "|=", TOK_PIPE_EQ),
    LEXER_TEST("xor assign", "^=", TOK_CARET_EQ),
    LEXER_TEST("left shift assign", "<<=", TOK_LT_LT_EQ),
    LEXER_TEST("right shift assign", ">>=", TOK_GT_GT_EQ),

    // In context
    LEXER_TEST("x = 5", "x = 5", TOK_IDENTIFIER, TOK_EQ, TOK_INT_LITERAL),
    LEXER_TEST("a += 1", "a += 1", TOK_IDENTIFIER, TOK_PLUS_EQ, TOK_INT_LITERAL),
    LEXER_TEST("b -= 2", "b -= 2", TOK_IDENTIFIER, TOK_MINUS_EQ, TOK_INT_LITERAL),
    LEXER_TEST("c *= 3", "c *= 3", TOK_IDENTIFIER, TOK_STAR_EQ, TOK_INT_LITERAL)
);

static test_suite suite_ops_shell = TEST_SUITE("Operators - Shell Operations",
    LEXER_TEST("push shell", "<-", TOK_LT_MINUS),
    LEXER_TEST("remove shell", "->", TOK_MINUS_GT),
    LEXER_TEST("force push", "<<-", TOK_LT_LT_MINUS),
    LEXER_TEST("remove all", "->>", TOK_MINUS_GT_GT),
    LEXER_TEST("query shell", ">-", TOK_GT_MINUS),

    // In context
    LEXER_TEST("player <- Shell", "player <- Shell",
        TOK_IDENTIFIER, TOK_LT_MINUS, TOK_IDENTIFIER),
    LEXER_TEST("entity -> Combat", "entity -> Combat",
        TOK_IDENTIFIER, TOK_MINUS_GT, TOK_IDENTIFIER),
    LEXER_TEST("obj <<- Force", "obj <<- Force",
        TOK_IDENTIFIER, TOK_LT_LT_MINUS, TOK_IDENTIFIER),
    LEXER_TEST("player ->> Buff", "player ->> Buff",
        TOK_IDENTIFIER, TOK_MINUS_GT_GT, TOK_IDENTIFIER),
    LEXER_TEST("if player >- Frozen", "if player >- Frozen",
        TOK_KW_IF, TOK_IDENTIFIER, TOK_GT_MINUS, TOK_IDENTIFIER)
);

static test_suite suite_ops_special = TEST_SUITE("Operators - Special",
    LEXER_TEST("safe access", "?.", TOK_QUESTION_DOT),
    LEXER_TEST("null coalesce", "??", TOK_QUESTION_QUESTION),
    LEXER_TEST("question", "?", TOK_QUESTION),
    LEXER_TEST("dot", ".", TOK_DOT),
    LEXER_TEST("range exclusive", "..", TOK_DOT_DOT),
    LEXER_TEST("range inclusive", "..=", TOK_DOT_DOT_EQ),
    LEXER_TEST("arrow", "=>", TOK_ARROW),
    LEXER_TEST("colon", ":", TOK_COLON),
    LEXER_TEST("double colon", "::", TOK_COLON_COLON),
    LEXER_TEST("semicolon", ";", TOK_SEMICOLON),
    LEXER_TEST("comma", ",", TOK_COMMA),
    LEXER_TEST("underscore", "_", TOK_UNDERSCORE),
    LEXER_TEST("at", "@", TOK_AT),

    // In context
    LEXER_TEST("obj?.field", "obj?.field",
        TOK_IDENTIFIER, TOK_QUESTION_DOT, TOK_IDENTIFIER),
    LEXER_TEST("x ?? default", "x ?? default",
        TOK_IDENTIFIER, TOK_QUESTION_QUESTION, TOK_IDENTIFIER),
    LEXER_TEST("result?", "result?", TOK_IDENTIFIER, TOK_QUESTION),
    LEXER_TEST("obj.field", "obj.field", TOK_IDENTIFIER, TOK_DOT, TOK_IDENTIFIER),
    LEXER_TEST("0..10", "0..10", TOK_INT_LITERAL, TOK_DOT_DOT, TOK_INT_LITERAL),
    LEXER_TEST("0..=10", "0..=10", TOK_INT_LITERAL, TOK_DOT_DOT_EQ, TOK_INT_LITERAL),
    LEXER_TEST("x => y", "x => y", TOK_IDENTIFIER, TOK_ARROW, TOK_IDENTIFIER),
    LEXER_TEST("Module::func", "Module::func",
        TOK_IDENTIFIER, TOK_COLON_COLON, TOK_IDENTIFIER)
);

static test_suite suite_delimiters = TEST_SUITE("Delimiters",
    LEXER_TEST("lparen", "(", TOK_LPAREN),
    LEXER_TEST("rparen", ")", TOK_RPAREN),
    LEXER_TEST("lbracket", "[", TOK_LBRACKET),
    LEXER_TEST("rbracket", "]", TOK_RBRACKET),
    LEXER_TEST("lbrace", "{", TOK_LBRACE),
    LEXER_TEST("rbrace", "}", TOK_RBRACE),

    // Nested
    LEXER_TEST("nested parens", "(())", TOK_LPAREN, TOK_LPAREN, TOK_RPAREN, TOK_RPAREN),
    LEXER_TEST("nested brackets", "[[]]", TOK_LBRACKET, TOK_LBRACKET, TOK_RBRACKET, TOK_RBRACKET),
    LEXER_TEST("nested braces", "{{}}", TOK_LBRACE, TOK_LBRACE, TOK_RBRACE, TOK_RBRACE),
    LEXER_TEST("mixed delimiters", "({[]})",
        TOK_LPAREN, TOK_LBRACE, TOK_LBRACKET, TOK_RBRACKET, TOK_RBRACE, TOK_RPAREN)
);

//=============================================================================
// LITERAL TEST SUITES
//=============================================================================

static test_suite suite_literals_int = TEST_SUITE("Literals - Integers",
    // Basic integers
    LEXER_TEST("zero", "0", TOK_INT_LITERAL),
    LEXER_TEST("single digit", "5", TOK_INT_LITERAL),
    LEXER_TEST("multi digit", "42", TOK_INT_LITERAL),
    LEXER_TEST("large number", "1234567890", TOK_INT_LITERAL),

    // With underscores
    LEXER_TEST("underscore separator", "1_000_000", TOK_INT_LITERAL),
    LEXER_TEST("underscore pairs", "10_00_00", TOK_INT_LITERAL),

    // Hexadecimal
    LEXER_TEST("hex lowercase", "0xff", TOK_INT_LITERAL),
    LEXER_TEST("hex uppercase", "0xFF", TOK_INT_LITERAL),
    LEXER_TEST("hex mixed", "0xDeadBeef", TOK_INT_LITERAL),
    LEXER_TEST("hex zero", "0x0", TOK_INT_LITERAL),
    LEXER_TEST("hex with underscore", "0xFF_FF", TOK_INT_LITERAL),

    // Binary
    LEXER_TEST("binary basic", "0b1010", TOK_INT_LITERAL),
    LEXER_TEST("binary zero", "0b0", TOK_INT_LITERAL),
    LEXER_TEST("binary one", "0b1", TOK_INT_LITERAL),
    LEXER_TEST("binary with underscore", "0b1010_1010", TOK_INT_LITERAL),
    LEXER_TEST("binary byte", "0b1111_1111", TOK_INT_LITERAL),

    // Octal
    LEXER_TEST("octal basic", "0o755", TOK_INT_LITERAL),
    LEXER_TEST("octal zero", "0o0", TOK_INT_LITERAL),
    LEXER_TEST("octal with underscore", "0o755_644", TOK_INT_LITERAL),

    // Negative (unary minus + literal)
    LEXER_TEST("negative int", "-42", TOK_MINUS, TOK_INT_LITERAL),
    LEXER_TEST("negative hex", "-0xFF", TOK_MINUS, TOK_INT_LITERAL)
);

static test_suite suite_literals_float = TEST_SUITE("Literals - Floats",
    // Basic floats
    LEXER_TEST("simple float", "3.14", TOK_FLOAT_LITERAL),
    LEXER_TEST("zero point", "0.0", TOK_FLOAT_LITERAL),
    LEXER_TEST("leading zero", "0.5", TOK_FLOAT_LITERAL),
    LEXER_TEST("trailing zero", "2.0", TOK_FLOAT_LITERAL),

    // Scientific notation
    LEXER_TEST("scientific e", "1.5e10", TOK_FLOAT_LITERAL),
    LEXER_TEST("scientific E", "1.5E10", TOK_FLOAT_LITERAL),
    LEXER_TEST("scientific positive", "1.5e+10", TOK_FLOAT_LITERAL),
    LEXER_TEST("scientific negative", "1.5e-10", TOK_FLOAT_LITERAL),
    LEXER_TEST("integer exponent", "1e10", TOK_FLOAT_LITERAL),

    // Float suffix
    LEXER_TEST("float suffix f", "3.14f", TOK_FLOAT_LITERAL),
    LEXER_TEST("float suffix F", "3.14F", TOK_FLOAT_LITERAL),
    LEXER_TEST("integer with f", "42f", TOK_FLOAT_LITERAL),

    // With underscores
    LEXER_TEST("float underscore", "1_000.0", TOK_FLOAT_LITERAL),
    LEXER_TEST("float underscore decimal", "3.141_592", TOK_FLOAT_LITERAL),

    // Negative floats
    LEXER_TEST("negative float", "-3.14", TOK_MINUS, TOK_FLOAT_LITERAL),
    LEXER_TEST("negative scientific", "-1e10", TOK_MINUS, TOK_FLOAT_LITERAL)
);

static test_suite suite_literals_string = TEST_SUITE("Literals - Strings",
    // Basic strings
    LEXER_TEST("empty string", "\"\"", TOK_STRING_LITERAL),
    LEXER_TEST("simple string", "\"hello\"", TOK_STRING_LITERAL),
    LEXER_TEST("string with spaces", "\"hello world\"", TOK_STRING_LITERAL),

    // Escape sequences
    LEXER_TEST("escape newline", "\"line1\\nline2\"", TOK_STRING_LITERAL),
    LEXER_TEST("escape tab", "\"col1\\tcol2\"", TOK_STRING_LITERAL),
    LEXER_TEST("escape backslash", "\"path\\\\file\"", TOK_STRING_LITERAL),
    LEXER_TEST("escape quote", "\"say \\\"hello\\\"\"", TOK_STRING_LITERAL),
    LEXER_TEST("escape carriage", "\"line\\r\"", TOK_STRING_LITERAL),
    LEXER_TEST("escape null", "\"null\\0char\"", TOK_STRING_LITERAL),
    LEXER_TEST("multiple escapes", "\"a\\nb\\tc\\\\d\"", TOK_STRING_LITERAL),

    // Raw strings
    LEXER_TEST("raw string", "r\"no\\escapes\"", TOK_RAW_STRING_LITERAL),
    LEXER_TEST("raw empty", "r\"\"", TOK_RAW_STRING_LITERAL),
    LEXER_TEST("raw with backslash", "r\"C:\\path\\file\"", TOK_RAW_STRING_LITERAL),

    // Multiline strings
    LEXER_TEST("multiline basic", "\"\"\"multi\nline\"\"\"", TOK_MULTILINE_STRING_LITERAL),
    LEXER_TEST("multiline empty", "\"\"\"\"\"\"", TOK_MULTILINE_STRING_LITERAL),

    // String in context
    LEXER_TEST("string assignment", "x = \"hello\"",
        TOK_IDENTIFIER, TOK_EQ, TOK_STRING_LITERAL),
    LEXER_TEST("function call with string", "print(\"test\")",
        TOK_IDENTIFIER, TOK_LPAREN, TOK_STRING_LITERAL, TOK_RPAREN)
);

static test_suite suite_literals_bool = TEST_SUITE("Literals - Booleans",
    LEXER_TEST("true literal", "true", TOK_KW_TRUE),
    LEXER_TEST("false literal", "false", TOK_KW_FALSE),
    LEXER_TEST("bool in condition", "if true",
        TOK_KW_IF, TOK_KW_TRUE),
    LEXER_TEST("bool assignment", "flag = false",
        TOK_IDENTIFIER, TOK_EQ, TOK_KW_FALSE),
    LEXER_TEST("bool comparison", "a == true",
        TOK_IDENTIFIER, TOK_EQ_EQ, TOK_KW_TRUE)
);

//=============================================================================
// IDENTIFIER TEST SUITES
//=============================================================================

static test_suite suite_identifiers = TEST_SUITE("Identifiers",
    // Basic identifiers
    LEXER_TEST("single char", "x", TOK_IDENTIFIER),
    LEXER_TEST("lowercase", "foo", TOK_IDENTIFIER),
    LEXER_TEST("uppercase", "FOO", TOK_IDENTIFIER),
    LEXER_TEST("mixed case", "FooBar", TOK_IDENTIFIER),
    LEXER_TEST("camelCase", "myVariable", TOK_IDENTIFIER),
    LEXER_TEST("PascalCase", "MyClass", TOK_IDENTIFIER),
    LEXER_TEST("snake_case", "my_variable", TOK_IDENTIFIER),
    LEXER_TEST("SCREAMING_SNAKE", "MAX_VALUE", TOK_IDENTIFIER),

    // With numbers
    LEXER_TEST("trailing number", "foo1", TOK_IDENTIFIER),
    LEXER_TEST("embedded numbers", "vec3d", TOK_IDENTIFIER),
    LEXER_TEST("multiple numbers", "test123abc", TOK_IDENTIFIER),

    // With underscores
    LEXER_TEST("leading underscore", "_private", TOK_IDENTIFIER),
    LEXER_TEST("double underscore", "__dunder__", TOK_IDENTIFIER),
    LEXER_TEST("trailing underscore", "reserved_", TOK_IDENTIFIER),

    // Keywords as prefixes (should be identifiers)
    LEXER_TEST("if_condition (not keyword)", "if_condition", TOK_IDENTIFIER),
    LEXER_TEST("integer (not int)", "integer", TOK_IDENTIFIER),
    LEXER_TEST("floating (not float)", "floating", TOK_IDENTIFIER),
    LEXER_TEST("boolean (not bool)", "boolean", TOK_IDENTIFIER),
    LEXER_TEST("format (not for)", "format", TOK_IDENTIFIER),
    LEXER_TEST("while_loop (not while)", "while_loop", TOK_IDENTIFIER),
    LEXER_TEST("return_value (not return)", "return_value", TOK_IDENTIFIER),

    // Edge cases
    LEXER_TEST("all underscores", "___", TOK_IDENTIFIER),
    LEXER_TEST("very long identifier",
        "thisIsAVeryLongIdentifierNameThatExceedsNormalLengths", TOK_IDENTIFIER)
);

//=============================================================================
// COMMENT TEST SUITES
//=============================================================================

static test_suite suite_comments = TEST_SUITE("Comments",
    // Line comments (should be skipped, resulting in just EOF)
    LEXER_TEST("line comment only", "// this is a comment", TOK_EOF),
    LEXER_TEST("line comment after code", "x // comment",
        TOK_IDENTIFIER),
    LEXER_TEST("line comment with symbols", "// @#$%^&*()", TOK_EOF),

    // Block comments
    LEXER_TEST("block comment only", "/* comment */", TOK_EOF),
    LEXER_TEST("block comment multiline", "/* line1\nline2 */", TOK_EOF),
    LEXER_TEST("block comment between tokens", "a /* comment */ b",
        TOK_IDENTIFIER, TOK_IDENTIFIER),
    LEXER_TEST("block comment no space", "a/**/b",
        TOK_IDENTIFIER, TOK_IDENTIFIER),

    // Nested block comments
    LEXER_TEST("nested block comment", "/* outer /* inner */ outer */", TOK_EOF),

    // Mixed comments
    LEXER_TEST("line in block", "/* // not a line comment */", TOK_EOF),
    LEXER_TEST("block after line", "// line\n/* block */", TOK_EOF)
);

//=============================================================================
// EDGE CASE TEST SUITES
//=============================================================================

static test_suite suite_edge_whitespace = TEST_SUITE("Edge Cases - Whitespace",
    LEXER_TEST("empty input", "", TOK_EOF),
    LEXER_TEST("only spaces", "   ", TOK_EOF),
    LEXER_TEST("only tabs", "\t\t\t", TOK_EOF),
    LEXER_TEST("only newlines", "\n\n\n", TOK_EOF),
    LEXER_TEST("mixed whitespace", "  \t\n  \t\n  ", TOK_EOF),
    LEXER_TEST("token with leading space", "  x", TOK_IDENTIFIER),
    LEXER_TEST("token with trailing space", "x  ", TOK_IDENTIFIER),
    LEXER_TEST("tokens separated by newline", "a\nb",
        TOK_IDENTIFIER, TOK_IDENTIFIER),
    LEXER_TEST("tokens separated by multiple spaces", "a    b",
        TOK_IDENTIFIER, TOK_IDENTIFIER)
);

static test_suite suite_edge_operators = TEST_SUITE("Edge Cases - Operator Disambiguation",
    // Distinguish similar operators
    LEXER_TEST("< vs <<", "< <<", TOK_LT, TOK_LT_LT),
    LEXER_TEST("> vs >>", "> >>", TOK_GT, TOK_GT_GT),
    LEXER_TEST("= vs ==", "= ==", TOK_EQ, TOK_EQ_EQ),
    LEXER_TEST("& vs &&", "& &&", TOK_AMP, TOK_AMP_AMP),
    LEXER_TEST("| vs ||", "| ||", TOK_PIPE, TOK_PIPE_PIPE),
    LEXER_TEST("! vs !=", "! !=", TOK_BANG, TOK_BANG_EQ),

    // Shell operators vs arrows
    LEXER_TEST("<- vs < -", "<- < -", TOK_LT_MINUS, TOK_LT, TOK_MINUS),
    LEXER_TEST("-> vs - >", "-> - >", TOK_MINUS_GT, TOK_MINUS, TOK_GT),
    LEXER_TEST("<<- sequence", "<<-", TOK_LT_LT_MINUS),
    LEXER_TEST("->> sequence", "->>", TOK_MINUS_GT_GT),

    // Dots and ranges
    LEXER_TEST(". vs ..", ". ..", TOK_DOT, TOK_DOT_DOT),
    LEXER_TEST(".. vs ..=", ".. ..=", TOK_DOT_DOT, TOK_DOT_DOT_EQ),
    LEXER_TEST("a.b vs a..b", "a.b a..b",
        TOK_IDENTIFIER, TOK_DOT, TOK_IDENTIFIER,
        TOK_IDENTIFIER, TOK_DOT_DOT, TOK_IDENTIFIER),

    // Question mark variants
    LEXER_TEST("? vs ?.", "? ?.", TOK_QUESTION, TOK_QUESTION_DOT),
    LEXER_TEST("? vs ??", "? ??", TOK_QUESTION, TOK_QUESTION_QUESTION),

    // Colons
    LEXER_TEST(": vs ::", ": ::", TOK_COLON, TOK_COLON_COLON),

    // No spaces
    LEXER_TEST("a+b no space", "a+b", TOK_IDENTIFIER, TOK_PLUS, TOK_IDENTIFIER),
    LEXER_TEST("x==y no space", "x==y", TOK_IDENTIFIER, TOK_EQ_EQ, TOK_IDENTIFIER)
);

static test_suite suite_edge_numbers = TEST_SUITE("Edge Cases - Number Boundaries",
    // Numbers touching identifiers
    LEXER_TEST("123abc", "123abc", TOK_INT_LITERAL, TOK_IDENTIFIER),
    LEXER_TEST("0xGHI (invalid hex)", "0xGHI", TOK_INT_LITERAL, TOK_IDENTIFIER),

    // Dot after integer
    LEXER_TEST("1.x member access", "1.x", TOK_INT_LITERAL, TOK_DOT, TOK_IDENTIFIER),
    LEXER_TEST("1..10 range", "1..10", TOK_INT_LITERAL, TOK_DOT_DOT, TOK_INT_LITERAL),

    // Float edge cases
    LEXER_TEST("1. (trailing dot)", "1.", TOK_FLOAT_LITERAL),
    LEXER_TEST(".5 (leading dot)", ".5", TOK_FLOAT_LITERAL),

    // Multiple dots
    LEXER_TEST("1.2.3 (version-like)", "1.2.3",
        TOK_FLOAT_LITERAL, TOK_DOT, TOK_INT_LITERAL),

    // Exponent edge
    LEXER_TEST("1e (incomplete)", "1e", TOK_INT_LITERAL, TOK_IDENTIFIER)
);

//=============================================================================
// ERROR TEST SUITES
//=============================================================================

static test_suite suite_errors = TEST_SUITE("Lexer Errors",
    // Unterminated strings
    LEXER_TEST_ERROR("unterminated string", "\"hello", "unterminated"),
    LEXER_TEST_ERROR("unterminated with newline", "\"hello\nworld\"", "unterminated"),
    LEXER_TEST_ERROR("unterminated raw string", "r\"hello", "unterminated"),
    LEXER_TEST_ERROR("unterminated multiline", "\"\"\"hello", "unterminated"),

    // Invalid escapes
    LEXER_TEST_ERROR("invalid escape", "\"\\q\"", "escape"),

    // Unterminated comments
    LEXER_TEST_ERROR("unterminated block comment", "/* comment", "unterminated"),
    LEXER_TEST_ERROR("nested unterminated", "/* outer /* inner */", "unterminated"),

    // Invalid characters
    LEXER_TEST_ERROR("backtick", "`", "unexpected"),
    LEXER_TEST_ERROR("dollar sign", "$", "unexpected"),
    LEXER_TEST_ERROR("at sign alone", "@!", "unexpected"),

    // Invalid number formats
    LEXER_TEST_ERROR("hex no digits", "0x", "invalid"),
    LEXER_TEST_ERROR("binary no digits", "0b", "invalid"),
    LEXER_TEST_ERROR("octal no digits", "0o", "invalid"),
    LEXER_TEST_ERROR("leading underscore number", "_123", "identifier"),
    LEXER_TEST_ERROR("double underscore in number", "1__2", "invalid")
);

//=============================================================================
// INTEGRATION TEST SUITES
//=============================================================================

static test_suite suite_integration_declarations = TEST_SUITE("Integration - Declarations",
    // Variable declarations
    LEXER_TEST("int declaration", "int x = 5;",
        TOK_KW_INT, TOK_IDENTIFIER, TOK_EQ, TOK_INT_LITERAL, TOK_SEMICOLON),
    LEXER_TEST("float declaration", "float y = 3.14;",
        TOK_KW_FLOAT, TOK_IDENTIFIER, TOK_EQ, TOK_FLOAT_LITERAL, TOK_SEMICOLON),
    LEXER_TEST("string declaration", "string name = \"hello\";",
        TOK_KW_STRING, TOK_IDENTIFIER, TOK_EQ, TOK_STRING_LITERAL, TOK_SEMICOLON),
    LEXER_TEST("bool declaration", "bool flag = true;",
        TOK_KW_BOOL, TOK_IDENTIFIER, TOK_EQ, TOK_KW_TRUE, TOK_SEMICOLON),

    // Const declarations
    LEXER_TEST("const declaration", "const MAX = 100;",
        TOK_KW_CONST, TOK_IDENTIFIER, TOK_EQ, TOK_INT_LITERAL, TOK_SEMICOLON),
    LEXER_TEST("comptime const", "@comptime const SIZE = 256;",
        TOK_AT_COMPTIME, TOK_KW_CONST, TOK_IDENTIFIER, TOK_EQ, TOK_INT_LITERAL, TOK_SEMICOLON),

    // Vector declarations
    LEXER_TEST("vec3 declaration", "vec3 pos = vec3(0, 0, 0);",
        TOK_KW_VEC3, TOK_IDENTIFIER, TOK_EQ, TOK_KW_VEC3, TOK_LPAREN,
        TOK_INT_LITERAL, TOK_COMMA, TOK_INT_LITERAL, TOK_COMMA, TOK_INT_LITERAL, TOK_RPAREN, TOK_SEMICOLON)
);

static test_suite suite_integration_functions = TEST_SUITE("Integration - Functions",
    // Simple function
    LEXER_TEST("void function", "def foo() { }",
        TOK_KW_DEF, TOK_IDENTIFIER, TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE),

    // Function with return type
    LEXER_TEST("function with return", "def add(int a, int b) -> int { return a + b; }",
        TOK_KW_DEF, TOK_IDENTIFIER, TOK_LPAREN, TOK_KW_INT, TOK_IDENTIFIER, TOK_COMMA,
        TOK_KW_INT, TOK_IDENTIFIER, TOK_RPAREN, TOK_MINUS_GT, TOK_KW_INT,
        TOK_LBRACE, TOK_KW_RETURN, TOK_IDENTIFIER, TOK_PLUS, TOK_IDENTIFIER, TOK_SEMICOLON, TOK_RBRACE),

    // Function with default param
    LEXER_TEST("default param", "def greet(string name = \"World\") { }",
        TOK_KW_DEF, TOK_IDENTIFIER, TOK_LPAREN, TOK_KW_STRING, TOK_IDENTIFIER,
        TOK_EQ, TOK_STRING_LITERAL, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE)
);

static test_suite suite_integration_control = TEST_SUITE("Integration - Control Flow",
    // If statement
    LEXER_TEST("if statement", "if x > 0 { y = 1; }",
        TOK_KW_IF, TOK_IDENTIFIER, TOK_GT, TOK_INT_LITERAL, TOK_LBRACE,
        TOK_IDENTIFIER, TOK_EQ, TOK_INT_LITERAL, TOK_SEMICOLON, TOK_RBRACE),

    // If-else
    LEXER_TEST("if-else", "if a { b; } else { c; }",
        TOK_KW_IF, TOK_IDENTIFIER, TOK_LBRACE, TOK_IDENTIFIER, TOK_SEMICOLON, TOK_RBRACE,
        TOK_KW_ELSE, TOK_LBRACE, TOK_IDENTIFIER, TOK_SEMICOLON, TOK_RBRACE),

    // Ternary-like if
    LEXER_TEST("ternary if", "x = if cond then a else b;",
        TOK_IDENTIFIER, TOK_EQ, TOK_KW_IF, TOK_IDENTIFIER, TOK_KW_THEN,
        TOK_IDENTIFIER, TOK_KW_ELSE, TOK_IDENTIFIER, TOK_SEMICOLON),

    // While loop
    LEXER_TEST("while loop", "while x > 0 { x -= 1; }",
        TOK_KW_WHILE, TOK_IDENTIFIER, TOK_GT, TOK_INT_LITERAL, TOK_LBRACE,
        TOK_IDENTIFIER, TOK_MINUS_EQ, TOK_INT_LITERAL, TOK_SEMICOLON, TOK_RBRACE),

    // For loop
    LEXER_TEST("for range", "for int i in 0..10 { }",
        TOK_KW_FOR, TOK_KW_INT, TOK_IDENTIFIER, TOK_KW_IN, TOK_INT_LITERAL,
        TOK_DOT_DOT, TOK_INT_LITERAL, TOK_LBRACE, TOK_RBRACE),

    // Match
    LEXER_TEST("match expression", "match state { State.Idle => x, _ => y }",
        TOK_KW_MATCH, TOK_IDENTIFIER, TOK_LBRACE,
        TOK_IDENTIFIER, TOK_DOT, TOK_IDENTIFIER, TOK_ARROW, TOK_IDENTIFIER, TOK_COMMA,
        TOK_UNDERSCORE, TOK_ARROW, TOK_IDENTIFIER, TOK_RBRACE)
);

static test_suite suite_integration_types = TEST_SUITE("Integration - Type Definitions",
    // Enum
    LEXER_TEST("simple enum", "enum Color { Red, Green, Blue }",
        TOK_KW_ENUM, TOK_IDENTIFIER, TOK_LBRACE,
        TOK_IDENTIFIER, TOK_COMMA, TOK_IDENTIFIER, TOK_COMMA, TOK_IDENTIFIER, TOK_RBRACE),

    // Enum with values
    LEXER_TEST("enum with data", "enum State { Idle, Moving(vec3) }",
        TOK_KW_ENUM, TOK_IDENTIFIER, TOK_LBRACE,
        TOK_IDENTIFIER, TOK_COMMA, TOK_IDENTIFIER, TOK_LPAREN, TOK_KW_VEC3, TOK_RPAREN, TOK_RBRACE),

    // Struct
    LEXER_TEST("struct", "struct Point { int x; int y; }",
        TOK_KW_STRUCT, TOK_IDENTIFIER, TOK_LBRACE,
        TOK_KW_INT, TOK_IDENTIFIER, TOK_SEMICOLON,
        TOK_KW_INT, TOK_IDENTIFIER, TOK_SEMICOLON, TOK_RBRACE),

    // Core
    LEXER_TEST("core", "core Player { public int health; vec3 position; }",
        TOK_KW_CORE, TOK_IDENTIFIER, TOK_LBRACE,
        TOK_KW_PUBLIC, TOK_KW_INT, TOK_IDENTIFIER, TOK_SEMICOLON,
        TOK_KW_VEC3, TOK_IDENTIFIER, TOK_SEMICOLON, TOK_RBRACE)
);

static test_suite suite_integration_shells = TEST_SUITE("Integration - Shell System",
    // Shell definition
    LEXER_TEST("shell basic", "shell Damageable { requires { int hp; } }",
        TOK_KW_SHELL, TOK_IDENTIFIER, TOK_LBRACE,
        TOK_KW_REQUIRES, TOK_LBRACE, TOK_KW_INT, TOK_IDENTIFIER, TOK_SEMICOLON,
        TOK_RBRACE, TOK_RBRACE),

    // Shell operations
    LEXER_TEST("shell push chain", "player <- Combat <- Buff;",
        TOK_IDENTIFIER, TOK_LT_MINUS, TOK_IDENTIFIER, TOK_LT_MINUS, TOK_IDENTIFIER, TOK_SEMICOLON),

    LEXER_TEST("shell remove", "player -> Frozen;",
        TOK_IDENTIFIER, TOK_MINUS_GT, TOK_IDENTIFIER, TOK_SEMICOLON),

    LEXER_TEST("shell query", "if player >- Buff { }",
        TOK_KW_IF, TOK_IDENTIFIER, TOK_GT_MINUS, TOK_IDENTIFIER, TOK_LBRACE, TOK_RBRACE)
);

static test_suite suite_integration_coroutines = TEST_SUITE("Integration - Coroutines",
    // Spawn
    LEXER_TEST("spawn coroutine", "coro[int] c = spawn task();",
        TOK_KW_CORO, TOK_LBRACKET, TOK_KW_INT, TOK_RBRACKET, TOK_IDENTIFIER, TOK_EQ,
        TOK_KW_SPAWN, TOK_IDENTIFIER, TOK_LPAREN, TOK_RPAREN, TOK_SEMICOLON),

    // Yield
    LEXER_TEST("yield value", "yield 42;",
        TOK_KW_YIELD, TOK_INT_LITERAL, TOK_SEMICOLON),

    // Yield from coro
    LEXER_TEST("yield from coro", "int result = yield c;",
        TOK_KW_INT, TOK_IDENTIFIER, TOK_EQ, TOK_KW_YIELD, TOK_IDENTIFIER, TOK_SEMICOLON)
);

static test_suite suite_integration_error_handling = TEST_SUITE("Integration - Error Handling",
    // Result type
    LEXER_TEST("result type", "def foo() -> result[int, string] { }",
        TOK_KW_DEF, TOK_IDENTIFIER, TOK_LPAREN, TOK_RPAREN, TOK_MINUS_GT,
        TOK_KW_RESULT, TOK_LBRACKET, TOK_KW_INT, TOK_COMMA, TOK_KW_STRING, TOK_RBRACKET,
        TOK_LBRACE, TOK_RBRACE),

    // Success return
    LEXER_TEST("return success", "return success(42);",
        TOK_KW_RETURN, TOK_KW_SUCCESS, TOK_LPAREN, TOK_INT_LITERAL, TOK_RPAREN, TOK_SEMICOLON),

    // Error return
    LEXER_TEST("return error", "return error(\"failed\");",
        TOK_KW_RETURN, TOK_KW_ERROR, TOK_LPAREN, TOK_STRING_LITERAL, TOK_RPAREN, TOK_SEMICOLON),

    // Error propagation
    LEXER_TEST("error propagation", "int x = try_parse()?;",
        TOK_KW_INT, TOK_IDENTIFIER, TOK_EQ, TOK_IDENTIFIER, TOK_LPAREN, TOK_RPAREN,
        TOK_QUESTION, TOK_SEMICOLON),

    // Null coalescing
    LEXER_TEST("null coalesce", "int x = maybe_val ?? 0;",
        TOK_KW_INT, TOK_IDENTIFIER, TOK_EQ, TOK_IDENTIFIER, TOK_QUESTION_QUESTION,
        TOK_INT_LITERAL, TOK_SEMICOLON)
);

static test_suite suite_integration_collections = TEST_SUITE("Integration - Collections",
    // List
    LEXER_TEST("list type", "list[int] nums;",
        TOK_KW_LIST, TOK_LBRACKET, TOK_KW_INT, TOK_RBRACKET, TOK_IDENTIFIER, TOK_SEMICOLON),

    // Dict
    LEXER_TEST("dict type", "dict[string -> int] ages;",
        TOK_KW_DICT, TOK_LBRACKET, TOK_KW_STRING, TOK_MINUS_GT, TOK_KW_INT, TOK_RBRACKET,
        TOK_IDENTIFIER, TOK_SEMICOLON),

    // Tuple
    LEXER_TEST("tuple type", "tuple[int, string] pair;",
        TOK_KW_TUPLE, TOK_LBRACKET, TOK_KW_INT, TOK_COMMA, TOK_KW_STRING, TOK_RBRACKET,
        TOK_IDENTIFIER, TOK_SEMICOLON),

    // List indexing
    LEXER_TEST("list index", "x = list[0];",
        TOK_IDENTIFIER, TOK_EQ, TOK_IDENTIFIER, TOK_LBRACKET, TOK_INT_LITERAL, TOK_RBRACKET, TOK_SEMICOLON),

    // Range
    LEXER_TEST("range inclusive", "for i in 0..=10 { }",
        TOK_KW_FOR, TOK_IDENTIFIER, TOK_KW_IN, TOK_INT_LITERAL, TOK_DOT_DOT_EQ,
        TOK_INT_LITERAL, TOK_LBRACE, TOK_RBRACE)
);

static test_suite suite_integration_imports = TEST_SUITE("Integration - Imports",
    LEXER_TEST("import module", "import math;",
        TOK_KW_IMPORT, TOK_IDENTIFIER, TOK_SEMICOLON),

    LEXER_TEST("import from", "from math import sin, cos;",
        TOK_KW_FROM, TOK_IDENTIFIER, TOK_KW_IMPORT, TOK_IDENTIFIER, TOK_COMMA,
        TOK_IDENTIFIER, TOK_SEMICOLON),

    LEXER_TEST("export function", "export def public_func() { }",
        TOK_KW_EXPORT, TOK_KW_DEF, TOK_IDENTIFIER, TOK_LPAREN, TOK_RPAREN,
        TOK_LBRACE, TOK_RBRACE)
);

//=============================================================================
// MAIN TEST RUNNER
//=============================================================================

int main(int argc, char **argv) {
    printf(ANSI_BOLD "\n╔═══════════════════════════════════════════╗\n");
    printf("║     ELECTRON LEXER TEST SUITE             ║\n");
    printf("╚═══════════════════════════════════════════╝\n" ANSI_RESET);

    test_stats stats = {0};

    // Keyword suites
    test_suite *keyword_suites[] = {
        &suite_keywords_types,
        &suite_keywords_declarations,
        &suite_keywords_control,
        &suite_keywords_operators,
        &suite_keywords_error,
        &suite_keywords_coro,
        &suite_keywords_component,
        &suite_keywords_reserved,
        &suite_keywords_literals
    };

    // Operator suites
    test_suite *operator_suites[] = {
        &suite_ops_arithmetic,
        &suite_ops_comparison,
        &suite_ops_logical,
        &suite_ops_bitwise,
        &suite_ops_assignment,
        &suite_ops_shell,
        &suite_ops_special,
        &suite_delimiters
    };

    // Literal suites
    test_suite *literal_suites[] = {
        &suite_literals_int,
        &suite_literals_float,
        &suite_literals_string,
        &suite_literals_bool
    };

    // Other suites
    test_suite *other_suites[] = {
        &suite_identifiers,
        &suite_comments,
        &suite_edge_whitespace,
        &suite_edge_operators,
        &suite_edge_numbers,
        &suite_errors
    };

    // Integration suites
    test_suite *integration_suites[] = {
        &suite_integration_declarations,
        &suite_integration_functions,
        &suite_integration_control,
        &suite_integration_types,
        &suite_integration_shells,
        &suite_integration_coroutines,
        &suite_integration_error_handling,
        &suite_integration_collections,
        &suite_integration_imports
    };

    // Run all keyword tests
    printf(ANSI_CYAN "\n━━━ KEYWORD TESTS ━━━" ANSI_RESET "\n");
    for (size_t i = 0; i < sizeof(keyword_suites)/sizeof(keyword_suites[0]); i++) {
        run_test_suite(keyword_suites[i]);
        stats.total_suites++;
        stats.total_cases += keyword_suites[i]->case_count;
        stats.passed += keyword_suites[i]->passed;
        stats.failed += keyword_suites[i]->failed;
        stats.skipped += keyword_suites[i]->skipped;
        stats.errors += keyword_suites[i]->errors;
    }

    // Run all operator tests
    printf(ANSI_CYAN "\n━━━ OPERATOR TESTS ━━━" ANSI_RESET "\n");
    for (size_t i = 0; i < sizeof(operator_suites)/sizeof(operator_suites[0]); i++) {
        run_test_suite(operator_suites[i]);
        stats.total_suites++;
        stats.total_cases += operator_suites[i]->case_count;
        stats.passed += operator_suites[i]->passed;
        stats.failed += operator_suites[i]->failed;
        stats.skipped += operator_suites[i]->skipped;
        stats.errors += operator_suites[i]->errors;
    }

    // Run all literal tests
    printf(ANSI_CYAN "\n━━━ LITERAL TESTS ━━━" ANSI_RESET "\n");
    for (size_t i = 0; i < sizeof(literal_suites)/sizeof(literal_suites[0]); i++) {
        run_test_suite(literal_suites[i]);
        stats.total_suites++;
        stats.total_cases += literal_suites[i]->case_count;
        stats.passed += literal_suites[i]->passed;
        stats.failed += literal_suites[i]->failed;
        stats.skipped += literal_suites[i]->skipped;
        stats.errors += literal_suites[i]->errors;
    }

    // Run other tests
    printf(ANSI_CYAN "\n━━━ OTHER TESTS ━━━" ANSI_RESET "\n");
    for (size_t i = 0; i < sizeof(other_suites)/sizeof(other_suites[0]); i++) {
        run_test_suite(other_suites[i]);
        stats.total_suites++;
        stats.total_cases += other_suites[i]->case_count;
        stats.passed += other_suites[i]->passed;
        stats.failed += other_suites[i]->failed;
        stats.skipped += other_suites[i]->skipped;
        stats.errors += other_suites[i]->errors;
    }

    // Run integration tests
    printf(ANSI_CYAN "\n━━━ INTEGRATION TESTS ━━━" ANSI_RESET "\n");
    for (size_t i = 0; i < sizeof(integration_suites)/sizeof(integration_suites[0]); i++) {
        run_test_suite(integration_suites[i]);
        stats.total_suites++;
        stats.total_cases += integration_suites[i]->case_count;
        stats.passed += integration_suites[i]->passed;
        stats.failed += integration_suites[i]->failed;
        stats.skipped += integration_suites[i]->skipped;
        stats.errors += integration_suites[i]->errors;
    }

    print_final_stats(&stats);

    return (stats.failed > 0 || stats.errors > 0) ? 1 : 0;
}
