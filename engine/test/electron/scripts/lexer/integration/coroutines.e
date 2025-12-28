// Coroutine integration test
// Tests all coroutine-related lexical features

// =============================================================================
// BASIC COROUTINES
// =============================================================================

def simple_coro() -> coro[int] {
    yield 1;
    yield 2;
    yield 3;
    return 4;
}

def string_coro() -> coro[string] {
    yield "first";
    yield "second";
    return "done";
}

def void_coro() -> coro[void] {
    print("step 1");
    yield;
    print("step 2");
    yield;
    print("step 3");
}

// =============================================================================
// COROUTINES WITH PARAMETERS
// =============================================================================

def countdown(int n) -> coro[int] {
    for int i in n..=0 {
        yield i;
    }
    return 0;
}

def repeat(string msg, int times) -> coro[string] {
    for int i in 0..times {
        yield msg;
    }
    return "";
}

def range_generator(int start, int end, int step) -> coro[int] {
    int current = start;
    while current < end {
        yield current;
        current += step;
    }
    return current;
}

// =============================================================================
// ASYNC-STYLE COROUTINES
// =============================================================================

def load_texture(string path) -> coro[Texture] {
    print("Loading texture: {path}");
    yield wait(0.1);

    Texture tex = Texture.load(path);
    if tex == null {
        yield wait(0.05);
        tex = Texture.load(path); // Retry
    }

    return tex;
}

def load_resources() -> coro[bool] {
    // Sequential loading
    Texture tex1 = yield load_texture("player.png");
    Texture tex2 = yield load_texture("enemy.png");
    Texture tex3 = yield load_texture("background.png");

    if tex1 == null || tex2 == null || tex3 == null {
        return false;
    }

    return true;
}

// =============================================================================
// SPAWNING COROUTINES
// =============================================================================

def test_spawn() {
    // Basic spawn
    coro[int] c1 = spawn simple_coro();

    // Spawn with args
    coro[int] c2 = spawn countdown(10);
    coro[string] c3 = spawn repeat("hello", 5);

    // Wait for result
    int result = yield c1;
    print("Result: {result}");

    // Spawn and forget (let it run)
    spawn void_coro();

    // Spawn multiple
    list[coro[int]] coros = [];
    for int i in 0..10 {
        coros.push(spawn countdown(i));
    }

    // Wait for all
    for c in coros {
        int val = yield c;
        print("Got: {val}");
    }
}

// =============================================================================
// YIELD REQUEST
// =============================================================================

def interruptible_task() -> coro[int] {
    for int i in 0..100 {
        // Check if caller wants us to yield
        if yield_request {
            yield i;
        }

        // Do work
        process_item(i);
    }

    return 100;
}

def cooperative_task() -> coro[void] {
    while true {
        // Yield control periodically
        yield;

        // Do some work
        do_work();

        // Check for cancellation
        if should_stop {
            return;
        }
    }
}

// =============================================================================
// COROUTINE WITH ERROR HANDLING
// =============================================================================

def fallible_coro() -> coro[result[int, string]] {
    yield success(1);

    if random() < 0.5 {
        yield error("Random failure");
        return error("Final failure");
    }

    yield success(2);
    return success(3);
}

def safe_load() -> coro[result[Data, string]] {
    yield wait(0.1);

    Data data = try_load()?;
    if data == null {
        return error("Load failed");
    }

    yield wait(0.05);

    validate(data)?;

    return success(data);
}

// =============================================================================
// COROUTINE COMBINATORS
// =============================================================================

def race(coro[T] a, coro[T] b) -> coro[T] {
    // Return whichever finishes first
    while true {
        if a.is_done() {
            return yield a;
        }
        if b.is_done() {
            return yield b;
        }
        yield;
    }
}

def all(list[coro[T]] coros) -> coro[list[T]] {
    list[T] results = [];

    for c in coros {
        T result = yield c;
        results.push(result);
    }

    return results;
}

def timeout(coro[T] c, float seconds) -> coro[result[T, string]] {
    float elapsed = 0.0;
    float dt = 0.016; // Assume 60fps

    while elapsed < seconds {
        if c.is_done() {
            return success(yield c);
        }
        elapsed += dt;
        yield wait(dt);
    }

    c.cancel();
    return error("Timeout");
}

// =============================================================================
// COROUTINE STATE MACHINE
// =============================================================================

enum TaskState {
    Pending,
    Running,
    Paused,
    Completed(int),
    Failed(string)
}

def state_machine_coro() -> coro[TaskState] {
    yield TaskState.Pending;
    yield TaskState.Running;

    for int i in 0..10 {
        yield TaskState.Running;
        if should_pause {
            yield TaskState.Paused;
        }
    }

    if success_condition {
        return TaskState.Completed(42);
    } else {
        return TaskState.Failed("Task failed");
    }
}

// =============================================================================
// NESTED COROUTINES
// =============================================================================

def inner_coro(int depth) -> coro[int] {
    if depth <= 0 {
        return 0;
    }

    yield depth;
    int result = yield spawn inner_coro(depth - 1);
    return result + depth;
}

def outer_coro() -> coro[int] {
    coro[int] c = spawn inner_coro(5);
    int sum = 0;

    while not c.is_done() {
        int val = yield c;
        sum += val;
    }

    return sum;
}

// =============================================================================
// COROUTINE ITERATION
// =============================================================================

def fibonacci() -> coro[int] {
    int a = 0;
    int b = 1;

    yield a;
    yield b;

    loop {
        int next = a + b;
        a = b;
        b = next;
        yield next;
    }
}

def take(coro[T] c, int n) -> list[T] {
    list[T] results = [];

    for int i in 0..n {
        if c.is_done() {
            break;
        }
        results.push(yield c);
    }

    return results;
}

def test_iteration() {
    coro[int] fib = spawn fibonacci();

    // Take first 10 fibonacci numbers
    list[int] fibs = take(fib, 10);

    for int n in fibs {
        print("{n}");
    }
}
