// Token boundary edge cases test file

// =============================================================================
// NUMBER-IDENTIFIER BOUNDARIES
// =============================================================================

// Number followed by identifier (separate tokens)
123abc
456xyz
789test
0name
42answer

// Identifier with trailing number (single token)
abc123
xyz456
test789
name0
answer42

// Hex followed by identifier (tricky: 0xABC is valid hex)
0xGHI
0xZZZ
0x123abc

// Binary followed by identifier
0b2
0b101abc
0bxyz

// =============================================================================
// OPERATOR-IDENTIFIER BOUNDARIES
// =============================================================================

// No space
a+b
a-b
a*b
a/b
a%b
a&b
a|b
a^b

a==b
a!=b
a<b
a>b
a<=b
a>=b

a&&b
a||b
!a

// Identifier starting with operator-like chars
// (None valid - identifiers can't start with operators)

// =============================================================================
// DOT BOUNDARIES
// =============================================================================

// Member access
a.b
obj.field
player.position.x

// Range
0..10
a..b
start..end

// Float
1.0
0.5
3.14

// Float then member (tricky)
1.0.toString
3.14.toInt

// Range of floats
1.0..2.0
0.5..1.5

// Ambiguous: is this 1. .5 or 1 . .5 or 1 ..5?
1..5

// =============================================================================
// MINUS BOUNDARIES
// =============================================================================

// Subtraction
a-b
10-5

// Negative number
-42
-3.14

// Shell remove
a->b

// Decrement (if supported)
a--

// Arrow chain
a->b->c

// =============================================================================
// LESS-THAN BOUNDARIES
// =============================================================================

// Comparison
a<b

// Shell push
a<-b

// Shift
a<<b

// Force push
a<<-b

// Chained
a<b<c
a<<b<<c

// =============================================================================
// GREATER-THAN BOUNDARIES
// =============================================================================

// Comparison
a>b

// Shell query
a>-b

// Shift
a>>b

// Remove all
a->>b

// Chained
a>b>c
a>>b>>c

// =============================================================================
// EQUALS BOUNDARIES
// =============================================================================

// Assignment
a=b

// Equality
a==b

// Arrow
a=>b

// Inclusive range
a..=b

// Compound assignment
a+=b
a-=b
a*=b
a/=b

// =============================================================================
// QUESTION MARK BOUNDARIES
// =============================================================================

// Error propagation
a?

// Safe access
a?.b

// Null coalesce
a??b

// Combined
a?.b??c
a??b?.c

// =============================================================================
// COLON BOUNDARIES
// =============================================================================

// Type annotation
x:int

// Path separator
A::B

// Label
label:

// Named argument
func(name:value)

// =============================================================================
// SEMICOLON BOUNDARIES
// =============================================================================

// Statement end
a;
b;
c;

// Multiple statements
a;b;c

// Empty statement
;
;;
;;;

// =============================================================================
// BRACKET BOUNDARIES
// =============================================================================

// Tight
()
[]
{}

// With content
(a)
[b]
{c}

// Nested
((a))
[[b]]
{{c}}

// Adjacent
()()
[][]
{}{}

// Mixed
()[]{}
{[()]}
([{}])

// =============================================================================
// STRING BOUNDARIES
// =============================================================================

// Adjacent strings
"a""b"
"hello""world"

// String then identifier
"hello"world

// Identifier then string
world"hello"

// String in expression
a+"hello"+b

// =============================================================================
// COMMENT BOUNDARIES
// =============================================================================

// Comment touching code
a// comment
a/* comment */b

// Code touching comment
// comment
a

/* comment */a

// =============================================================================
// COMPLEX BOUNDARY CASES
// =============================================================================

// Multiple operators
a+-b
a-+b
a++b
a--b

// Operator then parenthesis
+(a)
-(b)
!(c)
~(d)

// Parenthesis then operator
(a)+
(b)-
(c)*

// Brackets with operators inside
[a+b]
{a-b}
(a*b)

// Complex expressions no spaces
a+b*c-d/e%f&g|h^i<<j>>k
a==b!=c<d>e<=f>=g&&h||i

// Very dense code
if(a>b){c=d;}else{e=f;}

// =============================================================================
// EOF BOUNDARIES
// =============================================================================

// Token at EOF
identifier
123
"string"
+
