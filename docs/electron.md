# Electron Language Specification

**Version:** 2.0
**Status:** Draft
**File Extension:** `.e`

Electron is a statically-typed scripting language designed for the Atom game engine. It features a unique composition-based object model using "cores" (data) and "shells" (behavior) instead of traditional classes, enabling flexible runtime behavior modification through shell stacking.

**Architecture Overview:**
- **Entity:** Opaque engine handle. An ID that groups components.
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
7. [Cores](#7-cores)
8. [Shells](#8-shells)
9. [Enums](#9-enums)
10. [Error Handling](#10-error-handling)
11. [Coroutines](#11-coroutines)
12. [Modules and Imports](#12-modules-and-imports)
13. [Engine Integration](#13-engine-integration)
14. [Standard Library](#14-standard-library)

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
bool        break       const       continue    core
coro        def         defer       dict        do
else        enum        false       float       for
from        if          import      in          inner
int         list        loop        match       mat2
mat3        mat4        mut         null        public
range       readonly    requires    return      shell
strict      string      then        true        type
unique      value       vec2        vec3        vec4
while       yield
```

**Reserved for Future Use:**
```
async       await       class       extends     implements
new         private     protected   static      super
this        throw       try         catch       finally
with        as          is          sizeof
```

### 1.4 Numeric Literals

```electron
42              // Integer
1_000_000       // Integer with separators
0xFF            // Hexadecimal
0b1010          // Binary
3.14            // Float
3.14f           // Explicit float
1.5e-10         // Scientific notation
```

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

**String Optimizations:**
- Compile-time folding: `"Error: " + "404"` → `"Error: 404"`
- Identical string literals are interned (share same reference)

---

## 2. Types

Electron is statically typed. All types must be explicitly declared.

### 2.1 Primitive Types

| Type | Description | Size |
|------|-------------|------|
| `int` | Signed integer | 32-bit |
| `float` | Floating-point number | 32-bit |
| `bool` | Boolean value (`true` or `false`) | 1 byte |

**Integer Overflow:** Integer arithmetic wraps on overflow (two's complement).

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
v.xx = vec2(1.0, 2.0);  // Error: aliasing pattern
```

### 2.3 Range Type

`range` is a built-in value type representing a sequence of integers.

| Type | Description | Size |
|------|-------------|------|
| `range` | Integer range with bounds and inclusivity | 9 bytes |

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

Tuples are fixed-size, heterogeneous value types.

**Declaration and Construction:**

```electron
(int, int) point = (10, 20);
(string, int, bool) record = ("Alice", 30, true);
```

**Access:**

```electron
int x = point.0;    // 10
int y = point.1;    // 20
```

**Destructuring:**

```electron
(int x, int y) = point;
(string name, int age, bool active) = record;

// Partial destructuring with wildcards
(string name, _, _) = record;
```

**Function Returns:**

```electron
def min_max(list[int] nums) -> (int, int) {
    int min_val = nums[0];
    int max_val = nums[0];
    for n in nums {
        if n < min_val { min_val = n; }
        if n > max_val { max_val = n; }
    }
    return (min_val, max_val);
}

(int lo, int hi) = min_max(numbers);
```

Tuples are value types: copied on assignment, cannot be nullable. Tuples must have at least two elements.

### 2.5 String Type

```electron
string greeting = "Hello";
```

Strings are immutable. Operations that modify a string return a new string.

### 2.6 Collection Types

**List:**

A dynamically-sized array of elements of a single type.

```electron
list[int] numbers = [1, 2, 3, 4, 5];
list[string] names = ["Alice", "Bob"];
list[vec3] points = [vec3(0, 0, 0), vec3(1, 1, 1)];
```

**Dictionary:**

A key-value mapping. Keys must be hashable: primitives, strings, or enums without data.

```electron
dict[string -> int] ages = {"alice": 30, "bob": 25};
dict[int -> string] lookup = {1: "one", 2: "two"};
```

### 2.7 Function Type

Functions are first-class values. Function types specify their parameter types and return type.

**Basic Function Types:**

```electron
// Fully typed function variable
(int, int) -> int adder = (int a, int b) -> int { return a + b; };

// Function that takes no arguments and returns void
() -> void callback = () { print("called"); };

// Function as parameter
def apply(int x, (int) -> int transform) -> int {
    return transform(x);
}
```

### 2.8 Nullable Types

By default, values cannot be `null`. Use the `?` suffix to make a reference type nullable.

**Only reference types can be nullable:**

| Category | Types | Nullable? |
|----------|-------|-----------|
| Primitives | `int`, `float`, `bool` | No |
| Value types | `vec*`, `mat*`, `range`, tuples, value cores | No |
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
string? name = get_name();
int? len = name?.length;  // null if name is null
```

**Null Coalescing Operator (`??`):**

Returns the left operand if non-null, otherwise the right operand:

```electron
string? name = null;
string display = name ?? "Unknown";  // "Unknown"
```

**Runtime Behavior:**

Accessing a null value without safe access (`?.`) or null check causes a panic with a stack trace.

### 2.9 Type Aliases

Create compile-time aliases for existing types:

```electron
type EntityId = int;
type Vec3List = list[vec3];
type Callback = (entity, float) -> bool;
type Position = vec3;
```

Type aliases are fully interchangeable with their underlying type at compile time. For distinct types that prevent accidental mixing, use cores:

```electron
core EntityId { int value; }
core PlayerId { int value; }

EntityId e = EntityId { value: 5 };
PlayerId p = PlayerId { value: 5 };
// e == p  // Compile error: different types
```

### 2.10 Type Coercion

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

### 2.11 Value vs Reference Semantics

| Category | Types | Semantics |
|----------|-------|-----------|
| Value | `int`, `float`, `bool`, `vec*`, `mat*`, `range`, tuples, value cores | Copied on assignment and function call |
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

`@comptime` constants are inlined by the compiler and support:
- Primitive literals (`int`, `float`, `bool`, `string`)
- Arithmetic on other `@comptime` constants
- String concatenation of `@comptime` constants

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

Entity e = Entity { id: 42, hp: 100 };
e.hp = 50;    // OK
e.id = 99;    // Error: readonly field
```

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
| `?` | Error propagation |
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
for num in numbers {
    print(num);
}

// With index
for i, num in numbers {
    print("Index {i}: {num}");
}

// Dictionary iteration
dict[string -> int] ages = {"alice": 30, "bob": 25};
for key, value in ages {
    print("{key} is {value} years old");
}

// Keys only
for key in ages {
    print(key);
}

// Range iteration
for i in 0..10 {
    print(i);  // 0, 1, 2, ..., 9
}

for i in 0..=10 {
    print(i);  // 0, 1, 2, ..., 10
}
```

**Loop Control:**

```electron
for item in items {
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
outer: for i in 0..10 {
    for j in 0..10 {
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
def process_file(string path) -> Result[Data, Error] {
    File f = open(path)?;
    defer f.close();  // Always runs when scope exits

    Buffer b = allocate(1024);
    defer free(b);

    if check_header(f) == false {
        return Err(Error { message: "Invalid header" });
        // f.close() and free(b) run here
    }

    Data d = parse(f, b)?;
    return Ok(d);
    // f.close() and free(b) run here
}
```

**Semantics:**

- Multiple defers execute in reverse order (LIFO)
- Defers run on normal exit, early return, and error propagation (`?`)
- Defers do **not** run on panic

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
match result {
    Ok(value) => use_value(value),
    Err(e) => handle_error(e)
}
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

// Also correct: explicit wildcard
match x {
    n if n < 0 => "negative",
    _ => "non-negative"
}
```

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

(int, int) -> int operation = add;
int result = operation(2, 3);  // 5

// Function as parameter
def apply((int) -> int f, int x) -> int {
    return f(x);
}

// Storing in collections
list[(int) -> int] transforms = [
    (int x) -> int { return x * 2; },
    (int x) -> int { return x + 1; }
];
```

### 6.3 Closures (Anonymous Functions)

```electron
(int) -> int doubler = (int x) -> int {
    return x * 2;
};

(string) -> void greeter = (string name) {
    print("Hello, {name}!");
};

// Inline usage
list[int] doubled = numbers.map((int x) -> int { return x * 2; });
```

**Capturing Variables:**

By default, closures capture variables by value (snapshot at creation time):

```electron
int x = 10;
() -> void f = () {
    print(x);
};
x = 20;
f();  // Prints 10 (captured value at creation)
```

To capture by reference, use `[*variable]`:

```electron
int x = 10;
() -> void f = () [*x] {
    print(x);
};
x = 20;
f();  // Prints 20 (reference to x)
```

Multiple captures:

```electron
int a = 1;
int b = 2;
() -> void f = () [*a, *b] {
    print(a + b);
};
```

**Reference Capture Restrictions:**

Closures with reference captures cannot escape their defining scope:

```electron
// OK: closure used within same scope
def process(list[int] items) {
    int sum = 0;
    items.for_each((int x) [*sum] {
        sum += x;
    });
    print(sum);
}

// OK: stored in variable but used in same scope
def example() {
    int x = 10;
    () -> void f = () [*x] { print(x); };
    f();      // OK
    f();      // OK
    // f cannot be returned or stored in a field
}

// Error: reference capture cannot escape
def make_counter() -> () -> int {
    int count = 0;
    return () [*count] -> int {  // Error!
        count += 1;
        return count;
    };
}
```

To create escaping closures with mutable state, use a core:

```electron
core Counter {
    public int count;
}

def make_counter() -> () -> int {
    Counter c = Counter { count: 0 };
    return () -> int {  // Value capture of reference type
        c.count += 1;
        return c.count;
    };
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

## 7. Cores

Cores are data structures similar to C structs. They hold state but define no behavior.

### 7.1 Core Declaration

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

Core literals always require a type prefix:

```electron
Player p = Player {
    health: 100,
    position: vec3(0, 0, 0)
};

// Positional syntax (must match declaration order)
Player p2 = Player { 100, vec3(0, 0, 0) };

// Anonymous core literal only in typed context
Player p3 = { health: 100, position: vec3(0, 0, 0) };  // OK: type known
do_something({ health: 50 });  // Error: ambiguous, use Player { health: 50 }
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

GameObject obj = GameObject {
    id: 1,
    trans: Transform {
        position: vec3(0, 0, 0),
        rotation: vec3(0, 0, 0),
        scale: vec3(1, 1, 1)
    },
    name: "Player"
};
```

### 7.5 Core Methods

Cores themselves cannot have methods. All behavior is defined in shells that attach to cores.

### 7.6 Value Cores

Value cores are small, stack-allocated data structures with value semantics.

**Declaration:**

```electron
value core Color {
    public float r;
    public float g;
    public float b;
    public float a;
}

value core Rect {
    public float x;
    public float y;
    public float width;
    public float height;
}

value core AABB {
    public vec3 min;
    public vec3 max;
}
```

**Constraints:**

| Constraint | Rationale |
|------------|-----------|
| Maximum 64 bytes total size | Efficient stack allocation and copying |
| No shell attachment | Shells require reference semantics |
| Cannot be nullable | Nullable implies reference; value types always exist |
| Copied on assignment | Value semantics, like `int` or `vec3` |
| Copied when passed to functions | Unless ref parameter |
| Can contain only value types | No `string`, `list`, or reference cores |
| No recursive/self-referential | Would require indirection |

**Size Calculation:**

```electron
value core Example {
    int a;      // 4 bytes
    int b;      // 4 bytes
    float c;    // 4 bytes
    vec3 d;     // 12 bytes
}               // Total: 24 bytes (OK)

value core TooBig {
    mat4 a;     // 64 bytes
    int b;      // 4 bytes
}               // Error: 68 bytes exceeds 64 byte limit
```

**Reference Parameters:**

Value cores can be passed by reference to avoid copying:

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

Color col = Color { r: 1.0, g: 0.0, b: 0.0, a: 1.0 };
set_color(col);           // Copies col
read_color(*col);         // Passes reference
modify_color(mut *col);   // Passes mutable reference
```

**In Collections:**

```electron
list[Color] palette = [
    Color { r: 1.0, g: 0.0, b: 0.0, a: 1.0 },
    Color { r: 0.0, g: 1.0, b: 0.0, a: 1.0 },
];

// Accessing copies the value out
Color c = palette[0];
c.r = 0.5;              // Does NOT modify palette[0]

// Modify in place with index assignment
palette[0].r = 0.5;     // Modifies palette[0] directly
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

**The `core` and `inner` Keywords:**

Within a shell, two special keywords are available:

| Keyword | Type | Description |
|---------|------|-------------|
| `core` | interface type | Reference to the attached core instance. Type-checked against the shell's requirements. |
| `inner` | shell reference | Reference to the next shell in the stack. Used for delegation. |

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

### 8.3 Shell State

Shells can have their own state, stored per-attachment:

```electron
shell Cooldown {
    float time_remaining = 0.0;

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
Player player = Player { hp: 100, position: vec3(0, 0, 0) };

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

Entity boss = Entity { hp: 1000, position: vec3(0, 0, 0) };
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

**Inner Call Behavior:**

| Syntax | Behavior |
|--------|----------|
| `inner.method()` | Silent no-op if no handler below |
| `inner!.method()` | Panic if no handler below |
| `inner?.method()` | Explicit silent no-op (same as `inner.method()`) |

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

### 8.7 Shell Composition

Shells can include other shells (like mixins):

```electron
shell Character: Movable, Damageable {
    requires { vec3 position, int hp }

    // Additional methods specific to Character
    def interact() {
        print("Interacting...");
    }
}
```

This copies all methods from `Movable` and `Damageable` into `Character`.

**Conflict Resolution:**

When composed shells define the same method, use qualified calls:

```electron
shell Character: Movable, Damageable {
    requires { vec3 position, int hp }

    // Both Movable and Damageable define update()
    def update(float dt) {
        Movable::update(dt);      // Call Movable's implementation
        Damageable::update(dt);   // Call Damageable's implementation
    }
}
```

Unresolved conflicts are compile errors.

**Shell Composition Sugar:**

```electron
shell PlayerCharacter = Movable + Damageable + Controllable;
```

Equivalent to shell composition with no additional methods.

### 8.8 Unique Shells

Mark a shell as `unique` to enforce only one instance per core:

```electron
unique shell PlayerController {
    def update(float dt) {
        // Handle input
    }
}

Player player = Player { hp: 100, position: vec3(0, 0, 0) };
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

### 8.9 Operator Overloading

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

| Method | Operator |
|--------|----------|
| `__add__` | `+` |
| `__sub__` | `-` |
| `__mul__` | `*` |
| `__div__` | `/` |
| `__mod__` | `%` |
| `__eq__` | `==` |
| `__neq__` | `!=` |
| `__lt__` | `<` |
| `__gt__` | `>` |
| `__lte__` | `<=` |
| `__gte__` | `>=` |
| `__neg__` | `-` (unary) |

---

## 9. Enums

Enums define a type with a fixed set of variants.

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

Electron uses explicit error handling with `Result` types instead of exceptions.

### 10.1 Result Type

`Result` is a built-in generic enum provided by the compiler:

```electron
// This is a compiler built-in, not user-definable
enum Result[T, E] {
    Ok(T),
    Err(E)
}
```

The compiler provides special support for `Result`:
- **Type inference:** `Ok(5)` infers `Result[int, ?]` where `?` is determined by context
- **Error propagation:** The `?` operator works with `Result` types
- **Pattern matching:** Full destructuring support in `match` expressions

```electron
def divide(int a, int b) -> Result[int, string] {
    if b == 0 {
        return Err("Division by zero");
    }
    return Ok(a / b);
}

Result[int, string] r1 = Ok(42);
Result[int, string] r2 = Err("failed");
```

### 10.2 Error Propagation

Use `?` to propagate errors automatically:

```electron
def load_config(string path) -> Result[Config, Error] {
    File f = open(path)?;           // Returns early if open fails
    string data = read_all(f)?;     // Returns early if read fails
    Config cfg = parse(data)?;      // Returns early if parse fails
    return Ok(cfg);
}
```

The `?` operator desugars to:

```electron
// expr?  becomes:
match expr {
    Ok(v) => v,
    Err(e) => return Err(e)
}
```

**Error Type Compatibility:**

The error types must match. If they don't, explicit conversion is required:

```electron
def process() -> Result[Data, AppError] {
    // If open returns Result[File, IoError], convert:
    File f = open(path).map_err((IoError e) -> AppError {
        return AppError { message: e.message, code: 1 };
    })?;
    return Ok(process_file(f));
}
```

### 10.3 Handling Results

```electron
Result[int, string] result = divide(10, 2);

match result {
    Ok(value) => print("Result: {value}"),
    Err(msg) => print("Error: {msg}")
}

// Or with if-is pattern
if result is Ok(value) {
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

def open_file(string path) -> Result[File, FileError] {
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
| Shell requirements mismatch | Error at attachment time |
| `inner!` with no handler | Panic |
| `entity.require[T]()` missing | Panic |

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

Coroutines communicate with the scheduler via `YieldRequest`, a built-in enum:

```electron
// Built-in yield types (compiler intrinsics)
enum YieldRequest {
    NextFrame,                    // Resume next frame
    Wait(float seconds),          // Resume after duration
    Until(() -> bool condition),  // Resume when condition true
    While(() -> bool condition),  // Resume when condition false
    Join(coro other),             // Resume when other completes
}
```

### 11.3 Declaring Coroutines

```electron
def enemy_patrol() -> coro {
    loop {
        move_to(point_a);
        yield YieldRequest.Wait(2.0);  // Explicit
        // Or with sugar:
        yield wait(2.0);               // wait() returns YieldRequest.Wait(2.0)
        move_to(point_b);
        yield;                         // Sugar for yield YieldRequest.NextFrame
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

```electron
coro patrol = spawn enemy_patrol();
coro fade = spawn fade_in(1.5);
```

### 11.5 Yield Expressions

| Expression | Behavior |
|------------|----------|
| `yield;` | Pause until next frame |
| `yield wait(seconds);` | Pause for duration |
| `yield until(condition);` | Pause until condition is true |
| `yield while(condition);` | Pause while condition is true |

### 11.6 Coroutine Control

```electron
coro c = spawn some_behavior();

// Check if running
if c.running() {
    print("Still going");
}

// Cancel
c.cancel();

// Wait for completion (blocks current coroutine)
yield c;
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

### 11.8 Engine Scheduler Contract

The engine manages coroutine execution:

1. `spawn` registers coroutine with engine scheduler
2. Engine calls `resume()` each frame on eligible coroutines
3. `resume()` returns `YieldRequest` telling engine when to call again
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

### 12.3 Exporting

By default, top-level declarations are private to the module. Use `public` to export:

```electron
// utils/math.e

public const PI = 3.14159;

public def clamp(float x, float min, float max) -> float {
    if x < min { return min; }
    if x > max { return max; }
    return x;
}

// Private helper, not exported
def internal_helper() {
    // ...
}

public core Vector2 {
    public float x;
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

### 13.2 Property Change Observers

React to field changes:

```electron
core Player {
    public int hp;
    public int max_hp;
}

shell HealthUI {
    @change(hp)
    def on_hp_changed(int old_value, int new_value) {
        update_health_bar(new_value, core.max_hp);

        if new_value < old_value {
            flash_damage_indicator();
        }
    }
}
```

### 13.3 The `entity` Type

`entity` is an opaque engine handle representing a container for cores.

**Entity Lifecycle:**

```electron
// Spawn a new entity
entity e = entity.spawn();

// Destroy an entity
entity.destroy(e);

// Check if alive
if entity.alive(e) { }

// Get entity ID
int id = e.id;
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
player.add(PlayerData { hp: 100, position: vec3(0, 0, 0), score: 0 });
player.add(Transform { position: vec3(0, 0, 0), rotation: vec3(0, 0, 0), scale: vec3(1, 1, 1) });

// Retrieve cores (nullable if missing)
PlayerData? data = player.get[PlayerData]();

// Retrieve cores (panic if missing)
Transform t = player.require[Transform]();

// Check for core
if player.has[PlayerData]() { }

// Remove core
player.remove[PlayerData]();
```

**Shell Attachment:**

Shells attach to cores, not entities:

```electron
PlayerData data = player.require[PlayerData]();
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
for e in enemies {
    Enemy enemy = e.require[Enemy]();
    Transform t = e.require[Transform]();
    // ...
}
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
nums.clear();              // Remove all

int len = nums.length;
bool empty = nums.is_empty();
bool has = nums.contains(2);
int idx = nums.index_of(2);  // -1 if not found

// Functional operations
list[int] doubled = nums.map((int x) -> int { return x * 2; });
list[int] evens = nums.filter((int x) -> bool { return x % 2 == 0; });
int sum = nums.reduce(0, (int acc, int x) -> int { return acc + x; });
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

---

## Appendix A: Grammar Summary

```
program         → declaration* EOF
declaration     → constDecl | coreDecl | shellDecl | enumDecl
                | funcDecl | importDecl | typeAlias

constDecl       → decorator* "const" IDENTIFIER "=" expression ";"
coreDecl        → "value"? "core" IDENTIFIER "{" fieldDecl* "}"
shellDecl       → "strict"? "unique"? "shell" IDENTIFIER (":" IDENTIFIER ("," IDENTIFIER)*)?
                  "{" requiresDecl? (fieldDecl | funcDecl)* "}"
enumDecl        → "enum" IDENTIFIER "{" enumVariant ("," enumVariant)* "}"
funcDecl        → decorator* "def" IDENTIFIER "(" paramList? ")" ("->" type)? block
importDecl      → "import" STRING ("as" IDENTIFIER)?
                | "from" STRING "import" IDENTIFIER ("," IDENTIFIER)*
typeAlias       → "type" IDENTIFIER "=" type ";"

requiresDecl    → "requires" "{" typeField ("," typeField)* "}"
fieldDecl       → "public"? "readonly"? type IDENTIFIER ("=" expression)? ";"
paramList       → param ("," param)*
param           → "mut"? "*"? type IDENTIFIER ("=" expression)?

type            → primitiveType | collectionType | funcType | builtinType
                | tupleType | nullableType | IDENTIFIER
primitiveType   → "int" | "float" | "bool" | "string"
                | "vec2" | "vec3" | "vec4" | "mat2" | "mat3" | "mat4" | "range"
collectionType  → "list" "[" type "]" | "dict" "[" type "->" type "]"
funcType        → "(" typeList? ")" "->" type
builtinType     → "coro" ("[" type "]")? | "entity"
                | "Result" "[" type "," type "]"
tupleType       → "(" type ("," type)+ ")"
nullableType    → type "?"

statement       → exprStmt | varDecl | assignment | ifStmt | whileStmt
                | doWhileStmt | loopStmt | forStmt | matchStmt
                | returnStmt | yieldStmt | breakStmt | continueStmt
                | deferStmt | labeledStmt | block
block           → "{" statement* "}"

labeledStmt     → IDENTIFIER ":" (whileStmt | forStmt | loopStmt)
deferStmt       → "defer" statement
breakStmt       → "break" IDENTIFIER? ";"
continueStmt    → "continue" IDENTIFIER? ";"

expression      → ternary
ternary         → "if" expression "then" expression "else" expression | logicOr
logicOr         → logicAnd ("||" logicAnd)*
logicAnd        → equality ("&&" equality)*
equality        → comparison (("==" | "!=") comparison)*
comparison      → shellExpr (("<" | ">" | "<=" | ">=") shellExpr)*
shellExpr       → bitOr (("<-" | "->" | "<<-" | "->>" | ">-") IDENTIFIER)*
bitOr           → bitXor ("|" bitXor)*
bitXor          → bitAnd ("^" bitAnd)*
bitAnd          → shift ("&" shift)*
shift           → term (("<<" | ">>") term)*
term            → factor (("+" | "-") factor)*
factor          → unary (("*" | "/" | "%") unary)*
unary           → ("!" | "-" | "~") unary | postfix
postfix         → primary (call | typeParamCall | index | member | innerCall)*
call            → "(" argList? ")"
typeParamCall   → "[" type "]" "(" argList? ")"
index           → "[" expression "]"
member          → ("." | "?.") IDENTIFIER
innerCall       → "inner" ("!" | "?")? "." IDENTIFIER "(" argList? ")"

primary         → NUMBER | STRING | "true" | "false" | "null"
                | IDENTIFIER | "(" expression ")" | listLiteral | dictLiteral
                | coreLiteral | vectorLiteral | tupleLiteral | rangeLiteral | closure
                | qualifiedCall

listLiteral     → "[" (expression ("," expression)*)? "]"
dictLiteral     → "{" (dictEntry ("," dictEntry)*)? "}"
dictEntry       → expression ":" expression

coreLiteral     → IDENTIFIER "{" (fieldInit ("," fieldInit)*)? "}"
fieldInit       → IDENTIFIER ":" expression | expression

vectorLiteral   → ("vec2" | "vec3" | "vec4") "(" expression ("," expression)* ")"
                | ("mat2" | "mat3" | "mat4") "(" expression ("," expression)* ")"

tupleLiteral    → "(" expression ("," expression)+ ")"

rangeLiteral    → expression (".." | "..=") expression

closure         → "(" paramList? ")" captureList? ("->" type)? block
captureList     → "[" captureItem ("," captureItem)* "]"
captureItem     → "*" IDENTIFIER

qualifiedCall   → IDENTIFIER "::" IDENTIFIER "(" argList? ")"

enumVariant     → IDENTIFIER ("(" type ("," type)* ")")?

decorator       → "@" decoratorName ("(" argList? ")")?
decoratorName   → "comptime" | "debug" | "on_update" | "on_fixed_update"
                | "on_collision" | "on_trigger" | "on_destroy"
                | "on_enable" | "on_disable" | "on_hot_reload" | "change"

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
| 5 | `==` `!=` | Left |
| 6 | `<` `>` `<=` `>=` | Left |
| 7 | `<-` `->` `<<-` `->>` `>-` (shell ops) | Left |
| 8 | `\|` | Left |
| 9 | `^` | Left |
| 10 | `&` | Left |
| 11 | `<<` `>>` | Left |
| 12 | `+` `-` | Left |
| 13 | `*` `/` `%` | Left |
| 14 | `..` `..=` | Left |
| 15 | `!` `-` `~` (unary) | Right |
| 16 | `.` `?.` `()` `[]` | Left |
| 17 (highest) | Primary | - |

---

## Appendix C: Type Sizes

| Type | Size (bytes) |
|------|--------------|
| `int` | 4 |
| `float` | 4 |
| `bool` | 1 |
| `vec2` | 8 |
| `vec3` | 12 |
| `vec4` | 16 |
| `mat2` | 16 |
| `mat3` | 36 |
| `mat4` | 64 |
| `range` | 9 |
| `string` | pointer (8 on 64-bit) |
| `list[T]` | pointer |
| `dict[K->V]` | pointer |
| Core | pointer |
| Value core | sum of fields (max 64) |
| Tuple | sum of elements |

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
| Value core | All fields at defaults |
| Nullable type | `null` |

---

## Appendix E: Shell Dispatch Implementation

Shell method dispatch uses per-call-site caching for O(1) performance:

```
// Conceptual implementation
struct CallSiteCache {
    uint32_t shell_version;  // Incremented on any shell change
    void* handler;           // Cached function pointer
    void* inner_handler;     // For inner delegation
};

// Each core tracks its shell version
struct Core {
    uint32_t shell_version;
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
