// Whitespace edge cases test file

// =============================================================================
// EMPTY AND WHITESPACE-ONLY
// =============================================================================

// This section tests minimal input




// Multiple blank lines above



// Tab only above



// Spaces only above



// Mixed whitespace above

// =============================================================================
// WHITESPACE BETWEEN TOKENS
// =============================================================================

// Single space
a b

// Multiple spaces
a    b

// Tab separation
a	b

// Multiple tabs
a			b

// Mixed whitespace
a 	 	 b

// Newline separation
a
b

// Multiple newlines
a


b

// =============================================================================
// LEADING WHITESPACE
// =============================================================================

 a
  b
   c
    d
     e

	a
		b
			c

// =============================================================================
// TRAILING WHITESPACE
// =============================================================================

a
b
c
d

a
b
c

// =============================================================================
// WHITESPACE IN EXPRESSIONS
// =============================================================================

// No whitespace
a+b
1+2
x==y

// Lots of whitespace
a    +    b
1    +    2
x    ==    y

// Asymmetric whitespace
a +b
a+ b
a +  b
a  + b

// Newlines in expressions
a +
b

a
+ b

a
+
b

// =============================================================================
// WHITESPACE AND DELIMITERS
// =============================================================================

( a )
[ b ]
{ c }

(a)
[b]
{c}

(  a  )
[  b  ]
{  c  }

// Newlines in delimiters
(
a
)

[
b
]

{
c
}

// =============================================================================
// WHITESPACE AND STRINGS
// =============================================================================

"   "
"	"
"
"
"   leading"
"trailing   "
"   both   "

// =============================================================================
// LINE CONTINUATION (if supported)
// =============================================================================

// Backslash line continuation
a + \
b

// Expression spanning lines (natural continuation)
(a +
 b +
 c)

// =============================================================================
// CARRIAGE RETURNS
// =============================================================================

a
b

// =============================================================================
// FORM FEEDS AND OTHER WHITESPACE
// =============================================================================

// Form feed (if whitespace)
ab

// Vertical tab (if whitespace)
ab

// =============================================================================
// INDENTATION PATTERNS
// =============================================================================

// Spaces only
    level1
        level2
            level3

// Tabs only
	level1
		level2
			level3

// Mixed (problematic)
	 level1
	  level2
 	  level3

// =============================================================================
// UNICODE WHITESPACE (if treated as whitespace)
// =============================================================================

// Non-breaking space
a b

// Other unicode spaces may go here

// =============================================================================
// EDGE CASES
// =============================================================================

// Just one space


// Just one tab


// Just one newline


// Whitespace before EOF
a

// No whitespace at all
abc123def
