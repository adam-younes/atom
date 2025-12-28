// Identifier test file
// Comprehensive tests for identifier lexing

// =============================================================================
// BASIC IDENTIFIERS
// =============================================================================

// Single character lowercase
a
b
c
x
y
z

// Single character uppercase
A
B
C
X
Y
Z

// Short words lowercase
foo
bar
baz
qux

// Short words uppercase
FOO
BAR
BAZ

// =============================================================================
// CASE STYLES
// =============================================================================

// camelCase
camelCase
myVariable
firstName
lastName
getUserName
setPosition
isVisible
hasChildren
canExecute

// PascalCase
PascalCase
MyClass
PlayerController
GameManager
EntitySystem
ComponentFactory

// snake_case
snake_case
my_variable
first_name
last_name
get_user_name
set_position
is_visible
has_children
can_execute

// SCREAMING_SNAKE_CASE
SCREAMING_SNAKE_CASE
MAX_VALUE
MIN_VALUE
DEFAULT_SIZE
BUFFER_LENGTH
PI
E
GRAVITY

// kebab-case (NOT valid as single identifier)
// kebab-case  // This would be: kebab MINUS case

// =============================================================================
// WITH NUMBERS
// =============================================================================

// Trailing numbers
x1
x2
player1
player2
item42
version2

// Embedded numbers
vec2
vec3
vec4
mat2x2
mat3x3
mat4x4
utf8
base64
sha256

// Multiple numbers
test123
abc123def
v1beta2
type2d3d

// All digits at end
variable123456789

// =============================================================================
// WITH UNDERSCORES
// =============================================================================

// Leading underscore
_private
_hidden
_internal
__dunder__
___triple___

// Trailing underscore
reserved_
type_
class_
for_
while_

// Multiple underscores
__init__
__main__
__name__
__doc__
__file__

// Underscores with numbers
_1
_123
var_1
var_2
_v1
_v2_

// All underscores
_
__
___
____

// =============================================================================
// SIMILAR TO KEYWORDS (but not keywords)
// =============================================================================

// Keyword prefixes extended
if_condition
else_branch
while_loop
for_each
return_value
break_here
continue_loop

// Keyword suffixes added
myif
theelse
dowhile
loopfor
toreturn
nobreak
maycontinu

// Keywords with numbers
if1
else2
while3
for4
return5

// Keywords in longer names
ifdef
ifndef
elif
endif
forever

// Similar spellings
iff
els
whil
fo
retur

// Extended type keywords
integer
floating
boolean
strings
character

// =============================================================================
// UNICODE IDENTIFIERS (if supported)
// =============================================================================

// Greek letters
alpha
beta
gamma
delta
epsilon

// With actual unicode (if supported)
// These may or may not be valid depending on language spec

// =============================================================================
// LONG IDENTIFIERS
// =============================================================================

thisIsAVeryLongIdentifierNameThatExceedsNormalLengths
THIS_IS_A_VERY_LONG_CONSTANT_NAME_IN_SCREAMING_CASE
thisIdentifierIsExtremelyLongAndShouldStillBeParsedCorrectly

// Maximum reasonable length
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa

// =============================================================================
// IDENTIFIERS IN CONTEXT
// =============================================================================

// Variable declarations
int count = 0;
float temperature = 98.6;
string firstName = "John";
bool isActive = true;

// Function names
def calculate() { }
def processData() { }
def get_value() { }
def SET_CONFIG() { }

// Type names
struct Point { }
core Player { }
shell Damageable { }
enum State { }

// Member access chains
player.position.x
entity.component.field.value
this.that.other

// Function calls
func()
myFunction(arg)
Calculate(a, b, c)
get_value()

// =============================================================================
// ADJACENT IDENTIFIERS
// =============================================================================

int x
float y
string s
bool b

// Multiple on line
a b c d e
foo bar baz

// With operators (no spaces)
a+b
x-y
c*d
e/f

// =============================================================================
// IDENTIFIERS AND NUMBERS
// =============================================================================

// Number then identifier (separate tokens)
123abc
456def
0xFFghi

// Identifier then number (separate tokens)
abc123
def456

// =============================================================================
// EDGE CASES
// =============================================================================

// Single underscore
_

// Double underscore
__

// Underscore with single char
_a
a_

// Very short
a
ab

// Starts with underscore and number
_1var
_2test

// All the same character
aaaa
AAAA
____

// Alternating case
aAaAaA
AaAaAa
