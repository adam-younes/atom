// Operator disambiguation test file
// Tests lexer's ability to distinguish between similar operators

// =============================================================================
// ANGLE BRACKETS AND SHIFTS
// =============================================================================

// Less than vs left shift
a < b
a << b
a < < b
a << < b
a < << b

// Greater than vs right shift
a > b
a >> b
a > > b
a >> > b
a > >> b

// Left shift vs force shell push
a << b
a <<- b
a << -b
<<-
<< -

// Right shift vs remove all shells
a >> b
a ->> b
->>

// =============================================================================
// EQUALS VARIANTS
// =============================================================================

// Assign vs equality
a = b
a == b
a = = b
a == = b

// With not equals
a != b
a ! = b

// =============================================================================
// AMPERSAND VARIANTS
// =============================================================================

// Bitwise and vs logical and
a & b
a && b
a & & b
a && & b

// With assignment
a &= b
a & = b

// =============================================================================
// PIPE VARIANTS
// =============================================================================

// Bitwise or vs logical or
a | b
a || b
a | | b

// With assignment
a |= b
a | = b

// =============================================================================
// MINUS AND ARROW DISAMBIGUATION
// =============================================================================

// Minus vs shell remove
a - b
a -> b
a - > b

// Minus with greater
a - > b
a -> b
a - >> b
a ->> b

// Shell push
a <- b
a < - b
a < -b

// Force push
a <<- b
a << - b
a << -b

// =============================================================================
// DOT VARIANTS
// =============================================================================

// Member access vs range
a.b
a..b
a . b
a .. b

// Range vs inclusive range
a..b
a..=b
a.. =b
a ..= b

// Triple dots (not valid, should error or tokenize separately)
a...b

// =============================================================================
// QUESTION MARK VARIANTS
// =============================================================================

// Error propagation vs safe access
a?
a?.b
a? .b

// Error propagation vs null coalesce
a?
a ?? b
a? ? b
a ?? ?

// =============================================================================
// COLON VARIANTS
// =============================================================================

// Single vs double colon
a:b
a::b
a: :b
a :: b

// =============================================================================
// GREATER/LESS WITH EQUALS
// =============================================================================

// Less than vs less-equal
a < b
a <= b
a < = b

// Greater than vs greater-equal
a > b
a >= b
a > = b

// Arrow (=>) vs greater-equal
a => b
a >= b
a > = b
a = > b

// =============================================================================
// COMPLEX DISAMBIGUATION
// =============================================================================

// Multiple operators without spaces
a+b-c*d/e%f
a==b!=c<d>e<=f>=g
a&&b||c!d
a&b|c^d~e<<f>>g

// Shell operators in sequence
player <- Combat -> Buff

// Mixed operators
a <- b -> c >- d

// Shift with shell ops
x << y <- z
a ->> b >> c

// =============================================================================
// EDGE CASES
// =============================================================================

// Operator at end of line
a +
b

// Operator at start of line
a
+ b

// Multiple operators adjacent
a+-b
a-+b
a*-b
a/-b

// Parenthesized operators
(+)
(-)
(*)
(/)

// Operators with numbers
1+2
1-2
1*2
1/2
1%2

// Operators with floats (watch for dot disambiguation)
1.0+2.0
1.0-2.0
1.0*2.0
1.0/2.0
