// Comprehensive operator test file
// Every operator in the Electron language

// =============================================================================
// ARITHMETIC OPERATORS
// =============================================================================
+
-
*
/
%

// In expressions
a + b
x - y
c * d
e / f
g % h

// Compound expressions
a + b - c * d / e % f

// Unary minus
-x
-42
-3.14

// =============================================================================
// COMPARISON OPERATORS
// =============================================================================
==
!=
<
>
<=
>=

// In expressions
a == b
x != y
i < j
k > l
m <= n
o >= p

// Chained comparisons
a == b && c != d
x < y && y < z

// =============================================================================
// LOGICAL OPERATORS
// =============================================================================
&&
||
!

// In expressions
a && b
x || y
!flag
!true
!false

// Complex logical
a && b || c
!a && b
a || !b

// =============================================================================
// BITWISE OPERATORS
// =============================================================================
&
|
^
~
<<
>>

// In expressions
a & b
x | y
c ^ d
~mask
value << 4
value >> 2

// Combined bitwise
(a & b) | (c ^ d)
~(x << 2)

// =============================================================================
// ASSIGNMENT OPERATORS
// =============================================================================
=
+=
-=
*=
/=
%=
&=
|=
^=
<<=
>>=

// In statements
x = 5
a += 1
b -= 2
c *= 3
d /= 4
e %= 5
f &= 0xFF
g |= 0x10
h ^= mask
i <<= 2
j >>= 1

// =============================================================================
// SHELL OPERATORS
// =============================================================================
<-
->
<<-
->>
>-

// In expressions
player <- Damageable
entity -> Frozen
obj <<- ForcePush
player ->> AllBuffs
target >- IsAlive

// Chained shell operations
player <- Combat <- Buff <- StatusEffect

// =============================================================================
// SPECIAL OPERATORS
// =============================================================================
?.
??
?
.
..
..=
=>
:
::
;
,
_
@

// Safe access
player?.health
entity?.position?.x

// Null coalescing
value ?? default
name ?? "Unknown"

// Error propagation
result?
parse_int()?

// Member access
obj.field
player.position.x

// Range operators
0..10
0..=10
start..end
a..=b

// Match arrow
case => action
x => y

// Path separator
Module::function
Enum::Variant

// Colon
label: statement
type: value

// Semicolon
statement;
a; b; c;

// Comma
a, b, c
func(x, y, z)

// Wildcard
_
match { _ => default }

// At sign
@comptime
@inline

// =============================================================================
// DELIMITERS
// =============================================================================
(
)
[
]
{
}

// Paired usage
()
[]
{}
(a)
[i]
{ x }

// Nested
(())
[[]]
{{}}
({[]})

// Function calls
func()
method(arg)
multi(a, b, c)

// Array access
arr[0]
matrix[i][j]
dict["key"]

// Blocks
{ statement; }
{
    line1;
    line2;
}
