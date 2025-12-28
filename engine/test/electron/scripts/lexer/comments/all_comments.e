// Single line comment test file

// =============================================================================
// SINGLE-LINE COMMENTS
// =============================================================================

// Empty comment
//

// Simple comment
// This is a comment

// Comment with leading spaces
//    indented comment

// Comment with special characters
// @#$%^&*()!~`[]{}|;:'",.<>?/\

// Comment with code-like content
// if (x > 0) { return true; }
// int x = 5;
// def func() -> int { }

// Comment with keywords
// if else while for return break continue
// true false null
// int float string bool

// Comment with operators
// + - * / % == != < > <= >= && || !
// <- -> <<- ->> >-

// Comment with numbers
// 42 3.14 0xFF 0b1010 1e10

// Comment with strings
// "hello" "world" "test"

// =============================================================================
// SINGLE-LINE COMMENTS AFTER CODE
// =============================================================================

int x = 5; // inline comment
float y = 3.14; // another inline
string s = "hello"; // string with comment

def func() { // function comment
    return 42; // return comment
} // closing brace comment

if x > 0 { // condition comment
    print("yes"); // body comment
}

// =============================================================================
// CONSECUTIVE SINGLE-LINE COMMENTS
// =============================================================================

// Line 1
// Line 2
// Line 3

//
//
//

// First block
// continues here
// and here

// Second block
// more content
// even more

// =============================================================================
// BLOCK COMMENTS
// =============================================================================

/* Empty block */

/* Simple block comment */

/* Block comment
   spanning
   multiple
   lines */

/*
 * Formatted block comment
 * with asterisks
 * on each line
 */

/**
 * Documentation-style comment
 * @param x The first parameter
 * @return The result
 */

/* Comment with special characters: @#$%^&*()!~`[]{}|;:'",.<>?/\ */

/* Comment with keywords: if else while for return int float string */

/* Comment with operators: + - * / % == != < > <= >= && || ! */

// =============================================================================
// BLOCK COMMENTS IN CODE
// =============================================================================

int /* inline */ x = 5;
float y = /* between */ 3.14;
string s = "hello" /* after string */;

def /* comment */ func(/* param comment */ int x) /* before body */ {
    return /* mid-expression */ x + /* another */ 1;
}

// =============================================================================
// NESTED BLOCK COMMENTS (if supported)
// =============================================================================

/* outer /* inner */ still outer */

/* level 1
   /* level 2
      /* level 3 */
   back to 2 */
back to 1 */

/* /* /* deeply nested */ */ */

// =============================================================================
// MIXED COMMENTS
// =============================================================================

// Line comment followed by block
/* Block comment */

/* Block comment */ // followed by line

// /* not a block start
/* // line comment inside block */

/* Multi
   // embedded line comment (treated as content)
   line */

// =============================================================================
// COMMENTS AND STRINGS
// =============================================================================

"// not a comment"
"/* also not a comment */"
"string with // slashes"
"string with /* stars */"

// "not a string"
/* "also not a string" */

// =============================================================================
// EDGE CASES
// =============================================================================

// Comment at end of file (no newline)
// EOF without newline

/* Block at end of file */

///// Many slashes
/*//* Tricky block */

/* * / not end */
/* / * not start */

// Backslash at end of line comment \
continued?

/* Contains *
 * asterisks *
 * everywhere */

/****/
/////

/* / */
/* * */
/* /* */
