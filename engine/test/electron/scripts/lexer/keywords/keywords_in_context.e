// Keywords used in proper syntactic context

// Type declarations with type keywords
bool is_active = true;
int count = 42;
float temperature = 98.6;
string message = "hello";

// Vector and matrix usage
vec2 position2d = vec2(10.0, 20.0);
vec3 position = vec3(1.0, 2.0, 3.0);
vec4 color = vec4(1.0, 0.0, 0.0, 1.0);

// Collection types
list[int] numbers;
dict[string -> int] ages;
tuple[int, string] pair;

// Const declarations
const MAX_HEALTH = 100;
const GRAVITY = vec3(0, -9.81, 0);

// Function definitions
def void_function() {
    return;
}

def add(int a, int b) -> int {
    return a + b;
}

def greet(string name) -> string {
    return "Hello, " + name;
}

// Control flow
if count > 0 {
    print("positive");
} else {
    print("non-positive");
}

while count > 0 {
    count -= 1;
}

do {
    process();
} while running;

loop {
    if done { break; }
}

for int i in 0..10 {
    print(i);
}

// Match expression
match state {
    State.Idle => idle_action(),
    State.Moving(pos) => move_to(pos),
    _ => default_action()
}

// Defer
def cleanup_example() {
    defer cleanup();
    risky_operation();
}

// Type checking
if value is int {
    use_int(value as int);
}

if not is_valid {
    return error("invalid");
}

// Boolean logic with keywords
if enabled and visible {
    render();
}

if paused or stopped {
    wait();
}

// Error handling
def risky() -> result[int, string] {
    if bad_condition {
        return error("something went wrong");
    }
    return success(42);
}

// Coroutines
coro[Stats] c = spawn load_player();
Stats player = yield c;

// Core and shell
core Player {
    public int health;
    readonly int id;
    vec3 position;
}

shell Damageable {
    requires { int hp; int max_hp; }

    def take_damage(int amount) -> bool {
        if inner.hp <= 0 { return false; }
        inner.hp -= amount;
        return true;
    }
}

// Shell attachment
entity <- Damageable <- Combat;
entity -> Frozen;

if entity >- Buff {
    apply_effect();
}

// Imports
import math;
from physics import RigidBody, Collider;
export def public_api() { }

// Enum with variants
enum State {
    Idle,
    Moving(vec3),
    Attacking(int),
    Dead
}

// Struct
struct Color {
    public float r;
    public float g;
    public float b;
    public float a;
}

// Nullable types
string? optional_name = null;
entity? target = find_target();

// Never type (for functions that don't return)
def abort() -> never {
    exit(1);
}
