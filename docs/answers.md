1 - 
int x = 5           // Cannot be null
string? name = null // Explicitly nullable
dict? cache = null  // Explicitly nullableAccessing a null optional without checking is a runtime error (not undefined behavior, not silent default). This catches bugs fast. Provide safe access:
name?.length        // Returns null if name is null
name ?? "default"   // Null coalescing

2 - all typing is explicit, only conversions that are allowed are between int and float, and between bool and (int, float, string, list, dict) where empty or zero is false and any value other than that is true. 
conversion should work like python casting, with "(type1) variable_name;" syntax 

3 - strings are immutable 
4 - 

int, float, bool, vec2, vec3, vec4, mat2, mat3, mat4 - passed by value 
string - immutable reference 
list, dict, core, func - reference 

let a = vec3(1, 2, 3)
let b = a           // Copy
b.x = 99            // a.x still 1

5 - for now, A simple mark-and-sweep GC, run at controlled points (end of frame, loading screens), is more predictable for games. Let the engine control when collection happens:

// Engine side
vm.collect_garbage()  // Called between frames

6 - constructor syntax, reseve braces for dicts, cores, and shells 

7 - within a shell, "core" refers to the core this shell is currently wrapped around

8 - 

when a shell is defined, there should be an optional core interface definition that indicates the cores it accepts 

`
shell movable {
  interface { float x, float y }
  ...
}
`

9 - yes, any core that matches the interface for the shell. 

10 - 

to push 

Player << frozen  

to pop 

Player >>

11 - yes, shells can have state
Shell state is per-attachment. If you push Cooldown twice, each has its own time_remaining. This is stored alongside the shell reference in the object's shell stack.

12 - 

composition is allowed, not inheritance. 

here is the syntax 

```c 
shell character: movable, draggable { 
  interface {
    ...
  };
}
```
where movable and draggable are other shells 
 copies the methods in (like a mixin). No inheritance hierarchy, no diamond problem. If there are conflicts, it's a compile error—you must resolve explicitly.

13 - 
shell Slowed {
    fn move(self, dx: float, dy: float) {
        inner.move(dx * 0.5, dy * 0.5)  // Delegate to next shell
    }
}

shell Frozen {
    fn move(self, dx: float, dy: float) {
        // No inner call = movement blocked entirely
    }
}

14 - 
if player >- Frozen {
    show_frozen_indicator()
}

15 - 
args list 

16 - no overloading. 

17 - yes you can, absolutely, functions also have type "func" which is a reference to a function. 

18 - by value by default

```c 
int x = 10;
func f = () {
    print(x);
};
x += 20;
f(); // prints 10

int y = 10; 
func g = () [*y] {
    print(y);
};
y += 20;
g(); // prints 30
```


19 - yes, default parameters are allowed
20 - omit return type for void 
21 - match case with pattern matching instead of switch case, for item in list iterator loops, while and do while both supported
break
continue
loop (alias for while true)
for i, item in dict 
for item in dict 
if, else if, else 

22 - yes, this should be at the shell level 

```c 
shell vec_ops {
    interface vec3;

    def __add__(vec3 other) -> vec3 {
        return vec3(core.x + other.x, core.y + other.y, core.z + other.z);
    }
}
```

23 - + operator with string interpolation via. inline curly brackets 

24 - reference equality by default, with an overridable == operator. 

25 - yes, should be 

if condition then a else b 

26 - explicit imports with module paths, syntax like python 

```py 
import "weapons/sword" 
import "utils" as utils 
from "utils" import rotate, explode
```

27 - Each file is implicitly a module. Importing brings symbols into scope. No separate namespace keyword needed:

28 - allow with a warning 

29 - no forward declaration, two pass resoultion instead 

30 - Recommendation: Minimal core, rich standard library
In the language core:

print(), assert()
Math: sin, cos, sqrt, abs, min, max, clamp, lerp
Type queries: typeof(), is

In engine-provided modules:
import "engine/entity"
import "engine/input"
import "engine/time"

let dt = time.delta()
if input.pressed("jump") {
    // ...
}
let e = entity.spawn("enemy")
Keep the language itself engine-agnostic. All engine specifics come through imported modules.


31 - use your best judgement for this 

32 - decorators on load time 

@on_collision 
def handle_collision(entity other) {
    if other -< bullet {
        self.damage(10);
    }
}

33 - Yes, design for it from the start
This means:

Shells are looked up by name at runtime (not baked pointers)
Core data is preserved, shell code is swapped
Function references are indirect through a registry

The shells model helps here—cores (data) survive, shells (behavior) reload cleanly.

34 - Recommendation: Explicit errors, not crashes or silent defaults
ErrorBehaviorNull dereferenceRuntime errorArray out of boundsRuntime errorDivision by zeroReturn inf or nan for float, error for intShell requirement not metError at attachment
Errors include stack traces. In debug mode, break into debugger.

35 - 

def load_config(string path) -> Result[config, error] {
    file File = open(path)?  // Propagates error if failed
    string data = parse(File)?
    return Ok(data)
}

// Usage
match load_config("game.cfg") {
    Ok(cfg) => apply(cfg),
    Err(e) => print("Failed: {e}")
}

No exceptions. Explicit error handling is more predictable for games.

36 - always static, never dynamically typed. 

37 - both  
38 - 

"line1\nline2"
"tab\there"
r"no\escapes\here"     // Raw string
"""
multiline
string
"""

39 - 
1_000_000
0xFF
0b1010
1.5e-10
3.14f        // Explicit float (vs double if you had both)
all of them

40 - that's good 



2. Built-in entity queries: YES, but as a library function, not syntax:
3. Coroutines: ABSOLUTELY YES. This is huge for game scripting:

def enemy_behavior(self) -> coro {
    loop {
        patrol(self);
        yield wait(1.0);
        if player_nearby(self) {
            attack(self);
            yield wait(0.5);
        }
    }
}

4. yes, this should be allowed
5. Shell composition: YES:
shell PlayerCharacter = Movable + Damageable + Controllable
should just be syntactic sugar for includes



always explicit, let was a mistake
always core, never self, it was a mistake 

coroutine details
spawn is good , "coro.cancel()" is also good. yes, yes 

to callback, there is a second decorators

shell health_ui {
    @change(hp)
    def on_hp_changed(int old, int new) {
        update_health_bar(new);
    }
}

all of the patterns 

that's a fine enough initial API 

error type: 
error is not a built in type, but it is a core in the standard library.


private by default, explicit public, the keyword should be "public" 

const should exist for compile time constants 


enums should also exist with that syntax

type aliases

These are purely compile-time aliases—no runtime cost, no distinct type. EntityId and int are fully interchangeable.
For distinct types (where you want type safety and can't accidentally mix them), use a newtype pattern:
type EntityId = int
type Vec3List = list[vec3]
type Callback = (Entity, float) -> bool
type Position = vec3

core EntityId { int value }
core PlayerId { int value }

EntityId e = { value: 5 }
PlayerId p = { value: 5 }
// e == p  // Compile error, different types


entity = entity.pop()              // Remove outermost shell
entity = entity.pop(Frozen)        // Remove outermost Frozen specifically
entity = entity.remove(Frozen)     // Remove ALL instances of Frozen
entity = entity.remove_at(2)       // Remove shell at stack index 2 (advanced)

