# Electron Language Specification

**Status:** Draft
**File Extension:** `.e`

Electron is a statically-typed scripting language designed for the Atom game engine. It features a unique composition-based object model using "cores" (data) and "shells" (behavior) instead of traditional classes, enabling flexible runtime behavior modification through shell stacking.

**Architecture Overview:**
- **Entity:** Opaque engine handle with generation counter. An ID that groups components.
- **Core:** Data container. Attachable to entities as components.
- **Shell:** Behavior layer. Stacks onto cores, not entities directly.

---

## Table of Contents

1. [Lexical Structure](#1-lexical-structure)
2. [Types](#2-types)
3. [Variables and Constants](#3-variables-and-constants)
4. [Operators](#4-operators)
5. [Control Flow](#5-control-flow)
6. [Functions](#6-functions)
7. [Cores and Structs](#7-cores-and-structs)
8. [Shells](#8-shells)
9. [Enums](#9-enums)
10. [Error Handling](#10-error-handling)
11. [Coroutines](#11-coroutines)
12. [Modules and Imports](#12-modules-and-imports)
13. [Engine Integration](#13-engine-integration)
14. [Standard Library](#14-standard-library)
15. [Memory Model](#15-memory-model)
16. [Concurrency Model](#16-concurrency-model)

---

## 1. Lexical Structure

### 1.1 Comments

```electron
// Single-line comment

/*
   Multi-line
   comment
*/
```

### 1.2 Statements

All statements are terminated with a semicolon:

```electron
int x = 5;
print("Hello");
```

### 1.3 Identifiers

Identifiers must begin with a letter or underscore, followed by letters, digits, or underscores. Reserved keywords cannot be used as identifiers.

**Reserved Keywords:**
```
allows      as          bool        break       const
continue    core        coro        def         defer
dict        do          else        enum        error
export      false       float       for         from
func        if          import      in          inner
int         is          list        local       loop
match       mat2        mat3        mat4        mut
never       not         null        on_cancel   outer
public      range       readonly    requires    result
return      sealed      shell       sibling     sibling_all
spawn       strict      string      struct      success
then        true        tuple       type        unique
vec2        vec3        vec4        while       yield
yield_request
```

**Reserved for Future Use:**
```
async       await       class       extends     implements
new         private     protected   static      super
this        throw       try         catch       finally
with        sizeof
```

### 1.4 Numeric Literals

```electron
42              // Integer (decimal)
1_000_000       // Integer with separators
0xFF            // Hexadecimal (0x prefix)
0b1010          // Binary (0b prefix)
3.14            // Float
3.14f           // Explicit float
1.5e-10         // Scientific notation
```

**Note:** Octal literals are not supported. Use hexadecimal or decimal instead.

### 1.5 String Literals

Strings are immutable sequences of characters.

```electron
"Hello, World!"           // Basic string
"line1\nline2"            // Escape sequences
"tab\there"               // Tab character
"quote: \"hi\""           // Escaped quote
r"no\escapes\here"        // Raw string (no escape processing)
"""
Multiline
string
literal
"""                       // Multiline string
```

**Escape Sequences:**
| Sequence | Meaning |
|----------|---------|
| `\n` | Newline |
| `\t` | Tab |
| `\\` | Backslash |
| `\"` | Double quote |
| `\r` | Carriage return |
| `\0` | Null character |

**String Interpolation:**

```electron
string name = "Alice";
int age = 30;
string msg = "Hello, {name}! You are {age} years old.";
```

Expressions within `{}` are evaluated and converted to strings. String interpolation compiles to concatenation:

```electron
// "Hello, {name}!" compiles to:
"Hello, " + name.to_string() + "!"
```

**Important:** Interpolating nullable values is a compile error:

```electron
string? s = null;
string msg = "Hello {s}";       // Compile error: cannot interpolate nullable
string msg = "Hello {s ?? ""}"; // OK: provide default
```

**String Semantics:**

- **Equality:** String comparison (`==`, `!=`) uses value equality (content comparison), not reference equality
- **Interning:** String literals are interned at compile time (identical literals share storage). Runtime-created strings (concatenation, interpolation) are NOT automatically interned
- **Concatenation:** Creates a new string allocation. The result is not interned

**String Optimizations:**
- Compile-time folding: `"Error: " + "404"` → `"Error: 404"`
- Identical string literals are interned (share same reference)

---

## 2. Types

Electron is statically typed. All types must be explicitly declared.

### 2.1 Primitive Types

| Type | Description | Size |
|------|-------------|------|
| `int` | Signed 32-bit integer | 4 bytes |
| `float` | 32-bit floating-point number | 4 bytes |
| `bool` | Boolean value (`true` or `false`) | 1 byte |

**Integer Overflow:** Integer arithmetic wraps on overflow (two's complement).

**Note:** Only 32-bit signed integers are currently supported. Extended integer types (`int64`, `uint32`, etc.) are planned for a future release.

### 2.2 Vector and Matrix Types

These types mirror the engine's linear algebra library:

| Type | Description | Size |
|------|-------------|------|
| `vec2` | 2-component float vector | 8 bytes |
| `vec3` | 3-component float vector | 12 bytes |
| `vec4` | 4-component float vector | 16 bytes |
| `mat2` | 2x2 float matrix | 16 bytes |
| `mat3` | 3x3 float matrix | 36 bytes |
| `mat4` | 4x4 float matrix | 64 bytes |

**Construction:**

```electron
vec3 position = vec3(1.0, 2.0, 3.0);
vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
mat4 transform = mat4(1.0);  // Identity matrix
```

**Component Access:**

```electron
vec3 v = vec3(1.0, 2.0, 3.0);
float x = v.x;  // 1.0
float y = v.y;  // 2.0
float z = v.z;  // 3.0

// Swizzling (read)
vec2 xy = v.xy;
vec3 zyx = v.zyx;

// Swizzle assignment (non-aliasing patterns only)
v.xy = vec2(5.0, 6.0);  // OK
v.xx = vec2(1.0, 2.0);  // Error: 'x' appears twice in write pattern
```

**Swizzle Assignment Rules:**

1. Write patterns must not repeat components
2. Self-referential swizzles where reads and writes overlap are forbidden:

```electron
v.xz = v.zx;  // Error: reads and writes overlap (x and z)
v.xy = v.yx;  // Error: reads and writes overlap
v.xy = v.zw;  // OK: no overlap (reads z,w; writes x,y)

// Workaround for swaps:
float temp = v.x;
v.x = v.z;
v.z = temp;
```

### 2.3 Range Type

`range` is a built-in value type representing a sequence of integers.

| Type | Description | Size | Alignment |
|------|-------------|------|-----------|
| `range` | Integer range with bounds and inclusivity | 12 bytes | 4 bytes |

**Memory Layout:**
```
Offset 0-3:  start (int32)
Offset 4-7:  end (int32)
Offset 8:    inclusive (bool)
Offset 9-11: padding (3 bytes)
```

**Construction:**

```electron
range r1 = 0..10;       // Exclusive: 0, 1, 2, ..., 9
range r2 = 0..=10;      // Inclusive: 0, 1, 2, ..., 10
```

**Fields (read-only):**

```electron
range r = 5..15;
int s = r.start;        // 5
int e = r.end;          // 15
bool inc = r.inclusive; // false
```

**Usage:**

```electron
// Iteration
for i in 0..10 {
    print(i);
}

// Pattern matching
match score {
    0..60 => "F",
    60..70 => "D",
    70..80 => "C",
    80..90 => "B",
    90..=100 => "A",
    _ => "Invalid"
}

// Convert to list
list[int] nums = (0..5).to_list();  // [0, 1, 2, 3, 4]

// Check containment
if (0..100).contains(x) { }
```

`range` is a value type: copied on assignment, cannot be nullable.

### 2.4 Tuple Types

Tuples are fixed-size, heterogeneous value types. Tuples must have at least two elements.

**Declaration and Construction:**

```electron
tuple[int, int] point = (10, 20);
tuple[string, int, bool] record = ("Alice", 30, true);
```

**Access:**

Tuple element access requires a **compile-time constant** index:

```electron
int x = point[0];           // OK: literal index
int y = point[1];           // OK: literal index

@comptime const IDX = 1;
string s = record[IDX];     // OK: comptime constant

int i = get_index();
??? = point[i];             // Compile error: index must be compile-time constant
```

**Destructuring:**

```electron
tuple[int, int] (x, y) = point;
tuple[string, int, bool] (name, age, active) = record;

// Partial destructuring with wildcards
tuple[string, int, bool] (name, _, _) = record;
```

**Function Returns:**

```electron
def min_max(list[int] nums) -> tuple[int, int] {
    int min_val = nums[0];
    int max_val = nums[0];
    for n in nums {
        if n < min_val { min_val = n; }
        if n > max_val { max_val = n; }
    }
    return (min_val, max_val);
}

tuple[int, int] (lo, hi) = min_max(numbers);
```

**Parentheses vs Tuples:**

Parentheses for grouping do not create tuples:

```electron
int x = (5);           // Just 5, grouped (NOT a tuple)
int y = (5 + 3) * 2;   // Grouping for precedence

tuple[int, int] pair = (1, 2);  // Tuple (2+ elements required)
```

**Single-Element Tuple:**

Single-element tuples are not allowed, both in types and literals:

```electron
tuple[int] single;         // Compile error: tuple requires 2+ element types
tuple single = (5,);       // Compile error: no single-element tuple syntax
```

Use the element type directly instead of a single-element tuple.

Tuples are value types: copied on assignment, cannot be nullable.

### 2.5 String Type

```electron
string greeting = "Hello";
```

Strings are immutable references. Operations that modify a string return a new string.

**Character Access:**

Electron does not have a dedicated `char` type. Use string methods:

```electron
string s = "hello";
string ch = s.char_at(0);      // "h" (single-char string)
int code = s.code_at(0);       // 104 (Unicode code point)
string from_code = string.from_code(104);  // "h"

// Iterate characters:
for string ch in s.chars() { }
for int code in s.codes() { }
```

**String Interning:**

String literals and compile-time constant concatenations are interned (share storage). Runtime-created strings are not automatically interned.

```electron
string a = "hello";                    // Interned
string b = "hel" + "lo";               // Interned (compile-time concat)
string c = "hel";
string d = c + "lo";                   // NOT interned (runtime concat)

// Comparison uses value equality regardless of interning
a == d;  // true (same content)

// Explicit interning for runtime strings:
string e = string.intern(d);           // Now interned, same reference as a/b

// Identity comparison (rarely needed):
bool same_ref = string.same_ref(a, b); // true (both interned)
bool same_ref2 = string.same_ref(a, d); // false (d not interned)
```

String comparison (`==`) always uses value equality regardless of interning; interning is a memory optimization only.

### 2.6 Collection Types

**List:**

A dynamically-sized array of elements of a single type.

```electron
list[int] numbers = [1, 2, 3, 4, 5];
list[string] names = ["Alice", "Bob"];
list[vec3] points = [vec3(0, 0, 0), vec3(1, 1, 1)];
```

**Collection Type Inference:**

Collection literals require explicit element types:

```electron
list[int] nums = [1, 2, 3];    // OK: type on variable
list nums = [1, 2, 3];         // Error: element type required
list[int] empty = [];          // OK
list empty = [];               // Error

// Inferred from function parameter type:
def sum(list[int] nums) -> int { ... }
sum([1, 2, 3]);                // OK: inferred from parameter
```

**Dictionary:**

A key-value mapping. Keys must be hashable.

```electron
dict[string -> int] ages = {"alice": 30, "bob": 25};
dict[int -> string] lookup = {1: "one", 2: "two"};
```

**Hashable Types (valid as dictionary keys):**

| Type | Hashable | Notes |
|------|----------|-------|
| `int` | Yes | |
| `float` | Yes | NaN hashes consistently but NaN != NaN |
| `bool` | Yes | |
| `string` | Yes | Content-based hash |
| `range` | Yes | Hash of (start, end, inclusive) |
| Simple enum | Yes | Variants without data |
| Data enum | No | Variants with data are not hashable |
| `tuple[T...]` | Yes, if all T hashable | Recursive check |
| `struct` | Yes, if all fields hashable | Recursive check |
| `vec*`, `mat*` | Yes | Component-wise hash |
| Core | No | Reference types use identity, not value |
| `list`, `dict` | No | Mutable collections |
| `func` | No | Functions |
| Type aliases | Inherits from underlying type | |

### 2.7 Function Type

Functions are first-class values. Function variables must be declared with the `func` keyword.

**Basic Function Types:**

```electron
// Fully typed function variable
func[(int, int) -> int] adder = (int a, int b) -> int { return a + b; };

// Type annotations in the value are optional when the declaration is typed
func[(int, int) -> int] adder = (a, b) { return a + b; };

// When initialized, types can be inferred from the function definition
func adder = (int a, int b) -> int { return a + b; };

// Function that takes no arguments and returns void
func[() -> void] callback = () { print("called"); };

// Function as parameter (must be fully typed, no inference)
def apply(int x, func[(int) -> int] transform) -> int {
    return transform(x);
}

// Uninitialized function variables must have explicit types
func[(int) -> int] pending_transform;  // OK
func pending;                           // Error: type required without initialization
```

**Function Overloading:**

Electron does **not** support function overloading. Each function name must be unique within its scope:

```electron
def foo(int x) -> int { return x; }
def foo(float x) -> float { return x; }  // Error: 'foo' already defined

// Use different names instead:
def foo_int(int x) -> int { return x; }
def foo_float(float x) -> float { return x; }
```

**Local Function Type Qualifier:**

Functions that capture variables by reference (see Section 6.3) have the `local` type qualifier, which prevents them from escaping their defining scope:

```electron
int x = 10;
func[() -> void] local f = () [*x] { print(x); };  // 'local' inferred from [*x]

// Covariance: local functions CAN be passed where non-local is expected
def takes_callback(func[() -> void] cb) { cb(); }
takes_callback(f);  // OK: local can be passed to non-local parameter

// But assignment to non-local variable is an error
func[() -> void] g = f;  // Error: cannot assign local function to non-local variable

// local propagates through inference
func h = f;  // h is inferred as func[() -> void] local
```

Local functions cannot be returned, stored in fields, or stored in collections.

### 2.8 Nullable Types

By default, values cannot be `null`. Use the `?` suffix to make a reference type nullable.

**Only reference types can be nullable:**

| Category | Types | Nullable? |
|----------|-------|-----------|
| Primitives | `int`, `float`, `bool` | No |
| Value types | `vec*`, `mat*`, `range`, tuples, structs | No |
| Reference types | `string`, `list`, `dict`, cores | Yes |

```electron
int x = 5;                          // Cannot be null (primitive)
vec3 pos = vec3(0, 0, 0);           // Cannot be null (value type)
string? name = null;                // OK: string is a reference type
dict[string -> int]? cache = null;  // OK: dict is a reference type
Player? target = null;              // OK: cores are reference types

int? bad = null;                    // Compile error: primitives cannot be nullable
vec3? also_bad = null;              // Compile error: value types cannot be nullable
```

**Nullable Elements in Collections:**

```electron
list[string?] names;      // OK: string is reference type
list[Player?] players;    // OK: cores are reference types
list[int?] bad;           // Error: int is a value type
list[vec3?] also_bad;     // Error: vec3 is a value type
```

**Safe Access Operator (`?.`):**

Returns `null` if the left operand is null, otherwise accesses the member:

```electron
Player? target = get_target();
string? name = target?.name;  // null if target is null

// For primitive fields, use null coalescing to provide a default
int hp = target?.health ?? 0;  // 0 if target is null
```

**Null Coalescing Operator (`??`):**

Returns the left operand if non-null, otherwise the right operand:

```electron
string? name = null;
string display = name ?? "Unknown";  // "Unknown"
```

**Null in String Operations:**

Concatenating nullable values is a compile error:

```electron
string? s = null;
string msg = "Hello " + s;     // Compile error: cannot concat nullable

// Solutions:
string msg = "Hello " + (s ?? "");           // Explicit default
string msg = "Hello " + s.to_string_or("");  // Method approach
```

**Runtime Behavior:**

Accessing a null value without safe access (`?.`) or null check causes a panic with a stack trace.

### 2.9 Type Aliases

Create compile-time aliases for existing types:

```electron
type EntityId = int;
type Vec3List = list[vec3];
type Callback = func[(entity, float) -> bool];
type Position = vec3;
```

Type aliases are fully interchangeable with their underlying type at compile time. For distinct types that prevent accidental mixing, use cores:

```electron
core EntityId { int value; }
core PlayerId { int value; }

EntityId e = (EntityId) { value: 5 };
PlayerId p = (PlayerId) { value: 5 };
// e == p  // Compile error: different types
```

### 2.10 Generics

Electron v1.0 provides built-in generic types but does **not** support user-defined generics:

**Built-in Generics:**
- `list[T]` - Dynamic array
- `dict[K -> V]` - Hash map
- `result[T, E]` - Error handling
- `coro[T]` - Coroutine with return value
- `func[(Args) -> Ret]` - Function type

**Not Supported (v1.0):**

```electron
core Container[T] { public T value; }     // Error: user generics not supported
shell Processor[T] { }                     // Error
def identity[T](T x) -> T { return x; }   // Error
```

**Workarounds:**

```electron
// Use specific types:
core IntContainer { public int value; }
core StringContainer { public string value; }

// Use enums for type unions:
enum Value { Int(int), Float(float), String(string) }
core Container { public Value value; }
```

User-defined generics are planned for a future release.

### 2.11 Type Coercion

Type coercion is always explicit using cast syntax:

```electron
int i = 42;
float f = (float) i;      // 42.0

float pi = 3.14;
int rounded = (int) pi;   // 3
```

**Allowed Conversions:**

| From | To | Behavior |
|------|----|----------|
| `int` | `float` | Exact conversion |
| `float` | `int` | Truncation toward zero |
| `int`, `float` | `bool` | `0` / `0.0` → `false`, else `true` |
| `string` | `bool` | `""` → `false`, else `true` |
| `list[T]` | `bool` | Empty → `false`, else `true` |
| `dict[K->V]` | `bool` | Empty → `false`, else `true` |

### 2.12 Value vs Reference Semantics

| Category | Types | Semantics |
|----------|-------|-----------|
| Value | `int`, `float`, `bool`, `vec*`, `mat*`, `range`, tuples, structs | Copied on assignment and function call |
| Immutable Reference | `string` | Reference passed, but cannot be modified |
| Reference | `list`, `dict`, user-defined cores, function types | Reference passed, modifications affect original |

```electron
vec3 a = vec3(1, 2, 3);
vec3 b = a;          // Copy (value type)
b.x = 99;            // a.x still 1

list[int] x = [1, 2, 3];
list[int] y = x;     // Reference
y.push(4);           // x is now [1, 2, 3, 4]
```

### 2.13 The `never` Type

The `never` type (also called "bottom type") represents computations that never produce a value—they either diverge (loop forever) or abort (panic).

```electron
// unreachable() returns never
def process(State s) -> int {
    return match s {
        State.A => 1,
        State.B => 2,
        State.Removed => unreachable("State.Removed should never occur")
    };
}

// never coerces to any type, so it works in any context
int x = if condition then 42 else unreachable();
```

The `never` type is the return type of `unreachable()` and functions that always panic. It is a subtype of all types, allowing it to satisfy any type context.

---

## 3. Variables and Constants

### 3.1 Variable Declaration

Variables must have an explicit type:

```electron
int count = 0;
float velocity = 5.5;
string name = "Player";
vec3 position = vec3(0, 0, 0);
```

Variables can be declared without initialization (will have a default value):

```electron
int count;        // 0
float speed;      // 0.0
bool active;      // false
string text;      // ""
vec3 pos;         // vec3(0, 0, 0)
list[int] nums;   // []
```

### 3.2 Constants

Constants use the `const` keyword:

```electron
const GRAVITY = vec3(0, -9.81, 0);
const STARTUP_TIME = time.now();
const MAX_PLAYERS = 4;
```

Constants are evaluated once at program startup and cannot be reassigned.

**Compile-Time Constants:**

Use `@comptime` to require compile-time evaluation:

```electron
@comptime const MAX_ENTITIES = 65536;    // OK: literal
@comptime const PI = 3.14159;            // OK: literal
@comptime const TAU = PI * 2;            // OK: compile-time arithmetic
@comptime const BAD = vec3(0, 0, 0);     // Error: not comptime-evaluable
```

**@comptime Evaluation Rules:**

The following are valid in @comptime context:

| Operation | Allowed | Example |
|-----------|---------|---------|
| Integer/float/bool/string literals | Yes | `42`, `3.14`, `true`, `"hello"` |
| Arithmetic on comptime values | Yes | `PI * 2` |
| String concatenation of comptime | Yes | `"v" + "1"` |
| Comparison operators | Yes | `DEBUG && VERBOSE` |
| Conditional expressions | Yes | `if DEBUG then 1 else 0` |
| @comptime function calls | Yes | See below |
| Vector/matrix construction | No | Requires runtime |
| Collection literals | No | `[1,2,3]` |
| String methods | No | `"foo".length` |
| Core construction | No | |

**@comptime Functions:**

```electron
@comptime
def factorial(int n) -> int {
    return if n <= 1 then 1 else n * factorial(n - 1);
}

@comptime const FACT_10 = factorial(10);  // Computed at compile time
```

**@comptime Function Restrictions:**

| Allowed | Not Allowed |
|---------|-------------|
| Call other @comptime functions | Call non-@comptime functions |
| Reference @comptime const values | Reference mutable globals |
| Arithmetic, conditionals, recursion | Loops (use recursion instead) |
| Return primitives, string literals | Allocate (list/dict construction) |

Maximum recursion depth: 1000. This limit is tracked globally across the entire @comptime evaluation—if function A calls function B which calls function C, all three contribute to the same depth counter. This prevents deeply nested cross-module @comptime evaluations from consuming unbounded compile-time resources.

**Error Messages:**

```electron
@comptime const TOO_DEEP = factorial(1001);
// Error: "@comptime evaluation exceeded maximum recursion depth (1000)"

@comptime
def bad() -> int {
    return runtime_func();
}
// Error: "@comptime function cannot call non-@comptime function 'runtime_func'"

@comptime
def also_bad() -> int {
    return global_var;
}
// Error: "@comptime function cannot reference non-const global 'global_var'"
```

**Conditional Compilation:**

```electron
@comptime const DEBUG = true;

def update() {
    if @comptime DEBUG {
        validate_state();  // Only compiled in debug builds
    }
    // ...
}
```

### 3.3 Readonly Fields

Use `readonly` for core fields that are set at construction and never change:

```electron
core Entity {
    readonly int id;      // Set at creation, immutable after
    public int hp;        // Mutable
}

Entity e = (Entity) { id: 42, hp: 100 };
e.hp = 50;    // OK
e.id = 99;    // Error: readonly field
```

**Note:** `readonly` is only valid for core fields, not struct fields.

### 3.4 Scope

Variables are block-scoped. Inner scopes can shadow outer variables (compiler warning issued):

```electron
int x = 10;
{
    int x = 20;  // Warning: shadows outer 'x'
    print(x);    // 20
}
print(x);        // 10
```

---

## 4. Operators

### 4.1 Arithmetic Operators

| Operator | Description | Applicable Types |
|----------|-------------|------------------|
| `+` | Addition | `int`, `float`, `vec*`, `mat*`, `string` (concatenation) |
| `-` | Subtraction | `int`, `float`, `vec*`, `mat*` |
| `*` | Multiplication | `int`, `float`, `vec*`, `mat*` |
| `/` | Division | `int`, `float`, `vec*` |
| `%` | Modulo | `int`, `float` |
| `-` (unary) | Negation | `int`, `float`, `vec*` |

### 4.2 Comparison Operators

| Operator | Description |
|----------|-------------|
| `==` | Equal |
| `!=` | Not equal |
| `<` | Less than |
| `>` | Greater than |
| `<=` | Less than or equal |
| `>=` | Greater than or equal |

Core instances use reference equality by default. Override with `__eq__` in a shell.

### 4.3 Logical Operators

| Operator | Description |
|----------|-------------|
| `&&` | Logical AND (short-circuit) |
| `\|\|` | Logical OR (short-circuit) |
| `!` | Logical NOT |

### 4.4 Bitwise Operators

| Operator | Description |
|----------|-------------|
| `&` | Bitwise AND |
| `\|` | Bitwise OR |
| `^` | Bitwise XOR |
| `~` | Bitwise NOT |
| `<<` | Left shift |
| `>>` | Right shift |

**Flags Pattern:**

```electron
@comptime const FLAG_A = 1;       // 0b0001
@comptime const FLAG_B = 2;       // 0b0010
@comptime const FLAG_C = 4;       // 0b0100

int flags = FLAG_A | FLAG_C;      // 0b0101
if (flags & FLAG_A) != 0 { }      // Check flag
flags = flags | FLAG_B;           // Set flag
flags = flags & ~FLAG_A;          // Clear flag
```

### 4.5 Assignment Operators

| Operator | Equivalent |
|----------|------------|
| `=` | Assignment |
| `+=` | `a = a + b` |
| `-=` | `a = a - b` |
| `*=` | `a = a * b` |
| `/=` | `a = a / b` |
| `%=` | `a = a % b` |
| `&=` | `a = a & b` |
| `\|=` | `a = a \| b` |
| `^=` | `a = a ^ b` |
| `<<=` | `a = a << b` |
| `>>=` | `a = a >> b` |

### 4.6 Shell Operators

| Operator | Description | Return Type |
|----------|-------------|-------------|
| `<-` | Push shell onto core | core type |
| `->` | Remove first instance of shell from core | core type |
| `<<-` | Force push shell (even if already exists) | core type |
| `->>` | Remove all instances of shell from core | core type |
| `>-` | Check if core has shell attached | `bool` |

See [Section 8: Shells](#8-shells) for detailed usage.

### 4.7 Range Operators

| Operator | Description | Example |
|----------|-------------|---------|
| `..` | Exclusive range | `0..10` (0 to 9) |
| `..=` | Inclusive range | `0..=10` (0 to 10) |

### 4.8 Other Operators

| Operator | Description |
|----------|-------------|
| `?.` | Safe member access (null-safe) |
| `??` | Null coalescing |
| `?` | Error propagation (postfix) |
| `[]` | Index access |
| `.` | Member access |
| `()` | Function call |

---

## 5. Control Flow

### 5.1 Conditional Statements

**If/Else:**

```electron
if x > 0 {
    print("positive");
} else if x < 0 {
    print("negative");
} else {
    print("zero");
}
```

**Conditional Expression (Ternary):**

```electron
string result = if condition then "yes" else "no";
int abs_val = if x >= 0 then x else -x;
```

### 5.2 Loops

**While Loop:**

```electron
while condition {
    // body
}
```

**Do-While Loop:**

```electron
do {
    // body executes at least once
} while condition;
```

**Infinite Loop:**

```electron
loop {
    // runs forever until break
    if should_exit {
        break;
    }
}
```

**For-In Loop (Collections):**

```electron
list[int] numbers = [1, 2, 3, 4, 5];
for int num in numbers {
    print(num);
}

// With index
for int i, int num in numbers {
    print("Index {i}: {num}");
}

// Dictionary iteration
dict[string -> int] ages = {"alice": 30, "bob": 25};
for string key, int value in ages {
    print("{key} is {value} years old");
}

// Keys only
for string key in ages {
    print(key);
}

// Range iteration
for int i in 0..10 {
    print(i);  // 0, 1, 2, ..., 9
}

for int i in 0..=10 {
    print(i);  // 0, 1, 2, ..., 10
}

// Tuple destructuring in loops
list[tuple[string, int]] pairs = [("a", 1), ("b", 2), ("c", 3)];
for tuple[string, int] (name, value) in pairs {
    print("{name}: {value}");
}

// With index
for int i, tuple[string, int] (name, value) in pairs {
    print("{i}: {name} = {value}");
}

// Complex tuple types
list[tuple[string, float, func[(int) -> void]]] callbacks = [];
for tuple[string, float, func[(int) -> void]] (name, delay, action) in callbacks {
    schedule(delay, action);
}
```

**Loop Control:**

```electron
for Item item in items {
    if item.skip {
        continue;  // Skip to next iteration
    }
    if item.done {
        break;     // Exit loop entirely
    }
    process(item);
}
```

### 5.3 Labeled Loops

Labels allow breaking or continuing outer loops:

```electron
outer: for int i in 0..10 {
    for int j in 0..10 {
        if should_exit(i, j) {
            break outer;  // Exit both loops
        }
        if should_skip_row(i, j) {
            continue outer;  // Skip to next i
        }
    }
}
```

Labels are identifiers followed by a colon before a loop statement.

### 5.4 Defer Statement

`defer` schedules a statement to execute when the current scope exits:

```electron
def process_file(string path) -> result[Data, Error] {
    File f = open(path)?;
    defer f.close();  // Always runs when scope exits

    Buffer b = allocate(1024);
    defer free(b);

    if check_header(f) == false {
        return error((Error) { message: "Invalid header" });
        // f.close() and free(b) run here
    }

    Data d = parse(f, b)?;
    return success(d);
    // f.close() and free(b) run here
}
```

**Semantics:**

- Multiple defers execute in reverse order (LIFO)
- Defers run on normal exit, early return, and error propagation (`?`)
- Defers do **not** run on panic

**Defer and Coroutines:**

In coroutines, `defer` runs on completion or cancellation, but **not** on yield:

```electron
def my_coroutine() -> coro {
    Resource r = acquire();
    defer release(r);       // Runs when coroutine completes OR is cancelled

    yield wait(1.0);        // defer does NOT run here (yield is suspend, not exit)
    yield wait(1.0);        // defer does NOT run here either

    // defer runs here on normal completion
}

// If cancelled:
coro c = spawn my_coroutine();
c.cancel();  // defer runs here (cancellation is an exit)
```

| Event | `defer` runs? | Rationale |
|-------|---------------|-----------|
| `yield` | No | Suspend, not exit—coroutine may resume |
| Normal completion | Yes | Function exits |
| Cancellation | Yes | Function exits (abnormally) |
| Panic | No | Consistent with non-coroutine behavior |

This matches Go's behavior (defer runs on function return, not goroutine suspend) and Rust's Drop semantics (runs when value goes out of scope, not on async yield).

**Restrictions:**

`defer` blocks cannot contain control flow statements:

```electron
def foo() -> int {
    defer { return 5; }    // Error: return not allowed in defer
    defer { break; }       // Error: break not allowed in defer
    defer { yield; }       // Error: yield not allowed in defer
    defer { defer x; }     // Error: nested defer not allowed
    return 10;
}
```

```electron
def example() {
    defer print("first");   // Runs third
    defer print("second");  // Runs second
    defer print("third");   // Runs first
}
// Output: third, second, first
```

### 5.5 Pattern Matching

The `match` expression provides exhaustive pattern matching:

```electron
match value {
    0 => print("zero"),
    1 => print("one"),
    _ => print("other")
}
```

**Pattern Types:**

```electron
// Literal patterns
match x {
    42 => "answer",
    0 => "nothing",
    _ => "something else"
}

// Range patterns
match score {
    0..60 => "F",
    60..70 => "D",
    70..80 => "C",
    80..90 => "B",
    90..=100 => "A",
    _ => "Invalid"
}

// Guard patterns
match x {
    n if n < 0 => "negative",
    n if n > 100 => "large",
    n => "normal: {n}"
}

// Guards with multiple conditions (short-circuit evaluation applies)
match value {
    Some(x) if x > 0 && expensive() => ...,  // expensive() only called if x > 0
}

// Destructuring patterns
match point {
    vec3(0, 0, 0) => "origin",
    vec3(x, 0, z) => "on xz plane at ({x}, {z})",
    vec3(x, y, z) => "at ({x}, {y}, {z})"
}

// Tuple destructuring
match pair {
    (0, 0) => "origin",
    (x, 0) => "on x-axis",
    (0, y) => "on y-axis",
    (x, y) => "at ({x}, {y})"
}

// List destructuring
match items {
    [] => "empty",
    [single] => "one item: {single}",
    [first, ...rest] => "first: {first}, remaining: {rest.length}",
}

// Enum variant patterns (see Section 9)
match state {
    State.Idle => "waiting",
    State.Walking => "moving",
    State.Attacking(dmg) => "dealing {dmg} damage",
    State.Dead => "game over"
}

// Result patterns
match res {
    success(value) => use_value(value),
    error(e) => handle_error(e)
}
```

**List Destructuring Rest Patterns:**

Rest patterns capture remaining elements as `list[T]`. Only trailing rest is allowed:

```electron
match items {
    []                       => "empty",
    [only]                   => "single: {only}",
    [first, second]          => "exactly two",
    [first, ...rest]         => "first={first}, rest has {rest.length} items",
    [first, second, ...rest] => "at least two",
}

// If items: list[int], then rest: list[int]

// INVALID patterns:
[...front, last]           // Error: rest must be trailing
[first, ...middle, last]   // Error: rest must be trailing
[...a, ...b]               // Error: multiple rest patterns
```

**Exhaustiveness:**

Match expressions must be exhaustive. Guards do not count toward exhaustiveness:

```electron
// Error: not exhaustive (guards don't count)
match x {
    n if n < 0 => "negative",
    n if n >= 0 => "non-negative"
}

// Correct: unguarded fallback
match x {
    n if n < 0 => "negative",
    n => "non-negative"  // Catches everything else
}
```

**Data Enum Exhaustiveness:**

Patterns with literal values on data enum variants don't contribute to exhaustiveness:

```electron
enum State { Idle, Attacking(int) }

// NOT exhaustive - Attacking(0) is just one value:
match state {
    State.Idle => ...,
    State.Attacking(0) => ...,
}  // Error: non-exhaustive

// Exhaustive - capture variable matches all:
match state {
    State.Idle => ...,
    State.Attacking(dmg) => ...,  // dmg captures ANY int
}
```

**Guard Semantics:**

Pattern bindings are visible within the guard expression. Guards use standard short-circuit evaluation (`&&` and `||`):

```electron
match value {
    Some(x) if x > 0 => ...,           // x is bound, visible in guard
    (a, b) if a == b => ...,           // Multiple bindings visible
    n if n > 0 && expensive() => ...,  // Short-circuit: expensive() only if n > 0
}

// Evaluation order:
// 1. Pattern match attempted
// 2. If pattern matches, bindings are created
// 3. Guard evaluated left-to-right with short-circuit
// 4. If guard is true, arm executes
// 5. If guard is false, bindings are discarded, next arm tried
```

**The `is` Pattern Expression:**

For single-pattern checks, use `is` instead of a full `match`:

```electron
result[int, string] res = compute();

// Pattern: expr 'is' pattern
if res is success(value) {
    use(value);  // value is bound in this scope only
}

if res is error(msg) {
    print(msg);
}

// Works with any pattern
if point is vec3(0, 0, z) {
    print("On z-axis at {z}");
}

if state is State.Attacking(dmg) {
    print("Attacking with {dmg} damage");
}

// Negation
if res is not error(_) {
    // Success case
}
```

The `is` expression binds pattern variables only within the `if` block scope.

---

## 6. Functions

### 6.1 Function Declaration

```electron
def function_name(type1 param1, type2 param2) -> return_type {
    // function body
    return value;
}
```

**Void Functions:**

Omit the return type for functions that return nothing:

```electron
def greet(string name) {
    print("Hello, {name}!");
}
```

**Default Parameters:**

Default parameters are evaluated **left-to-right, once per call**:

```electron
def spawn_enemy(vec3 position, int health = 100, float scale = 1.0) {
    // ...
}

spawn_enemy(vec3(0, 0, 0));              // health=100, scale=1.0
spawn_enemy(vec3(0, 0, 0), 50);          // health=50, scale=1.0
spawn_enemy(vec3(0, 0, 0), 50, 2.0);     // health=50, scale=2.0
```

**Named Arguments:**

```electron
def spawn_enemy(
    vec3 position,
    int health = 100,
    float scale = 1.0,
    string name = "Enemy",
    bool boss = false
) { }

// Positional
spawn_enemy(vec3(0, 0, 0), 50, 2.0, "Goblin", false);

// Named (can be in any order after positional args)
spawn_enemy(vec3(0, 0, 0), name: "Dragon", boss: true, health: 500);

// All named
spawn_enemy(position: vec3(0, 0, 0), boss: true);
```

Rules:
- Named arguments must come after all positional arguments
- Cannot mix: `foo(x: 1, 2)` is an error
- Names must match parameter names exactly

### 6.2 First-Class Functions

Functions are first-class values with explicit function types:

```electron
def add(int a, int b) -> int {
    return a + b;
}

func operation = add;  // Type inferred from add
int result = operation(2, 3);  // 5

// Function as parameter
def apply(func[(int) -> int] f, int x) -> int {
    return f(x);
}

// Storing in collections
list[func[(int) -> int]] transforms = [
    (int x) -> int { return x * 2; },
    (int x) -> int { return x + 1; }
];
```

### 6.3 Closures (Anonymous Functions)

```electron
func doubler = (int x) -> int {
    return x * 2;
};

func greeter = (string name) {
    print("Hello, {name}!");
};

// Inline usage
list[int] doubled = numbers.map((int x) -> int { return x * 2; });
```

**Return Type Inference:**

Closure return types are inferred from `return` statements when omitted:

```electron
// Explicit return type
func f1 = (int x) -> int { return x * 2; };

// Inferred return type (all returns must have same type)
func f2 = (int x) { return x * 2; };          // Inferred: -> int
func f3 = (int x) { return x.to_string(); };  // Inferred: -> string

// No returns = void
func f4 = (int x) { print(x); };              // Inferred: -> void

// Multiple returns must agree
func f5 = (int x) {
    if x > 0 { return "positive"; }
    return "non-positive";                     // OK: both return string
};

func f6 = (int x) {
    if x > 0 { return x; }
    return "negative";                         // Error: int vs string
};

// Nullable inference only for reference types
func f7 = (Player? p) {
    if p == null { return null; }
    return p.name;                             // Inferred: -> string? (nullable)
};

// Result type inference requires explicit annotation
func f8 = (int x) -> result[int, string] {
    if x < 0 { return error("negative"); }
    return success(x * 2);                     // OK: explicit return type
};

func f9 = (int x) {
    if x < 0 { return error("negative"); }
    return success(x * 2);                     // Error: cannot infer result type
};
// The error type cannot be inferred from success(), and vice versa.
// Always provide explicit return types for closures returning result.
```

**Capturing Variables:**

By default, closures capture variables by value (snapshot at creation time):

```electron
int x = 10;
func f = () {
    print(x);
};
x = 20;
f();  // Prints 10 (captured value at creation)
```

To capture by reference, use `[*variable]`:

```electron
int x = 10;
func f = () [*x] {
    print(x);
};
x = 20;
f();  // Prints 20 (reference to x)
```

Multiple captures:

```electron
int a = 1;
int b = 2;
func f = () [*a, *b] {
    print(a + b);
};
```

**Reference Capture Restrictions (Local Functions):**

Closures with reference captures have the `local` type qualifier (see Section 2.7), which prevents them from escaping their defining scope:

```electron
// 'local' is automatically inferred for [*var] captures
int x = 10;
func[() -> void] local f = () [*x] { print(x); };  // 'local' inferred

// OK: closure used within same scope
def process(list[int] items) {
    int sum = 0;
    items.for_each((int x) [*sum] {
        sum += x;
    });
    print(sum);
}

// OK: stored in local variable and used in same scope
def example() {
    int x = 10;
    func f = () [*x] { print(x); };  // f has type func[() -> void] local
    f();      // OK
    f();      // OK
}

// Error: reference capture cannot escape
def make_counter() -> func[() -> int] {
    int count = 0;
    return () [*count] -> int {  // Error: cannot return local function
        count += 1;
        return count;
    };
}
```

**Local Function Enforcement Rules:**

| Allowed | Not Allowed |
|---------|-------------|
| Pass to function parameters | Return from functions |
| Store in local variables | Store in core/struct fields |
| Call immediately | Store in collections |

```electron
func[() -> void] local f = () [*x] { };

// Covariance: local can be passed to non-local parameters
def takes_callback(func[() -> void] cb) { cb(); }
takes_callback(f);  // OK

// But assignment to non-local is an error
func[() -> void] g = f;  // Error: cannot assign local to non-local
```

To create escaping closures with mutable state, use a core:

```electron
core Counter {
    public int count;
}

def make_counter() -> func[() -> int] {
    Counter c = (Counter) { count: 0 };
    return () -> int {  // Value capture of reference type
        c.count += 1;
        return c.count;
    };
}
```

**yield in Closures:**

`yield` is only valid inside functions returning `coro` or `coro[T]`. Closures, even those defined inside coroutines, cannot contain `yield`:

```electron
def not_a_coro() {
    yield wait(1.0);    // Compile error: yield outside coroutine
}

def valid_coro() -> coro {
    yield wait(1.0);    // OK
}

def outer_coro() -> coro {
    func f = () {
        yield;          // Error: closure is not a coroutine
    };
    yield;              // OK: in coroutine
}
```

**Workaround for Async Iteration:**

Use explicit `for` loops or delegate to sub-coroutines:

```electron
def my_coro() -> coro {
    list[int] items = [1, 2, 3];

    // Option 1: Explicit loop (recommended)
    for int x in items {
        yield wait(1.0);
        process(x);
    }

    // Option 2: Delegate to sub-coroutine
    yield process_with_delay(items, 1.0);
}

def process_with_delay(list[int] items, float delay) -> coro {
    for int x in items {
        process(x);
        yield wait(delay);
    }
}
```

### 6.4 Forward References

Functions can reference other functions defined later in the file. The compiler uses two-pass resolution:

```electron
def foo() {
    bar();  // OK: bar is defined below
}

def bar() {
    print("bar");
}
```

---

## 7. Cores and Structs

Electron provides two aggregate data types with different use cases:

| Feature | Core | Struct |
|---------|------|--------|
| Semantics | Reference | Value |
| Shell attachment | Yes | No |
| Nullable | Yes | No |
| Max size | Unlimited | 64 bytes |
| Passed to functions | By reference | By copy (unless `*`) |
| Entity attachment | Yes | No |
| Use case | Game objects, components | Small data (colors, rects, AABBs) |

**When to use which:**
- **Core:** Anything that needs behavior (shells), identity, or is part of the ECS
- **Struct:** Small, behavior-less data bundles passed by value (like GLSL's vec types)

### 7.1 Core Declaration

Cores are data structures similar to C structs. They hold state but define no behavior.

```electron
core Entity {
    int id;
    vec3 position;
    vec3 velocity;
    float health;
}
```

### 7.2 Access Modifiers

Fields are **private by default**. Use `public` for external access:

```electron
core Player {
    public int health;
    public vec3 position;
    int internal_state;      // Private: only accessible from attached shells
}
```

Use `readonly` for fields set once at construction:

```electron
core Entity {
    readonly int id;         // Set at creation, immutable
    public int hp;
}
```

### 7.3 Instantiation

Core literals use cast-style syntax with the type in parentheses:

```electron
Player p = (Player) {
    health: 100,
    position: vec3(0, 0, 0)
};

// Positional syntax (must match declaration order)
Player p2 = (Player) { 100, vec3(0, 0, 0) };

// Anonymous core literal only in typed context
Player p3 = { health: 100, position: vec3(0, 0, 0) };  // OK: type known
do_something({ health: 50 });  // Error: ambiguous, use (Player) { health: 50 }
do_something((Player) { health: 50 });  // OK: explicit
```

**Anonymous Literal Type Inference:**

Type is inferred from context: variable declaration, function parameter, collection element type, or return type.

```electron
// Variable declaration
Player p = { health: 100 };                    // OK: inferred from Player

// Function parameter
def spawn(Player p) { }
spawn({ health: 100 });                        // OK: inferred from parameter type

// Collection element
list[Player] team = [{ health: 100 }, { health: 80 }];  // OK: inferred from list[Player]

// Return type
def make_player() -> Player {
    return { health: 100 };                    // OK: inferred from return type
}

// Ambiguous contexts require explicit cast
list items = [{ health: 100 }];               // Error: element type unknown
dict[string -> ???] d = {"p": { health: 100 }}; // Error: value type unknown
```

**Field Initialization Order:**

Fields initialize in declaration order. Later fields can reference earlier ones:

```electron
core Foo {
    int a = 5;
    int b = a + 1;      // OK: a is initialized first
    int c = b * 2;      // OK: b is initialized
    int d = e;          // Error: e not yet initialized
    int e = 10;
}

// Explicit construction overrides defaults:
Foo f = (Foo) { a: 10, b: 20 };  // a=10, b=20 (not a+1), c=40, e=10
```

**Initialization Semantics:**

When constructing a core, fields are processed in declaration order:
1. If a field has an explicit value in the literal, that value is used
2. If a field has a default expression, it is evaluated using the *current* values of earlier fields (which may be explicitly provided or defaulted)
3. Default expressions are re-evaluated at construction time, not cached

```electron
// Given the Foo definition above:
Foo f1 = (Foo) { };               // a=5, b=6, c=12, e=10 (all defaults)
Foo f2 = (Foo) { a: 10 };         // a=10, b=11, c=22, e=10 (b uses new a)
Foo f3 = (Foo) { a: 10, b: 20 };  // a=10, b=20, c=40, e=10 (c uses new b)
Foo f4 = (Foo) { c: 100 };        // a=5, b=6, c=100, e=10 (c explicitly set)
```

### 7.4 Nested Cores

Cores can contain other cores:

```electron
core Transform {
    vec3 position;
    vec3 rotation;
    vec3 scale;
}

core GameObject {
    int id;
    Transform trans;
    string name;
}

GameObject obj = (GameObject) {
    id: 1,
    trans: (Transform) {
        position: vec3(0, 0, 0),
        rotation: vec3(0, 0, 0),
        scale: vec3(1, 1, 1)
    },
    name: "Player"
};
```

**Self-Referential Cores:**

Cores can reference their own type, enabling recursive data structures like trees and graphs:

```electron
core TreeNode {
    public int value;
    public TreeNode? left;   // Nullable self-reference
    public TreeNode? right;
}

core LinkedListNode {
    public int data;
    public LinkedListNode? next;
}

// Usage
TreeNode root = (TreeNode) {
    value: 10,
    left: (TreeNode) { value: 5, left: null, right: null },
    right: (TreeNode) { value: 15, left: null, right: null }
};
```

Self-references must be nullable (cores are reference types, so this works without special indirection). Circular references are allowed and handled by the garbage collector.

### 7.5 Core Methods

Cores themselves cannot have methods. All behavior is defined in shells that attach to cores.

### 7.6 Structs

Structs are small, stack-allocated data structures with value semantics.

**Declaration:**

```electron
struct Color {
    public float r;
    public float g;
    public float b;
    public float a;
}

struct Rect {
    public float x;
    public float y;
    public float width;
    public float height;
}

struct AABB {
    public vec3 min;
    public vec3 max;
}
```

**Constraints:**

| Constraint | Rationale |
|------------|-----------|
| No shell attachment | Shells require reference semantics |
| Cannot be nullable | Nullable implies reference; value types always exist |
| Copied on assignment | Value semantics, like `int` or `vec3` |
| Copied when passed to functions | Unless ref parameter |
| Can contain only value types | No `string`, `list`, or reference cores |
| No recursive/self-referential | Would require indirection |
| No `readonly` keyword | Value types are always copied; readonly is meaningless |

**Size Considerations:**

Structs have no hard size limit (unlike some earlier language proposals). However, the compiler issues a warning for large structs since they are copied on every assignment and function call:

```electron
struct Transform {
    mat4 matrix;    // 64 bytes
    bool dirty;     // 1 byte + padding
}                   // ~68 bytes - OK, but warning issued

struct Example {
    int a;          // 4 bytes
    int b;          // 4 bytes
    float c;        // 4 bytes
    vec3 d;         // 12 bytes
}                   // Total: 24 bytes (no warning)
```

| Struct Size | Behavior |
|-------------|----------|
| ≤ 64 bytes | No warning |
| 65-256 bytes | Warning: "Large struct 'T' (N bytes) will be copied on assignment. Consider using a core for reference semantics." |
| > 256 bytes | Warning: "Very large struct 'T' (N bytes). Strongly consider using a core." |

Developers can suppress warnings with `@allow(large_struct)` if copying is intentional. Use cores for large data that should be passed by reference.

**Struct Field Visibility:**

Struct fields follow the same visibility rules as cores. Private fields are accessible within the same module:

```electron
struct Color {
    public float r;
    public float g;
    public float b;
    float internal_alpha;  // Private: same-module access only
}

// In same module:
Color c = (Color) { r: 1.0, g: 0.0, b: 0.0, internal_alpha: 1.0 };
float a = c.internal_alpha;  // OK

// In different module:
Color c = (Color) { r: 1.0, g: 0.0, b: 0.0 };  // internal_alpha = 0.0 (default)
float a = c.internal_alpha;  // Error: private field
```

**Reference Parameters:**

Structs can be passed by reference to avoid copying:

```electron
// By value (copy)
def set_color(Color c) {
    // c is a copy, modifications don't affect caller
}

// By reference (read-only)
def read_color(*Color c) {
    print(c.r);
    // c.r = 1.0;  // Error: ref is read-only
}

// By mutable reference
def modify_color(mut *Color c) {
    c.r = 1.0;  // OK: modifies caller's value
}

Color col = (Color) { r: 1.0, g: 0.0, b: 0.0, a: 1.0 };
set_color(col);           // Copies col
read_color(*col);         // Passes reference
modify_color(mut *col);   // Passes mutable reference
```

**In Collections:**

```electron
list[Color] palette = [
    (Color) { r: 1.0, g: 0.0, b: 0.0, a: 1.0 },
    (Color) { r: 0.0, g: 1.0, b: 0.0, a: 1.0 },
];

// Accessing copies the value out
Color c = palette[0];
c.r = 0.5;              // Does NOT modify palette[0]

// Modify in place with index assignment
palette[0].r = 0.5;     // Modifies palette[0] directly
```

**Associated Functions Convention:**

Since structs cannot have methods, use module-level functions with a `Type_` prefix:

```electron
// In structs/color.e

export struct Color {
    public float r;
    public float g;
    public float b;
    public float a;
}

// Associated functions use Type_ prefix
export def Color_from_rgb(float r, float g, float b) -> Color {
    return (Color) { r: r, g: g, b: b, a: 1.0 };
}

export def Color_from_hex(int hex) -> Color {
    return (Color) {
        r: ((hex >> 16) & 0xFF) / 255.0,
        g: ((hex >> 8) & 0xFF) / 255.0,
        b: (hex & 0xFF) / 255.0,
        a: 1.0
    };
}

export def Color_lerp(Color a, Color b, float t) -> Color {
    return (Color) {
        r: lerp(a.r, b.r, t),
        g: lerp(a.g, b.g, t),
        b: lerp(a.b, b.b, t),
        a: lerp(a.a, b.a, t)
    };
}

// Usage:
Color red = Color_from_hex(0xFF0000);
Color orange = Color_lerp(red, yellow, 0.5);
```

---

## 8. Shells

Shells define behavior that can be attached to cores at runtime. They form a stack, with the topmost shell's methods taking precedence.

### 8.1 Shell Declaration

```electron
shell Movable {
    def move(float dx, float dy) {
        core.position.x += dx;
        core.position.y += dy;
    }

    def teleport(vec3 destination) {
        core.position = destination;
    }
}
```

**The `core`, `inner`, and `outer` Keywords:**

Within a shell, three special keywords are available:

| Keyword | Type | Description |
|---------|------|-------------|
| `core` | interface type | Reference to the attached core instance. Type-checked against the shell's requirements. |
| `inner` | shell reference | Reference to shells below in the stack. Used for delegation toward the core. |
| `outer` | shell reference | Reference to shells above in the stack. Used for upward service lookup. |

The `core` keyword provides access to fields declared in the shell's `requires` block. The compiler type-checks `core` against the requirements:

```electron
shell Movable {
    requires { vec3 position }

    def move(float dx, float dy) {
        core.position.x += dx;    // OK: position is in requirements
        core.health -= 10;         // Compile error: health not in requirements
    }
}
```

**Built-in Core Properties:**

Every core has access to its owning entity:

```electron
shell EntityAware {
    def get_entity() -> entity {
        return core.entity;  // Built-in: every core knows its entity
    }
}
```

### 8.2 Shell Constraints

Shells declare required fields via a `requires` block:

```electron
shell Movable {
    requires { vec3 position, vec3 velocity }

    def move(float dx, float dy) {
        core.position.x += dx;
        core.position.y += dy;
    }
}
```

This shell can only attach to cores with `position` and `velocity` fields of type `vec3`.

**Requirement Checking:**

Shell requirements are verified at attachment time (runtime). The compiler performs best-effort static analysis when the core type is statically known:

```electron
core Enemy { int hp; }
shell Movable { requires { vec3 position } }

Enemy e = (Enemy) { hp: 100 };
e <- Movable;  // Compile error: Enemy lacks 'position: vec3'
```

When the core type is dynamic (e.g., from entity lookup), checking defers to runtime:

```electron
// Runtime panic if core lacks required fields:
// "Cannot attach shell 'Movable' to core: missing required field 'position' of type 'vec3'"
```

**Shell Interface Privacy:**

Shells can access private core fields through their `requires` block. Privacy is for external code, not shells:

```electron
core Player {
    int internal_state;  // Private
    public int hp;
}

shell Debugger {
    requires { int internal_state }  // OK: shells can access private fields

    def dump() {
        print(core.internal_state);
    }
}
```

**Entity Requirements and Sibling Access:**

Shells can declare that other core types must exist on the same entity using `requires entity`:

```electron
shell PlayerController {
    requires { vec3 position, int hp }           // Fields on THIS core
    requires entity { Inventory, Equipment }     // Other cores on same entity

    def update(float dt) {
        // core.* accesses attached core
        core.position += get_input() * dt;

        // sibling[T] is shorthand for core.entity![T] when T is in requires entity
        Inventory inv = sibling[Inventory];     // Never null (guaranteed by requires)
        Equipment eq = sibling[Equipment];      // Never null

        // sibling[T]? is shorthand for core.entity[T] for optional lookups
        AIController? ai = sibling[AIController]?;  // Nullable (not in requires)

        // Method calls on sibling go through that core's shell stack
        inv.add_item(loot);  // Calls through Inventory's shell stack
    }
}
```

| Syntax | Equivalent | Behavior |
|--------|------------|----------|
| `sibling[T]` | `core.entity![T]` | Returns core, panics if missing. Only valid when `T` in `requires entity` |
| `sibling[T]?` | `core.entity[T]` | Returns nullable core. Valid for any core type |
| `sibling_all[T]` | (no shorthand) | Returns `list[T]` of all cores of type T on entity. Returns empty list `[]` if none exist |

**Dressed Core Semantics:**

The `sibling` keyword returns a "dressed" core—the core instance with its attached shell stack intact. Method calls dispatch through the shell stack normally, just as if you called the method on a direct reference to that core:

```electron
shell InventoryManager {
    requires entity { Inventory }

    def transfer_item(int slot) {
        Inventory inv = sibling[Inventory];  // Returns dressed core
        inv.add_item(get_item(slot));        // Dispatches through Inventory's shells
        // NOT raw field access—shells like ItemValidator, StackManager, etc. intercept
    }
}
```

This matches Unity's `GetComponent<T>()` and Godot's node references, where returned components/nodes include all attached behaviors. Bypassing the shell stack would defeat the purpose of the shell model.

### 8.3 Shell State

Shells can have their own state, stored per-attachment. State initializes **at push time**:

```electron
shell Cooldown {
    float time_remaining = 0.0;  // Initialized when shell is pushed
    float started_at = time.now();  // Also at push time

    def start(float duration) {
        time_remaining = duration;
    }

    def update(float dt) {
        if time_remaining > 0 {
            time_remaining -= dt;
        }
    }

    def is_ready() -> bool {
        return time_remaining <= 0;
    }
}
```

Each attachment of `Cooldown` to a core gets its own `time_remaining`.

### 8.4 Attaching and Removing Shells

All shell operations use operators.

**Push (Attach):**

```electron
Player player = (Player) { hp: 100, position: vec3(0, 0, 0) };

player <- Movable;          // Push Movable shell
player <- Controllable;     // Push Controllable (now on top)

// Force push: creates new instance even if already attached
player <<- Buff;
player <<- Buff;  // Player now has two Buff instances
```

**Pop (Remove):**

```electron
player -> Controllable;     // Remove first instance of Controllable
player ->> Buff;            // Remove ALL instances of Buff
```

**Query:**

```electron
if player >- Frozen {
    show_frozen_indicator();
}
```

**Chaining:**

Shell operators return the core, enabling chaining:

```electron
player <- Movable <- Controllable <- Damageable;
player -> Frozen ->> Buff;
```

**Chaining Semantics (Atomic with Rollback):**

Shell chains are evaluated left-to-right with atomic rollback on failure:

```electron
player <- A <- B <- C;
// Evaluation:
// 1. Validate A's requirements against player
// 2. Attach A
// 3. Validate B's requirements (including requires inner)
// 4. Attach B
// 5. Validate C's requirements
// 6. Attach C
// If step 5 fails: remove B, remove A, then panic
```

| Scenario | Behavior |
|----------|----------|
| All succeed | All shells attached in order |
| Middle fails (e.g., B) | A is rolled back, panic with error |
| Last fails | All previous rolled back, panic with error |

```electron
// Example: C requires inner { B }, but B wasn't attached due to error
player <- A <- B <- C;
// If B fails requirements:
// - A is detached (rollback)
// - Panic: "Cannot attach shell 'B' to core 'Player': missing required field 'velocity' of type 'vec3'"

// After panic, player has NO new shells (clean rollback)
```

This transactional approach prevents inconsistent state. Game code should fail loudly rather than leave objects partially configured.

### 8.5 Shell Stacking and Method Resolution

When a method is called, shells are searched top-to-bottom. The first shell with a matching method handles the call:

```electron
core Entity {
    public int hp;
    public vec3 position;
}

shell Enemy {
    def take_damage(int amount) {
        core.hp -= amount;
        if core.hp <= 0 {
            die();
        }
    }
}

shell Armored {
    def take_damage(int amount) {
        // Reduce damage by half, then delegate
        inner.take_damage(amount / 2);
    }
}

shell Invulnerable {
    def take_damage(int amount) {
        // Block all damage - no inner call
    }
}

Entity boss = (Entity) { hp: 1000, position: vec3(0, 0, 0) };
boss <- Enemy;
boss <- Armored;

boss.take_damage(100);  // Armored intercepts, Enemy receives 50
```

**Method Dispatch Performance:**

Shell method dispatch uses a cached dispatch table rebuilt when shells are pushed or popped. Method calls are O(1) after the initial table lookup:

```electron
// At compile time, each call site gets a cache slot
player.take_damage(50);

// Runtime behavior:
// - If shell stack unchanged: O(1) direct call via cached function pointer
// - If shell stack changed: rebuild cache, then O(1) call
```

### 8.6 Delegation with `inner`

Use `inner` to delegate to the next shell in the stack:

```electron
shell Slowed {
    def move(float dx, float dy) {
        // Move at half speed, then delegate
        inner.move(dx * 0.5, dy * 0.5);
    }
}

shell Frozen {
    def move(float dx, float dy) {
        // No inner call = movement completely blocked
    }
}
```

**Basic Inner Call Behavior:**

| Syntax | Handler Exists | No Handler |
|--------|----------------|------------|
| `inner.method()` | Returns handler's value | Returns default for return type |
| `inner!.method()` | Returns handler's value | Panic |
| `inner?.method()` | Returns handler's value | Returns default (explicit) |

**Return Value Semantics:**

When `inner.method()` has no handler below, the return value is the type's default:
- `int` → `0`
- `float` → `0.0`
- `bool` → `false`
- `string` → `""`
- Nullable types → `null`
- Collections → empty

```electron
shell Armored {
    def get_defense() -> int {
        return inner.get_defense() + 10;  // Returns 10 if no handler (0 + 10)
    }
}

shell BaseStats {
    def get_defense() -> int {
        return 5;
    }
}

// With both shells: get_defense() returns 15
// With only Armored: get_defense() returns 10
```

**Typed Inner Calls:**

Use bracket syntax to call a specific shell type in the stack:

```electron
inner[ShellType].method(args)     // First shell of type T below
inner[ShellType]!.method(args)    // Panic if T not found
inner[ShellType]?.method(args)    // Silent fallback (explicit)
```

Typed inner calls search downward from the current shell, looking for the first shell of the specified type. Shells between the current shell and the target are skipped:

```electron
// Stack (top to bottom): Armored -> Blessed -> Cursed -> BaseStats

shell Armored {
    def get_defense() -> int {
        // inner.get_defense() - calls Blessed.get_defense() (first below)
        // inner[BaseStats].get_defense() - skips Blessed and Cursed, calls BaseStats
        return inner[BaseStats].get_defense() + armor_bonus;
    }
}
```

**Typed Inner Call Behavior:**

| Syntax | Shell Found | Shell Not Found |
|--------|-------------|-----------------|
| `inner[T].method()` | Calls T's method | Returns default value |
| `inner[T]!.method()` | Calls T's method | Panic: "Shell 'T' not found below 'Current'" |
| `inner[T]?.method()` | Calls T's method | Returns default value (explicit) |

**Example: Direct Delegation to Specific Shell:**

```electron
shell DamageProcessor {
    requires { int hp }

    def take_damage(int amount) {
        // Apply damage through the normal chain
        inner.take_damage(amount);
    }

    def take_pure_damage(int amount) {
        // Skip all modifiers, go directly to HealthManager
        inner[HealthManager]!.apply_damage(amount);
    }
}

shell HealthManager {
    requires { int hp }

    def apply_damage(int amount) {
        core.hp -= amount;
    }

    def take_damage(int amount) {
        apply_damage(amount);
    }
}
```

**Multiple Shell Instances (`inner_all`, `outer_all`):**

When multiple instances of the same shell type exist in the stack, `inner[T]` returns only the first match (closest to current position). To access all instances, use `inner_all[T]`:

```electron
// Stack: DamageAmp -> Buff -> Buff -> Buff -> BaseStats
// (Three Buff instances stacked)

shell DamageAmp {
    def calculate_damage(int base) -> int {
        int damage = base;

        // inner[Buff].get_multiplier() - only first Buff (closest)
        // inner_all[Buff] - returns list of all Buff shells below

        for Buff b in inner_all[Buff] {
            damage = (int)(damage * b.get_multiplier());
        }

        return damage;
    }
}

shell Buff {
    float multiplier = 1.0;

    def get_multiplier() -> float {
        return multiplier;
    }
}
```

| Syntax | Returns | Use Case |
|--------|---------|----------|
| `inner[T]` | First T below (or default) | Single delegation |
| `inner[T]!` | First T below (or panic) | Required delegation |
| `inner_all[T]` | `list[T]` of all T below | Aggregation (sum all buffs, etc.) |
| `outer[T]` | First T above (or default) | Service lookup |
| `outer_all[T]` | `list[T]` of all T above | Multi-provider queries |

`inner_all[T]` and `outer_all[T]` return empty lists if no matching shells exist—they never panic. The returned list is ordered by stack position (closest first).

```electron
// Aggregate all buff multipliers
float total = inner_all[Buff].reduce(1.0, (float acc, Buff b) -> float {
    return acc * b.get_multiplier();
});
```

**Strict Shells:**

Mark a shell as `strict` to require that all `inner` calls have a handler:

```electron
strict shell DamageProcessor {
    requires { int hp }

    def take_damage(int amount) {
        // In a strict shell, inner calls become inner! implicitly
        inner.apply_damage(amount);  // Panics if no handler below
    }
}
```

**Note:** The `strict` modifier changes the semantics of `inner.method()` calls within the shell. When reading code, `inner.method()` in a strict shell behaves like `inner!.method()` (panic on no handler), while in a non-strict shell it returns the default value. Always check the shell declaration to understand the behavior. IDEs should highlight strict shells distinctly.

### 8.7 Upward Delegation with `outer`

The `outer` keyword enables upward traversal of the shell stack, allowing lower shells to access services or context provided by shells above them.

| Keyword | Direction | Use Case |
|---------|-----------|----------|
| `inner` | Downward (toward core) | Delegation, decoration, base implementation |
| `outer` | Upward (toward stack top) | Context access, coordination, service lookup |

**Syntax:**

```electron
outer.method(args)              // First shell above with this method
outer!.method(args)             // Panic if no handler above
outer?.method(args)             // Silent fallback (explicit)

outer[ShellType].method(args)   // First shell of type T above
outer[ShellType]!.method(args)  // Panic if T not found above
outer[ShellType]?.method(args)  // Silent fallback if T not found
```

**Outer Call Behavior:**

| Syntax | Shell Found | Shell Not Found |
|--------|-------------|-----------------|
| `outer.method()` | Calls first above with method | Returns default value |
| `outer!.method()` | Calls first above with method | Panic |
| `outer[T].method()` | Calls T's method | Returns default value |
| `outer[T]!.method()` | Calls T's method | Panic: "Shell 'T' not found above 'Current'" |

**Example: Accessing Context from Above:**

```electron
// Stack (top to bottom): CharacterController -> StatModifier -> Equipment -> BaseStats

shell CharacterController {
    requires { int hp, int max_hp }

    def get_max_hp() -> int {
        return core.max_hp;
    }

    def get_hp_percent() -> float {
        return (float)core.hp / (float)core.max_hp;
    }
}

shell Equipment {
    requires inner { CharacterController }

    int armor = 0;

    def get_effective_armor() -> int {
        // Access CharacterController above to check HP
        float hp_percent = outer[CharacterController].get_hp_percent();

        // Armor effectiveness scales with HP
        return (int)(armor * hp_percent);
    }

    def get_equipment_summary() -> string {
        // Access level from CharacterController
        int level = outer[CharacterController].get_level();
        return "Level {level} - Armor: {armor}";
    }
}
```

**Cycle Detection:**

Mutual recursion between `inner` and `outer` calls can create infinite loops. The runtime detects and prevents this using depth-based detection:

```electron
shell A {
    def foo() {
        outer[B].bar();  // Calls up to B
    }
}

shell B {
    def bar() {
        inner[A].foo();  // Calls down to A - infinite loop!
    }
}
```

| Build | Detection | Error Message |
|-------|-----------|---------------|
| Debug | Full call-chain + depth limit | `"Cycle detected: A.foo -> B.bar -> A.foo"` |
| Release | Depth limit only (64 calls) | `"Shell call depth exceeded (64): possible infinite recursion"` |

The depth-based detection has negligible overhead (one increment/decrement per call) and prevents infinite loops from freezing the game in release builds.

### 8.8 Shell Composition

Shells can include other shells (like mixins). Composed shells are **flattened into the composing shell** at compile time. The composed shell does NOT appear in the runtime shell stack.

```electron
shell Movable {
    def move(float dx, float dy) { core.position.x += dx; core.position.y += dy; }
    def update(float dt) { /* Movable update */ }
}

shell Damageable {
    def take_damage(int amount) { core.hp -= amount; }
    def update(float dt) { /* Damageable update */ }
}

shell Character: Movable, Damageable {
    requires { vec3 position, int hp }

    // Character now has: move(), take_damage()
    // update() conflicts - must be resolved explicitly

    def update(float dt) {
        Movable.update(dt);       // Call composed implementation directly
        Damageable.update(dt);    // Not inner calls - these are static
    }

    def special_move() {
        move(10, 0);             // Calls Character's copy of Movable.move
        inner.on_special_move(); // Delegates to NEXT SHELL IN STACK
    }
}

// Runtime stack for: player <- Enemy <- Character
// Stack (top to bottom): Character, Enemy
// Note: Movable, Damageable are NOT in stack - they're compiled into Character
```

**Conflict Resolution:**

When composed shells define the same method, use qualified calls:

```electron
shell Character: Movable, Damageable {
    requires { vec3 position, int hp }

    // Both Movable and Damageable define update()
    def update(float dt) {
        Movable.update(dt);       // Call Movable's implementation
        Damageable.update(dt);    // Call Damageable's implementation
    }
}
```

Unresolved conflicts are compile errors.

**Shell Composition Sugar:**

```electron
shell PlayerCharacter = Movable + Damageable + Controllable;
```

Equivalent to shell composition with no additional methods.

### 8.9 Core-Shell Relationship

Each shell instance attaches to exactly **one** core instance. A shell cannot span multiple cores:

```electron
// CORRECT: Shell attaches to one core
entity player = entity.spawn();
player.add((PlayerData) { hp: 100, position: vec3(0,0,0) });
player.add((Inventory) { slots: 10 });

PlayerData data = player![PlayerData];
data <- Controllable;  // Controllable's 'core' is this PlayerData instance
```

**Cross-Core Communication:**

If behavior needs data from multiple cores, use entity queries:

```electron
shell PlayerController {
    requires { vec3 position, int hp }  // Must be on a core with these

    def update(float dt) {
        // Access other cores via entity reference
        entity e = core.entity;  // Built-in: every core knows its entity
        Inventory? inv = e[Inventory];
        if inv != null {
            process_inventory(inv);
        }
    }
}
```

**Same Shell on Multiple Cores:**

The same shell TYPE can attach to different cores on one entity:

```electron
entity e = entity.spawn();
e.add((Weapon) { damage: 10, position: vec3(0,0,0) });
e.add((Shield) { armor: 5, position: vec3(0,0,0) });

Weapon w = e![Weapon];
Shield s = e![Shield];
w <- Movable;  // Movable instance 1
s <- Movable;  // Movable instance 2 (separate state)
```

### 8.10 Unique Shells

Mark a shell as `unique` to enforce only one instance per core:

```electron
unique shell PlayerController {
    def update(float dt) {
        // Handle input
    }
}

Player player = (Player) { hp: 100, position: vec3(0, 0, 0) };
player <- PlayerController;
player <- PlayerController;  // No-op: shell already attached
player <<- PlayerController; // Replaces existing with fresh instance
```

**Unique Shell Behavior Table:**

| Operation | Shell Present | Shell Absent |
|-----------|---------------|--------------|
| `<-` (push) | No-op (keeps existing) | Attaches new instance |
| `<<-` (force push) | Replaces with new instance | Attaches new instance |
| `->` (pop) | Removes the instance | No-op |
| `->>` (remove all) | Removes the instance | No-op |

### 8.11 Operator Overloading

Shells can define operators for their core type:

```electron
shell Vec3Ops {
    requires { float x, float y, float z }

    def __add__(vec3 other) -> vec3 {
        return vec3(
            core.x + other.x,
            core.y + other.y,
            core.z + other.z
        );
    }

    def __eq__(vec3 other) -> bool {
        return core.x == other.x &&
               core.y == other.y &&
               core.z == other.z;
    }
}
```

**Supported Operator Methods:**

| Method | Operator | Reverse |
|--------|----------|---------|
| `__add__` | `+` | `__radd__` |
| `__sub__` | `-` | `__rsub__` |
| `__mul__` | `*` | `__rmul__` |
| `__div__` | `/` | `__rdiv__` |
| `__mod__` | `%` | `__rmod__` |
| `__eq__` | `==` | - |
| `__neq__` | `!=` | - |
| `__lt__` | `<` | - |
| `__gt__` | `>` | - |
| `__lte__` | `<=` | - |
| `__gte__` | `>=` | - |
| `__neg__` | `-` (unary) | - |

**Operator Dispatch Rules:**

Operators use standard shell stack resolution (topmost shell wins). For binary operators:

1. Try LHS's `__op__` method (e.g., `a.__add__(b)`)
2. If LHS has no handler, try RHS's `__rop__` method (e.g., `b.__radd__(a)`)
3. If neither exists, use built-in behavior or error

```electron
shell ScaledVector {
    requires { float x, float y, float z }

    // LHS handler: vec * scalar
    def __mul__(float scalar) -> vec3 {
        return vec3(core.x * scalar, core.y * scalar, core.z * scalar);
    }

    // RHS handler: scalar * vec (reverse)
    def __rmul__(float scalar) -> vec3 {
        return vec3(core.x * scalar, core.y * scalar, core.z * scalar);
    }
}

// Both work:
vec3 a = my_vec * 2.0;   // Calls __mul__
vec3 b = 2.0 * my_vec;   // Calls __rmul__
```

**Inner Delegation:**

Operator methods can delegate via `inner` like regular methods:

```electron
shell DebugOps {
    def __add__(vec3 other) -> vec3 {
        print("Adding vectors");
        return inner.__add__(other);  // Delegate to shell below
    }
}
```

**Default Behavior:**

Cores without `__eq__`/`__neq__` shells fall back to reference equality.

### 8.12 Shell Stack Restrictions

Shells can declare restrictions on what can be stacked with them, enabling controlled shell architectures.

#### 8.12.1 Sealed Shells (Allowlist)

A `sealed` shell defines an allowlist of shells that can be pushed on top of it:

```electron
sealed shell ActorBase allows [Movement, Combat, Inventory, StatusEffect] {
    requires { int id, vec3 position }

    def get_id() -> int { return core.id; }
    def get_position() -> vec3 { return core.position; }
}
```

When a shell is pushed onto a core:
1. Search downward for any `sealed` shells
2. For each sealed shell found, verify the new shell is in its `allows` list
3. If any sealed shell rejects the new shell, the push fails with a panic

```electron
core Player {
    public int id;
    public vec3 position;
    public int hp;
}

Player p = (Player) { id: 1, position: vec3(0,0,0), hp: 100 };

p <- ActorBase;      // OK: foundation shell
p <- Movement;       // OK: in allows list
p <- Combat;         // OK: in allows list
p <- RandomShell;    // Panic: "Cannot push 'RandomShell': not in ActorBase.allows"
```

**Multiple Sealed Shells:**

If multiple sealed shells exist in a stack, the new shell must be allowed by ALL of them:

```electron
sealed shell ActorBase allows [Movement, Combat, AI] { }
sealed shell PlayerBase allows [Combat, Inventory, Input] { }

// Stack: PlayerBase -> ActorBase -> (core)
core <- ActorBase;
core <- PlayerBase;
core <- Combat;      // OK: allowed by both
core <- Movement;    // Panic: not in PlayerBase.allows
core <- Input;       // Panic: not in ActorBase.allows
```

#### 8.12.2 Inner Requirements (Dependency Declaration)

Shells can declare that they require specific shell types to exist below them using `requires inner`:

```electron
shell StatModifier {
    requires inner { CharacterController }  // CharacterController must be below

    float damage_multiplier = 1.0;

    def get_modified_damage(int base) -> int {
        return (int)(base * damage_multiplier);
    }
}

shell Equipment {
    requires inner { CharacterController, StatModifier }  // Both must be below

    int weapon_damage = 10;

    def get_total_damage() -> int {
        int base = weapon_damage;
        // Safe to call - StatModifier is guaranteed to exist
        return inner[StatModifier]!.get_modified_damage(base);
    }
}
```

When a shell with `requires inner` is pushed:
1. Verify each required shell type exists somewhere below in the stack
2. If any required shell is missing, the push fails with a panic

```electron
// Valid stacking order:
core <- CharacterController;  // Foundation
core <- StatModifier;         // OK: CharacterController is below
core <- Equipment;            // OK: both CharacterController and StatModifier below

// Invalid:
core <- Equipment;            // Panic: requires CharacterController, StatModifier below
core <- CharacterController;
core <- Equipment;            // Panic: requires StatModifier below
```

#### 8.12.3 Combining Restrictions

`sealed` and `requires inner` work together to create robust shell architectures:

```electron
// Foundation layer - controls what can stack on top
sealed shell GameObjectBase allows [Transform, Physics, Renderer, Script] {
    requires { int id }

    def get_id() -> int { return core.id; }
}

// Transform requires GameObjectBase below
shell Transform {
    requires { vec3 position, vec3 rotation, vec3 scale }
    requires inner { GameObjectBase }

    def get_world_position() -> vec3 {
        return core.position;
    }
}

// Physics requires both GameObjectBase and Transform
shell Physics {
    requires { vec3 velocity }
    requires inner { GameObjectBase, Transform }

    def update(float dt) {
        vec3 pos = inner[Transform]!.get_world_position();
        // ... physics update
    }
}

// Renderer has its own allowlist for render features
sealed shell Renderer allows [Material, Lighting, Shadow, PostProcess] {
    requires inner { GameObjectBase, Transform }

    def render() {
        vec3 pos = inner[Transform]!.get_world_position();
        // ... render at position
    }
}

// Material can only be on a Renderer (due to sealed) and requires Renderer below
shell Material {
    requires inner { Renderer }

    string shader = "default";
}
```

#### 8.12.4 Restriction Inheritance in Composition

When shells are composed, restrictions are merged:

```electron
sealed shell A allows [X, Y] { }
sealed shell B allows [Y, Z] { }

// Composed shell inherits INTERSECTION of allows
shell C: A, B { }
// C effectively allows [Y] (only Y is in both lists)

// requires inner merges with UNION
shell D {
    requires inner { Base1 }
}
shell E {
    requires inner { Base2 }
}
shell F: D, E { }
// F requires inner { Base1, Base2 }
```

**Compile-Time Assistance:**

The compiler provides best-effort static analysis when shell stacks are known:

| Situation | Compiler Behavior |
|-----------|-------------------|
| `inner[T]!` where T statically unreachable | Error |
| `inner[T]` where T statically unreachable | Warning |
| `outer[T]!` where T statically unreachable | Error |
| `outer[T]` where T statically unreachable | Warning |
| Push violates `sealed` (statically known) | Error |
| Push violates `requires inner` (statically known) | Error |

---

## 9. Enums

Enums define a type with a fixed set of variants. Enums must have at least one variant; empty enums are a compile error:

```electron
enum Empty { }  // Compile error: enum must have at least one variant
```

### 9.1 Simple Enums

```electron
enum Direction {
    North,
    South,
    East,
    West
}

Direction facing = Direction.North;

match facing {
    Direction.North => move(0, 1),
    Direction.South => move(0, -1),
    Direction.East => move(1, 0),
    Direction.West => move(-1, 0)
}
```

### 9.2 Enums with Data

Variants can carry associated data:

```electron
enum State {
    Idle,
    Walking,
    Attacking(int damage),
    Dead
}

State current = State.Attacking(25);

match current {
    State.Idle => wait(),
    State.Walking => move(),
    State.Attacking(dmg) => deal_damage(dmg),
    State.Dead => respawn()
}
```

### 9.3 Complex Enum Data

```electron
enum Event {
    KeyPress(int keycode),
    MouseMove(float x, float y),
    Collision(Entity other, vec3 point, vec3 normal),
    Custom(string name, dict[string -> string] data)
}
```

### 9.4 Enums as Dictionary Keys

Simple enums (without data) can be used as dictionary keys:

```electron
dict[Direction -> string] names = {
    Direction.North: "Up",
    Direction.South: "Down",
    Direction.East: "Right",
    Direction.West: "Left"
};
```

Enums with data cannot be dictionary keys.

---

## 10. Error Handling

Electron uses explicit error handling with `result` types instead of exceptions.

### 10.1 Result Type

`result` is a built-in generic enum provided by the compiler:

```electron
// This is a compiler built-in, not user-definable
enum result[T, E] {
    success(T),
    error(E)
}
```

The compiler provides special support for `result`:
- **Type inference:** `success(5)` infers `result[int, ?]` where `?` is determined by context
- **Error propagation:** The `?` operator works with `result` types
- **Pattern matching:** Full destructuring support in `match` expressions

```electron
def divide(int a, int b) -> result[int, string] {
    if b == 0 {
        return error("Division by zero");
    }
    return success(a / b);
}

result[int, string] r1 = success(42);
result[int, string] r2 = error("failed");
```

**Error Type Inference Rules:**

```electron
// 1. Return type provides context:
def foo() -> result[int, string] {
    return success(5);  // Infers result[int, string]
}

// 2. Variable type provides context:
result[int, MyError] r = success(5);  // OK

// 3. No context = compile error:
func f = () {
    return success(5);  // Error: cannot infer error type
};

// Fix by specifying return type:
func f = () -> result[int, string] {
    return success(5);  // OK
};
```

### 10.2 Error Propagation

Use `?` to propagate errors automatically:

```electron
def load_config(string path) -> result[Config, Error] {
    File f = open(path)?;           // Returns early if open fails
    string data = read_all(f)?;     // Returns early if read fails
    Config cfg = parse(data)?;      // Returns early if parse fails
    return success(cfg);
}
```

The `?` operator desugars to:

```electron
// expr?  becomes:
match expr {
    success(v) => v,
    error(e) => return error(e)
}
```

**Error Type Compatibility:**

The error types must match. If they don't, explicit conversion is required:

```electron
def process() -> result[Data, AppError] {
    // If open returns result[File, IoError], convert:
    File f = open(path).map_err((IoError e) -> AppError {
        return (AppError) { message: e.message, code: 1 };
    })?;
    return success(process_file(f));
}
```

### 10.3 Handling Results

```electron
result[int, string] res = divide(10, 2);

match res {
    success(value) => print("Result: {value}"),
    error(msg) => print("Error: {msg}")
}

// Or with if-is pattern
if res is success(value) {
    use(value);
}
```

### 10.4 Standard Error Type

The standard library provides an `Error` core:

```electron
core Error {
    public string message;
    public string? file;
    public int? line;
}
```

Custom error types can be created as cores:

```electron
core FileError {
    public string path;
    public string message;
    public int code;
}

def open_file(string path) -> result[File, FileError] {
    // ...
}
```

### 10.5 Runtime Errors and Panics

| Situation | Behavior |
|-----------|----------|
| Null dereference | Panic with stack trace |
| Array out of bounds | Panic with stack trace |
| Division by zero (int) | Panic |
| Division by zero (float) | IEEE 754: `1.0/0.0` → `inf`, `0.0/0.0` → `nan`, `-1.0/0.0` → `-inf` |
| Shell requirements mismatch | Panic: "Cannot attach shell 'X' to core 'Y': missing required field 'Z' of type 'T'" |
| `inner!` with no handler | Panic |
| `entity![T]` when core missing | Panic |
| Stale entity reference | Panic with generation mismatch message |

**Panic Semantics:**

A panic is an unrecoverable abort:
- Execution terminates immediately
- Stack trace is printed to stderr
- `defer` statements do **not** run
- Cannot be caught or recovered from

For recoverable errors, use `Result` types and the `?` operator.

---

## 11. Coroutines

Coroutines enable writing asynchronous game logic in a sequential style.

### 11.1 The `coro` Type

`coro` is a built-in generic type provided by the compiler:

```electron
// coro[T] represents a coroutine that yields T when complete
// coro is shorthand for coro[void] (no return value)
```

### 11.2 Yield Requests

Coroutines communicate with the scheduler via `yield_request`, a built-in enum:

```electron
// Built-in yield types (compiler intrinsics)
enum yield_request {
    next,                             // Resume next frame
    wait(float seconds),              // Resume after duration
    until(func[() -> bool] cond),     // Resume when condition true
    while_true(func[() -> bool] cond),// Resume when condition false
    join(coro other),                 // Resume when other completes
}
```

### 11.3 Declaring Coroutines

```electron
def enemy_patrol() -> coro {
    loop {
        move_to(point_a);
        yield yield_request.wait(2.0);  // Explicit
        // Or with sugar:
        yield wait(2.0);                // wait() returns yield_request.wait(2.0)
        move_to(point_b);
        yield;                          // Sugar for yield yield_request.next
    }
}

def fade_in(float duration) -> coro {
    float t = 0.0;
    while t < duration {
        set_alpha(t / duration);
        yield;  // Wait one frame
        t += delta_time();
    }
    set_alpha(1.0);
}
```

### 11.4 Spawning Coroutines

The `spawn` keyword creates a new coroutine instance and registers it with the engine scheduler:

```electron
coro patrol = spawn enemy_patrol();
coro fade = spawn fade_in(1.5);

// spawn is a keyword, not a function
coro c = spawn compute_path(start, end);
```

### 11.5 Yield Expressions

| Expression | Behavior |
|------------|----------|
| `yield;` | Pause until next frame |
| `yield wait(seconds);` | Pause for duration |
| `yield until(condition);` | Pause until condition is true |
| `yield while_true(condition);` | Pause while condition is true |

### 11.6 Coroutine Control

```electron
coro c = spawn some_behavior();

// Check if running
if c.running() {
    print("Still going");
}

// Check if cancelled
if c.cancelled() {
    print("Was cancelled");
}

// Cancel
c.cancel();

// Wait for completion (blocks current coroutine)
yield c;
```

**Cancellation Cleanup with `on_cancel`:**

Use `on_cancel` blocks to register cleanup code that runs when a coroutine is cancelled:

```electron
def critical_op() -> coro {
    Resource r = acquire_resource();

    // Register cancellation handler
    on_cancel {
        release_resource(r);
    };

    yield wait(5.0);

    // Normal cleanup (on_cancel does NOT run on normal completion)
    release_resource(r);
}
```

| Aspect | Behavior |
|--------|----------|
| **Trigger** | Runs when coroutine is cancelled via `.cancel()` or hot reload |
| **Hot reload** | `on_cancel` blocks DO run when cancelled due to hot reload |
| **Order** | Multiple `on_cancel` blocks execute in LIFO order (like `defer`) |
| **Restrictions** | Cannot contain `yield`, `return`, `break`, `continue`, or nested `on_cancel` |
| **Normal completion** | Does NOT run if coroutine completes normally |

```electron
def example() -> coro {
    on_cancel { print("first registered, runs last"); };
    on_cancel { print("second registered, runs first"); };
    yield wait(1.0);
}
// If cancelled: prints "second registered, runs first" then "first registered, runs last"
```

### 11.7 Coroutine Return Values

```electron
def compute_path(vec3 start, vec3 end) -> coro[list[vec3]] {
    // Expensive computation spread over frames
    list[vec3] path = [];
    // ...
    yield;
    // ...
    return path;
}

// Usage
coro[list[vec3]] path_coro = spawn compute_path(a, b);
list[vec3] path = yield path_coro;  // Wait and get result
```

**Coroutine States:**

| State | `running()` | `cancelled()` | `yield coro` behavior |
|-------|-------------|---------------|----------------------|
| Running | `true` | `false` | Suspends until completion, returns value |
| Completed | `false` | `false` | Returns cached result immediately |
| Cancelled | `false` | `true` | Returns default for T, or `error(CancellationError)` for `result[T,E]` |

**Detailed Semantics:**

```electron
// Yielding on completed coroutine returns cached result
coro[int] c = spawn compute();
int r1 = yield c;  // Waits, gets result (e.g., 42)
int r2 = yield c;  // Returns cached 42 immediately (no wait)

// Yielding on cancelled coroutine (simple types)
coro[int] c = spawn compute();
c.cancel();
int r = yield c;   // Returns 0 (default for int)

// Yielding on cancelled coroutine (result types)
coro[result[int, Error]] c = spawn may_fail();
c.cancel();
result[int, Error] r = yield c;  // Returns error(CancellationError)

match r {
    success(val) => use(val),
    error(e) if e is CancellationError => handle_cancelled(),
    error(e) => handle_other_error(e)
}

// Result access without yielding
coro[T].result() -> T?    // Returns null if not completed, value if completed
coro[T].result_or(T) -> T // Returns default if not completed
```

**CancellationError:**

`CancellationError` is a built-in error type for cancelled coroutines:

```electron
core CancellationError {
    public string message = "Coroutine was cancelled";
}
```

This allows cancelled coroutines with `result` return types to be handled gracefully rather than panicking or returning misleading success values.

### 11.8 Engine Scheduler Contract

The engine manages coroutine execution:

1. `spawn` registers coroutine with engine scheduler
2. Engine calls `resume()` each frame on eligible coroutines
3. `resume()` returns `yield_request` telling engine when to call again
4. Engine tracks timers, evaluates conditions, manages dependencies

---

## 12. Modules and Imports

### 12.1 Module Structure

Each `.e` file is implicitly a module. The file name (without extension) is the module name.

```
project/
├── main.e
├── player.e
├── enemies/
│   ├── goblin.e
│   └── dragon.e
└── utils/
    ├── math.e
    └── helpers.e
```

### 12.2 Import Syntax

```electron
// Import entire module
import "enemies/goblin"

// Import with alias
import "utils/math" as m

// Import specific symbols
from "utils/helpers" import clamp, lerp, normalize

// Import multiple modules
import "player"
import "enemies/dragon"
```

File extensions are omitted in import paths.

**Import Conflicts:**

Conflicts are compile-time errors requiring explicit resolution:

```electron
// utils/math.e
export def clamp(float x, float min, float max) -> float { ... }

// utils/helpers.e
export def clamp(int x, int min, int max) -> int { ... }

// main.e
from "utils/math" import clamp      // OK
from "utils/helpers" import clamp   // Error: 'clamp' already imported

// Resolution options:
import "utils/math" as math
import "utils/helpers" as helpers
math.clamp(1.0, 0.0, 2.0);
helpers.clamp(1, 0, 2);
```

### 12.3 Exporting

By default, top-level declarations are private to the module. Use `export` to export:

```electron
// utils/math.e

export const PI = 3.14159;

export def clamp(float x, float min, float max) -> float {
    if x < min { return min; }
    if x > max { return max; }
    return x;
}

// Private helper, not exported
def internal_helper() {
    // ...
}

export core Vector2 {
    public float x;  // public for field visibility
    public float y;
}
```

### 12.4 Circular Imports

Circular imports are allowed. The compiler resolves dependencies through two-pass analysis.

---

## 13. Engine Integration

### 13.1 Lifecycle Decorators

Decorators register functions for engine callbacks:

```electron
@on_update
def update(float dt) {
    // Called every frame
}

@on_fixed_update
def fixed_update(float dt) {
    // Called at fixed timestep (physics)
}

@on_collision
def handle_collision(Entity other, vec3 point, vec3 normal) {
    // Called on physics collision
}

@on_trigger
def handle_trigger(Entity other, bool entered) {
    // Called on trigger enter/exit
}

@on_destroy
def cleanup() {
    // Called when entity is destroyed
}

@on_enable
def on_enabled() {
    // Called when entity/shell is enabled
}

@on_disable
def on_disabled() {
    // Called when entity/shell is disabled
}
```

**Multiple Handlers:**

Multiple functions in the same module can have the same lifecycle decorator. They execute in declaration order:

```electron
@on_update
def update_physics(float dt) { }

@on_update
def update_animation(float dt) { }

@on_update
def update_ai(float dt) { }
// Execution order: update_physics, update_animation, update_ai
```

Across modules, execution order follows import order. For deterministic behavior, consider using a single entry point that calls other functions in explicit order.

### 13.2 Property Change Observers

React to field changes. The field name in `@change` refers to fields declared in the shell's `requires` block:

```electron
core Player {
    public int hp;
    public int max_hp;
}

shell HealthUI {
    requires { int hp, int max_hp }

    int ui_hp;  // Shell's own field - NOT watchable via @change

    @change(hp)  // Watches core.hp (from requires)
    def on_hp_changed(int old_value, int new_value) {
        update_health_bar(new_value, core.max_hp);

        if new_value < old_value {
            flash_damage_indicator();
        }
    }
}
```

**Observer Timing and Semantics:**

| Aspect | Behavior |
|--------|----------|
| **Timing** | End of frame, after all `@on_update` handlers |
| **Batching** | Multiple writes within a frame trigger ONE observer call |
| **Watched field values** | `old_value` is frame-start, `new_value` is current |
| **Non-watched field values** | Observers see CURRENT values for all non-watched fields |
| **Nested changes** | If observer modifies watched field, change is deferred to next frame |
| **Cascading** | If observer A modifies field watched by observer B, B fires in frame N+1 (dirty bits prevent A→B→A loops) |
| **Order** | Observers called in shell stack order (top-down) |

**Rules:**
- `@change(field)` watches `requires` field, error if ambiguous or missing
- Shell's own fields are NOT watchable (no self-observation)
- Nested paths are not supported (`@change(transform.position)` is a compile error)

### 13.3 The `entity` Type

`entity` is an opaque engine handle with a **generation counter** for safe reference tracking.

**Entity Lifecycle:**

```electron
// Spawn a new entity
entity e = entity.spawn();

// Destroy an entity
entity.destroy(e);

// Check if alive (validates generation)
if entity.alive(e) { }

// Get entity ID
int id = e.id;
```

**Generation Counter:**

Each entity ID includes a generation number. When an entity is destroyed, the generation increments. Stale references are detected at runtime:

```electron
entity e = entity.spawn();      // id: 42, gen: 1
PlayerData data = e![PlayerData];
entity.destroy(e);              // gen: 1 → 2

// Accessing stale reference produces clean panic:
data.hp;  // Panic: "Accessing core from destroyed entity (id: 42, expected gen: 1, current: 2)"

// entity.alive() returns false for stale references
entity.alive(e);  // false
```

**Core Management:**

Cores are data components attached to entities:

```electron
core PlayerData {
    public int hp;
    public vec3 position;
    public int score;
}

core Transform {
    public vec3 position;
    public vec3 rotation;
    public vec3 scale;
}

// Attach cores to entity
entity player = entity.spawn();
player.add((PlayerData) { hp: 100, position: vec3(0, 0, 0), score: 0 });
player.add((Transform) { position: vec3(0, 0, 0), rotation: vec3(0, 0, 0), scale: vec3(1, 1, 1) });

// Retrieve cores (nullable if missing)
PlayerData? data = player[PlayerData];

// Retrieve cores (panic if missing)
Transform t = player![Transform];

// Check for core (in boolean context, resolves to true/false)
if player[PlayerData] { }

// Remove core and return it
PlayerData removed = player[PlayerData].detach();

// Panic if core doesn't exist before detaching
PlayerData removed2 = player[PlayerData]!.detach();
```

**Best Practices:**

1. Don't store core references long-term; re-query from entity:
   ```electron
   def update(entity player) {
       if entity.alive(player) {
           PlayerData data = player![PlayerData];
           // Use data within this frame only
       }
   }
   ```

2. Use entity handles, not core references, for long-term storage:
   ```electron
   list[entity] enemies;  // Store entity handles
   // NOT: list[EnemyData] enemies;
   ```

3. Check `entity.alive()` before accessing stored entities

**Shell Attachment:**

Shells attach to cores, not entities:

```electron
PlayerData data = player![PlayerData];
data <- Controllable;
data <- Damageable;

// Query and modify shell stack
if data >- Frozen {
    data -> Frozen;
}
```

**Entity Queries:**

```electron
// Find all entities with specific cores
list[entity] enemies = scene.query[Enemy, Transform]();

// Iterate with core access
for entity e in enemies {
    Enemy enemy = e![Enemy];
    Transform t = e![Transform];
    // ...
}
```

**Query Snapshots:**

Entity queries return a snapshot of matching entities at query time. The returned list is not automatically updated when entities are created, destroyed, or have components added/removed:

```electron
list[entity] enemies = scene.query[Enemy, Transform]();

// This is a SNAPSHOT at query time
entity.destroy(enemies[0]);

// enemies list still contains the destroyed entity handle
enemies[0]![Enemy];  // Panic: "Accessing core from destroyed entity..."

// Safe pattern: validate before use
for entity e in enemies {
    if entity.alive(e) {
        Enemy data = e![Enemy];
        // Use data within this frame
    }
}

// Or: re-query when freshness matters
enemies = scene.query[Enemy, Transform]();  // Fresh snapshot
```

### 13.4 Hot Reloading

Electron is designed for hot reloading:

- Shells are looked up by name at runtime
- Core data is preserved across reloads
- Function references use indirect registry lookup

**When scripts are reloaded:**

1. **Core data is preserved:** Field additions get default values; removed fields are dropped
2. **Shell definitions are replaced atomically**
3. **Running coroutines:**
   - If structure unchanged (same yield points): continue at current position
   - If structure changed: restart from beginning with warning logged
4. **Registered callbacks are re-bound by name**
5. **`@change` observers:** Pending notifications are cleared. Post-reload, all fields are compared to defaults; observers fire for mismatches to resync UI/derived state

**Coroutine Hot Reload Control:**

```electron
@hot_reload_restart          // Restart from beginning (default)
def long_animation() -> coro { }

@hot_reload_preserve         // Attempt to preserve state (may fail)
def critical_sequence() -> coro { }

@hot_reload_cancel           // Cancel and don't restart
def optional_effect() -> coro { }
```

**Note:** When a coroutine is cancelled due to hot reload (via `@hot_reload_cancel` or structure change with `@hot_reload_restart`), any registered `on_cancel` blocks DO run. Use `on_cancel` to clean up resources:

```electron
@hot_reload_cancel
def holds_resource() -> coro {
    Resource r = acquire();
    on_cancel { release(r); };  // Runs on hot reload cancel
    yield wait(5.0);
}
```

**Migration Logic:**

The engine provides `@on_hot_reload` for migration logic:

```electron
@on_hot_reload
def migrate(int old_version, int new_version) {
    if old_version < 2 {
        // Migrate data from v1 to v2
    }
}
```

### 13.5 Debug and Conditional Compilation

**Debug Decorator:**

The `@debug` decorator marks code that is stripped in release builds:

```electron
// Debug function: becomes no-op in release
@debug
def log_frame(float dt) {
    print("Frame time: {dt}ms");
}

def update(float dt) {
    log_frame(dt);  // Call removed entirely in release

    // Debug statement: removed in release
    @debug print("Update called");

    // Actual game logic
    process(dt);
}
```

**Compile-Time Conditionals:**

Use `@comptime` constants with `if @comptime` for conditional compilation:

```electron
@comptime const ENABLE_PROFILING = false;

def expensive_operation() {
    if @comptime ENABLE_PROFILING {
        profiler.start("expensive_operation");
    }

    // ... actual work ...

    if @comptime ENABLE_PROFILING {
        profiler.end("expensive_operation");
    }
}
```

When the condition is `false`, the entire block is removed from compiled output.

---

## 14. Standard Library

### 14.1 Built-in Functions

Available without imports:

```electron
// Output
print(value)              // Print to console
assert(condition, msg?)   // Runtime assertion (panic on failure)

// Control flow
unreachable()             // Marks unreachable code (returns never)
unreachable(msg)          // With custom message

// Math
sin(x), cos(x), tan(x)
asin(x), acos(x), atan(x), atan2(y, x)
sqrt(x), pow(base, exp)
abs(x), floor(x), ceil(x), round(x)
min(a, b), max(a, b)
clamp(x, min, max)
lerp(a, b, t)

// Type queries
typeof(value) -> string
```

**The `unreachable()` Function:**

Marks code that should never execute. Returns the `never` type (see Section 2.13):

```electron
def process(State s) -> int {
    return match s {
        State.A => 1,
        State.B => 2,
        State.Removed => unreachable("State.Removed should never occur")
    };
}

// never coerces to any type
int x = if condition then 42 else unreachable();
```

| Build | Behavior |
|-------|----------|
| Debug | Panics with message: `"unreachable code reached"` or custom message |
| Release | Undefined behavior (optimizer may assume never reached) |

### 14.2 Engine Modules

```electron
import "engine/entity"
import "engine/input"
import "engine/time"
import "engine/scene"
import "engine/physics"
import "engine/audio"
```

**Time:**
```electron
float dt = time.delta();
float total = time.elapsed();
float scale = time.scale();
time.set_scale(0.5);  // Slow motion
```

**Input:**
```electron
if input.pressed("jump") { }
if input.held("forward") { }
if input.released("fire") { }

vec2 mouse = input.mouse_position();
vec2 mouse_delta = input.mouse_delta();
float axis = input.axis("horizontal");
```

**Scene:**
```electron
scene.load("levels/level1");
scene.add_entity(e);
scene.remove_entity(e);
list[entity] enemies = scene.query[Enemy, Transform]();
```

### 14.3 Collection Methods

**List:**
```electron
list[int] nums = [1, 2, 3];

nums.push(4);              // Add to end
int last = nums.pop();     // Remove and return last
nums.insert(0, 10);        // Insert at index
nums.remove(1);            // Remove at index
nums.swap_remove(1);       // O(1) remove, swaps with last element
nums.clear();              // Remove all

int len = nums.length;
bool empty = nums.is_empty();
bool has = nums.contains(2);
int idx = nums.index_of(2);  // -1 if not found

// Functional operations
list[int] doubled = nums.map((int x) -> int { return x * 2; });
list[int] evens = nums.filter((int x) -> bool { return x % 2 == 0; });
int sum = nums.reduce(0, (int acc, int x) -> int { return acc + x; });
list[int] rev = nums.reversed();  // Returns reversed copy
```

**Iteration Safety:**

Modifying a list during `for...in` iteration produces undefined behavior:

```electron
// UNDEFINED BEHAVIOR - do not do this:
for int i, Enemy e in enemies {
    enemies.remove(i);  // UB: invalidates iterator
}

// Safe patterns:
// 1. Filter to new list
enemies = enemies.filter((Enemy e) -> bool { return e.hp > 0; });

// 2. Backwards iteration with swap_remove
int i = enemies.length - 1;
while i >= 0 {
    if enemies[i].hp <= 0 {
        enemies.swap_remove(i);
    }
    i -= 1;
}
```

**Dict:**
```electron
dict[string -> int] ages = {"alice": 30};

ages["bob"] = 25;                    // Set
int? age = ages.get("alice");        // Get (nullable)
int age2 = ages.get_or("eve", 0);    // Get with default
bool exists = ages.has("bob");
ages.remove("alice");
ages.clear();

int count = ages.length;
list[string] keys = ages.keys();
list[int] values = ages.values();

for key, value in ages {
    print("{key}: {value}");
}
```

**String:**
```electron
string s = "Hello, World!";

int len = s.length;
string upper = s.upper();
string lower = s.lower();
string trimmed = s.trim();
list[string] parts = s.split(", ");
string joined = parts.join("-");
bool starts = s.starts_with("Hello");
bool ends = s.ends_with("!");
bool has = s.contains("World");
string sub = s.substring(0, 5);  // "Hello"
int idx = s.index_of("World");   // 7
string replaced = s.replace("World", "Electron");
string str_val = value.to_string();  // Convert any value to string
string interned = string.intern(s);  // Explicitly intern a runtime string
```

**Range:**
```electron
range r = 0..10;

int start = r.start;
int end = r.end;
bool inc = r.inclusive;
bool has = r.contains(5);
list[int] items = r.to_list();
```

**Result:**
```electron
result[int, string] res = success(42);

// Check variant
bool ok = res.is_success();
bool err = res.is_error();

// Extract values (panic if wrong variant)
int val = res.unwrap();           // Panics if error
string e = res.unwrap_err();      // Panics if success

// Extract with defaults
int val = res.unwrap_or(0);       // Returns 0 if error
int val = res.unwrap_or_else((string e) -> int { return -1; });

// Transform success value
result[string, string] mapped = res.map((int x) -> string { return x.to_string(); });

// Transform error value
result[int, Error] converted = res.map_err((string e) -> Error {
    return (Error) { message: e };
});

// Chain operations
result[int, string] chained = res.and_then((int x) -> result[int, string] {
    return if x > 0 then success(x * 2) else error("negative");
});
```

---

## 15. Memory Model

### 15.1 Garbage Collection

Electron uses tracing garbage collection for reference types (cores, strings, collections). The GC runs incrementally during frame boundaries.

### 15.2 Value Types

Primitives, vectors, matrices, ranges, tuples, and structs are stack-allocated when local and copied on assignment. No GC involvement.

### 15.3 Reference Types

Cores, strings, lists, dicts, and functions are heap-allocated and managed by GC. References are counted for cycle detection.

### 15.4 Engine Interop

Memory shared with the engine (entities, components) follows engine ownership rules. Electron references to engine objects are weak references validated via generation counters.

---

## 16. Concurrency Model

### 16.1 Single-Threaded Execution

Electron scripts execute on a single thread (the main/game thread). There are no threads, locks, or atomics in the language.

### 16.2 Engine Parallelism

The engine may parallelize systems that don't share state. From Electron's perspective, each script invocation is single-threaded.

### 16.3 Async Operations

Use coroutines for async-style programming. Coroutines are cooperative (not preemptive) and run on the main thread.

### 16.4 Future Consideration

Job system integration is planned for a future release:
```electron
@parallel
def process_batch(list[Entity] batch) { }
```

---

## Appendix A: Grammar Summary

```
program         → declaration* EOF
declaration     → constDecl | coreDecl | structDecl | shellDecl | enumDecl
                | funcDecl | importDecl | typeAlias

constDecl       → decorator* "const" IDENTIFIER "=" expression ";"
coreDecl        → "core" IDENTIFIER "{" fieldDecl* "}"
structDecl      → "struct" IDENTIFIER "{" fieldDecl* "}"
shellDecl       → shellModifiers "shell" IDENTIFIER compositionClause? allowsClause?
                  "{" shellRequires? (fieldDecl | funcDecl)* "}"
shellModifiers  → "strict"? "unique"? "sealed"?
compositionClause → ":" IDENTIFIER ("," IDENTIFIER)*
allowsClause    → "allows" "[" IDENTIFIER ("," IDENTIFIER)* "]"
shellRequires   → fieldRequires? entityRequires? innerRequires?
fieldRequires   → "requires" "{" typeField ("," typeField)* "}"
entityRequires  → "requires" "entity" "{" IDENTIFIER ("," IDENTIFIER)* "}"
innerRequires   → "requires" "inner" "{" IDENTIFIER ("," IDENTIFIER)* "}"
enumDecl        → "enum" IDENTIFIER "{" enumVariant ("," enumVariant)* "}"
funcDecl        → decorator* "def" IDENTIFIER "(" paramList? ")" ("->" type)? block
importDecl      → "import" STRING ("as" IDENTIFIER)?
                | "from" STRING "import" IDENTIFIER ("," IDENTIFIER)*
typeAlias       → "type" IDENTIFIER "=" type ";"

fieldDecl       → "public"? "readonly"? type IDENTIFIER ("=" expression)? ";"
paramList       → param ("," param)*
param           → "mut"? "*"? type IDENTIFIER ("=" expression)?

type            → primitiveType | collectionType | funcType | builtinType
                | tupleType | nullableType | IDENTIFIER | "never"
primitiveType   → "int" | "float" | "bool" | "string"
                | "vec2" | "vec3" | "vec4" | "mat2" | "mat3" | "mat4" | "range"
collectionType  → "list" "[" type "]" | "dict" "[" type "->" type "]"
funcType        → "func" "[" "(" typeList? ")" "->" type "]" "local"?
builtinType     → "coro" ("[" type "]")? | "entity"
                | "result" "[" type "," type "]"
tupleType       → "tuple" "[" type ("," type)+ "]"
nullableType    → type "?"

statement       → exprStmt | varDecl | assignment | ifStmt | whileStmt
                | doWhileStmt | loopStmt | forStmt | matchStmt
                | returnStmt | yieldStmt | breakStmt | continueStmt
                | deferStmt | onCancelStmt | labeledStmt | block

onCancelStmt    → "on_cancel" block

varDecl         → type IDENTIFIER ("=" expression)? ";"
assignment      → assignTarget assignOp expression ";"
assignTarget    → IDENTIFIER | postfix
assignOp        → "=" | "+=" | "-=" | "*=" | "/=" | "%="
                | "&=" | "|=" | "^=" | "<<=" | ">>="

returnStmt      → "return" expression? ";"
yieldStmt       → "yield" yieldExpr? ";"
yieldExpr       → expression | "wait" "(" expression ")"
                | "until" "(" expression ")" | "while_true" "(" expression ")"

matchStmt       → "match" expression "{" matchArm ("," matchArm)* "}"
matchArm        → pattern ("if" expression)? "=>" (expression | block)
pattern         → literalPattern | rangePattern | destructurePattern
                | bindingPattern | wildcardPattern
literalPattern  → NUMBER | STRING | "true" | "false" | "null"
rangePattern    → expression (".." | "..=") expression
destructurePattern → IDENTIFIER "(" pattern ("," pattern)* ")"
                   | "[" listPatternElements? "]"
                   | "(" pattern ("," pattern)+ ")"
listPatternElements → pattern ("," pattern)* ("," "..." IDENTIFIER)?
bindingPattern  → IDENTIFIER
wildcardPattern → "_"

block           → "{" statement* "}"
labeledStmt     → IDENTIFIER ":" (whileStmt | forStmt | loopStmt)
deferStmt       → "defer" statement
breakStmt       → "break" IDENTIFIER? ";"
continueStmt    → "continue" IDENTIFIER? ";"

expression      → ternary
ternary         → "if" expression "then" expression "else" expression | logicOr
logicOr         → logicAnd ("||" logicAnd)*
logicAnd        → isExpr ("&&" isExpr)*
isExpr          → equality (("is" | "is" "not") pattern)?
equality        → comparison (("==" | "!=") comparison)*
comparison      → shellExpr (("<" | ">" | "<=" | ">=") shellExpr)*
shellExpr       → bitOr (("<-" | "->" | "<<-" | "->>" | ">-") IDENTIFIER)*
bitOr           → bitXor ("|" bitXor)*
bitXor          → bitAnd ("^" bitAnd)*
bitAnd          → shift ("&" shift)*
shift           → term (("<<" | ">>") term)*
term            → factor (("+" | "-") factor)*
factor          → unary (("*" | "/" | "%") unary)*
unary           → ("!" | "-" | "~") unary | errorProp
errorProp       → postfix "?"?
postfix         → primary (call | typeParamCall | index | member)*
call            → "(" argList? ")"
typeParamCall   → "[" type "]" "(" argList? ")"
index           → "[" expression "]"
member          → ("." | "?.") IDENTIFIER

primary         → NUMBER | STRING | "true" | "false" | "null"
                | IDENTIFIER | "(" expression ")" | listLiteral | dictLiteral
                | coreLiteral | vectorLiteral | tupleLiteral | rangeLiteral
                | closure | innerExpr | outerExpr | innerAllExpr | outerAllExpr
                | siblingExpr | siblingAllExpr | spawnExpr | qualifiedCall

innerExpr       → "inner" typeArgument? failMode? "." IDENTIFIER "(" argList? ")"
outerExpr       → "outer" typeArgument? failMode? "." IDENTIFIER "(" argList? ")"
innerAllExpr    → "inner_all" "[" IDENTIFIER "]"
outerAllExpr    → "outer_all" "[" IDENTIFIER "]"
siblingExpr     → "sibling" "[" IDENTIFIER "]" "?"?
siblingAllExpr  → "sibling_all" "[" IDENTIFIER "]"
spawnExpr       → "spawn" callExpr
callExpr        → IDENTIFIER "(" argList? ")"
typeArgument    → "[" IDENTIFIER "]"
failMode        → "!" | "?"

listLiteral     → "[" (expression ("," expression)*)? "]"
dictLiteral     → "{" (dictEntry ("," dictEntry)*)? "}"
dictEntry       → expression ":" expression

coreLiteral     → "(" IDENTIFIER ")" "{" (fieldInit ("," fieldInit)*)? "}"
                | "{" (fieldInit ("," fieldInit)*)? "}"
fieldInit       → IDENTIFIER ":" expression | expression

vectorLiteral   → ("vec2" | "vec3" | "vec4") "(" expression ("," expression)* ")"
                | ("mat2" | "mat3" | "mat4") "(" expression ("," expression)* ")"

tupleLiteral    → "(" expression ("," expression)+ ")"

rangeLiteral    → expression (".." | "..=") expression

closure         → "(" paramList? ")" captureList? ("->" type)? block
captureList     → "[" captureItem ("," captureItem)* "]"
captureItem     → "*" IDENTIFIER

qualifiedCall   → IDENTIFIER "." IDENTIFIER "(" argList? ")"

enumVariant     → IDENTIFIER ("(" type ("," type)* ")")?

decorator       → "@" decoratorName decoratorArgs?
decoratorArgs   → "(" (IDENTIFIER | argList)? ")"
                // @change(fieldName) takes identifier; others take expressions
decoratorName   → "comptime" | "debug" | "on_update" | "on_fixed_update"
                | "on_collision" | "on_trigger" | "on_destroy"
                | "on_enable" | "on_disable" | "on_hot_reload" | "change"
                | "hot_reload_restart" | "hot_reload_preserve" | "hot_reload_cancel"

typeField       → type IDENTIFIER
typeList        → type ("," type)*
argList         → positionalArgs? namedArgs?
positionalArgs  → expression ("," expression)*
namedArgs       → IDENTIFIER ":" expression ("," IDENTIFIER ":" expression)*
```

---

## Appendix B: Operator Precedence

| Precedence | Operators | Associativity |
|------------|-----------|---------------|
| 1 (lowest) | `=` `+=` `-=` `*=` `/=` `%=` `&=` `\|=` `^=` `<<=` `>>=` | Right |
| 2 | `??` | Right |
| 3 | `\|\|` | Left |
| 4 | `&&` | Left |
| 5 | `is` `is not` | Left |
| 6 | `==` `!=` | Left |
| 7 | `<` `>` `<=` `>=` | Left |
| 8 | `<-` `->` `<<-` `->>` `>-` (shell ops) | Left |
| 9 | `\|` | Left |
| 10 | `^` | Left |
| 11 | `&` | Left |
| 12 | `<<` `>>` | Left |
| 13 | `+` `-` | Left |
| 14 | `*` `/` `%` | Left |
| 15 | `..` `..=` | Left |
| 16 | `!` `-` `~` (unary) | Right |
| 17 | `?` (error propagation) | Postfix |
| 18 | `.` `?.` `()` `[]` | Left |
| 19 (highest) | Primary | - |

**Disambiguation examples:**

```electron
a && b is Foo       // a && (b is Foo) - 'is' binds tighter than &&
a == b is Foo       // (a == b) is Foo - 'is' binds looser than ==
x is Foo && y > 0   // (x is Foo) && (y > 0)
```

---

## Appendix C: Type Sizes and Alignment

| Type | Size (bytes) | Alignment |
|------|--------------|-----------|
| `int` | 4 | 4 |
| `float` | 4 | 4 |
| `bool` | 1 | 1 |
| `vec2` | 8 | 4 |
| `vec3` | 12 | 4 |
| `vec4` | 16 | 4 |
| `mat2` | 16 | 4 |
| `mat3` | 36 | 4 |
| `mat4` | 64 | 4 |
| `range` | 12 | 4 |
| `string` | pointer (8 on 64-bit) | 8 |
| `list[T]` | pointer | 8 |
| `dict[K->V]` | pointer | 8 |
| Core | pointer | 8 |
| Struct | sum of fields (no limit, warning if > 64) | max field alignment |
| Tuple | sum of elements | max element alignment |
| `never` | 0 (uninhabited) | - |

---

## Appendix D: Default Values

| Type | Default Value |
|------|---------------|
| `int` | `0` |
| `float` | `0.0` |
| `bool` | `false` |
| `string` | `""` |
| `vec2` | `vec2(0, 0)` |
| `vec3` | `vec3(0, 0, 0)` |
| `vec4` | `vec4(0, 0, 0, 0)` |
| `mat2` | Identity |
| `mat3` | Identity |
| `mat4` | Identity |
| `range` | `0..0` |
| `list[T]` | `[]` |
| `dict[K->V]` | `{}` |
| Core | All fields at defaults |
| Struct | All fields at defaults |
| Nullable type | `null` |
| `never` | (uninhabited - no default) |

---

## Appendix E: Shell Dispatch Implementation

Shell method dispatch uses per-call-site caching for O(1) performance:

```
// Conceptual implementation
struct CallSiteCache {
    uint64_t shell_version;  // 64-bit to avoid wraparound
    void* handler;           // Cached function pointer
    void* inner_handler;     // For inner delegation
};

// Each core tracks its shell version
struct Core {
    uint64_t shell_version;
    // ...
};

void push_shell(Core* core, Shell* shell) {
    // ... attach shell ...
    core->shell_version++;  // Invalidate all caches
}

// Runtime dispatch
void take_damage_cached(Core* core, int amount, CallSiteCache* cache) {
    if (core->shell_version == cache->version) {
        // Cache hit - direct call
        ((TakeDamageFn)cache->handler)(core, amount);
    } else {
        // Cache miss - rebuild
        cache->handler = lookup_method(core, "take_damage");
        cache->version = core->shell_version;
        ((TakeDamageFn)cache->handler)(core, amount);
    }
}
```

---

## Appendix F: Performance Characteristics

| Operation | Complexity | Notes |
|-----------|------------|-------|
| `entity[Core]` lookup | O(n) where n = cores on entity | Typically <10 |
| `scene.query[T1, T2]()` | O(entities) | Consider caching results |
| Shell method dispatch | O(1) amortized | Cached per call-site |
| Shell push/pop | O(n × m) | n = shells, m = restrictions |
| `inner.method()` | O(1) amortized | Cached per call-site |
| `inner[T].method()` | O(1) amortized | Cached per call-site |
| `outer[T].method()` | O(1) amortized | Cached per call-site |
| Cycle detection | O(d) | d = call depth |

**Typed Call Caching Strategy:**

Typed lookups (`inner[T]`, `outer[T]`) maintain per-call-site caches:

```
struct TypedCallCache {
    stack_version: uint64,     // For cache invalidation
    target_shell_index: int,   // -1 if not found
    method_ptr: FunctionPtr,
}
```

Cache is invalidated when `stack.version` changes (any push/pop operation).

**Optimization Strategies:**
1. Cache entity queries per frame
2. Prefer fewer, larger cores over many small ones
3. Minimize shell stack changes during hot paths
4. Use typed calls (`inner[T]!`) when the target shell is known to skip search
5. Consider `requires inner` to guarantee shell presence and avoid runtime checks
