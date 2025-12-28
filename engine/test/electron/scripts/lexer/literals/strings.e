// String literal test file
// Comprehensive tests for all string formats

// =============================================================================
// BASIC STRINGS
// =============================================================================

// Empty string
""

// Single character
"a"
"Z"
"0"
"!"
" "

// Simple words
"hello"
"world"
"Hello"
"World"
"HELLO"

// With spaces
"hello world"
"Hello, World!"
"   spaces   "

// Longer strings
"The quick brown fox jumps over the lazy dog."
"Lorem ipsum dolor sit amet, consectetur adipiscing elit."

// =============================================================================
// ESCAPE SEQUENCES
// =============================================================================

// Newline
"line1\nline2"
"first\nsecond\nthird"
"\n"
"\n\n\n"

// Tab
"col1\tcol2"
"a\tb\tc"
"\t"
"\t\t\t"

// Carriage return
"line\roverwrite"
"\r"
"\r\n"
"\n\r"

// Backslash
"\\"
"path\\to\\file"
"C:\\Users\\name"
"\\\\"

// Double quote
"\""
"say \"hello\""
"\"quoted\""
"\"\"\"triple\"\"\""

// Single quote (may not need escaping)
"'"
"it's"
"don't"

// Null character
"\0"
"null\0char"
"end\0"

// Mixed escapes
"line1\nline2\ttabbed\nline3\\backslash"
"He said \"Hello\\nWorld\""
"\t\n\r\\\"\0"

// =============================================================================
// UNICODE ESCAPES (if supported)
// =============================================================================

// Unicode codepoints
"\u0041"
"\u00E9"
"\u4E2D"
"\u{1F600}"
"\u{10FFFF}"

// Common Unicode
"\u00A9"
"\u2122"
"\u2665"

// =============================================================================
// HEX ESCAPES (if supported)
// =============================================================================

"\x00"
"\x41"
"\xFF"
"\x0A"
"\x09"

// =============================================================================
// RAW STRINGS
// =============================================================================

// Basic raw strings
r""
r"hello"
r"Hello, World!"

// Raw strings preserve backslashes
r"C:\path\to\file"
r"no\escapes\here"
r"\n is not a newline"
r"\t is not a tab"
r"\\ is two backslashes"

// Raw strings with quotes inside
r"she said "hi""

// Raw strings with special chars
r"@#$%^&*()"
r"regex: \d+\.\d+"
r"path: ..\..\.."

// =============================================================================
// MULTILINE STRINGS
// =============================================================================

// Basic multiline
"""hello"""

// Empty multiline
""""""

// Actual multiline content
"""
line 1
line 2
line 3
"""

// Multiline with indentation
"""
    indented line 1
    indented line 2
"""

// Multiline with escapes
"""
escape: \n
tab: \t
quote: \"
"""

// Multiline preserving quotes
"""She said "hello" to him."""

// Complex multiline
"""
{
    "key": "value",
    "number": 42,
    "nested": {
        "inner": true
    }
}
"""

// =============================================================================
// STRING INTERPOLATION (if supported)
// =============================================================================

"Hello, {name}!"
"The answer is {42}"
"Sum: {a + b}"
"Position: ({x}, {y})"
"{greeting}, {name}! You are {age} years old."

// Nested braces (escaping)
"Literal braces: {{}}"
"Code block: {{ let x = 1; }}"

// Complex expressions in interpolation
"Result: {calculate(a, b)}"
"Formatted: {value:.2f}"
"Conditional: {if x > 0 then "positive" else "negative"}"

// =============================================================================
// STRING EDGE CASES
// =============================================================================

// Only whitespace
"   "
"\t\t\t"
"\n\n\n"
" \t \n "

// Very long strings
"This is a very long string that goes on and on and on and on and on and on and on"

// Strings with numbers
"123"
"3.14"
"0xFF"
"1e10"

// Strings that look like code
"if (x > 0) { return true; }"
"def func() -> int { return 42; }"
"int x = 5;"

// Strings with operators
"a + b"
"x == y"
"!flag"
"a && b || c"

// Strings with keywords
"if else while for return"
"true false null"
"int float string bool"

// =============================================================================
// STRINGS IN CONTEXT
// =============================================================================

// Assignment
string s = "hello";
const msg = "world";

// Function arguments
print("Hello, World!");
log("Error: {error}");
concat("a", "b", "c");

// Comparison
if name == "admin" { }
if msg != "" { }

// Concatenation
"hello" + " " + "world"
prefix + "suffix"
"[" + content + "]"

// Array of strings
["a", "b", "c"]
["hello", "world"]

// Dictionary with string keys
{"name": "John", "age": "30"}

// =============================================================================
// ADJACENT STRINGS
// =============================================================================

"first" "second"
"a" "b" "c"
"hello" + "world"

// =============================================================================
// STRINGS AND IDENTIFIERS
// =============================================================================

"string"string
string"string"
"a"b"c"

// =============================================================================
// ERROR CASES (for error testing)
// =============================================================================

// These should cause lexer errors (comment out for normal testing)
// "unterminated
// "line break
// in string"
// "\q"
// "\xGG"
// "\u123"
