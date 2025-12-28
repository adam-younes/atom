// Pattern matching integration test
// Tests all pattern matching lexical features

// =============================================================================
// BASIC ENUMS FOR MATCHING
// =============================================================================

enum Option[T] {
    Some(T),
    None
}

enum Result[T, E] {
    Ok(T),
    Err(E)
}

enum State {
    Idle,
    Moving(vec3),
    Jumping(float),
    Attacking(int, float),
    Stunned(float, string),
    Dead
}

enum Expression {
    Number(int),
    String(string),
    Binary(Box[Expression], string, Box[Expression]),
    Unary(string, Box[Expression]),
    Call(string, list[Expression]),
    Variable(string)
}

// =============================================================================
// SIMPLE MATCH EXPRESSIONS
// =============================================================================

def match_bool(bool b) -> string {
    return match b {
        true => "yes",
        false => "no"
    };
}

def match_int(int n) -> string {
    return match n {
        0 => "zero",
        1 => "one",
        2 => "two",
        _ => "many"
    };
}

def match_string(string s) -> int {
    return match s {
        "hello" => 1,
        "world" => 2,
        "foo" => 3,
        "bar" => 4,
        _ => 0
    };
}

// =============================================================================
// ENUM MATCHING
// =============================================================================

def match_option(Option[int] opt) -> int {
    return match opt {
        Option.Some(value) => value,
        Option.None => 0
    };
}

def match_result(Result[int, string] res) -> int {
    return match res {
        Result.Ok(value) => value,
        Result.Err(msg) => {
            print("Error: {msg}");
            return -1;
        }
    };
}

def match_state(State state) -> string {
    return match state {
        State.Idle => "Standing still",
        State.Moving(dir) => "Moving towards {dir}",
        State.Jumping(height) => "Jumping {height} units",
        State.Attacking(damage, cooldown) => "Attacking for {damage} damage",
        State.Stunned(duration, reason) => "Stunned for {duration}s: {reason}",
        State.Dead => "Dead"
    };
}

// =============================================================================
// NESTED PATTERN MATCHING
// =============================================================================

def match_nested(Result[Option[int], string] res) -> int {
    return match res {
        Result.Ok(Option.Some(value)) => value,
        Result.Ok(Option.None) => 0,
        Result.Err(_) => -1
    };
}

def match_expression(Expression expr) -> int {
    return match expr {
        Expression.Number(n) => n,

        Expression.Binary(left, "+", right) =>
            match_expression(*left) + match_expression(*right),

        Expression.Binary(left, "-", right) =>
            match_expression(*left) - match_expression(*right),

        Expression.Binary(left, "*", right) =>
            match_expression(*left) * match_expression(*right),

        Expression.Unary("-", inner) =>
            -match_expression(*inner),

        _ => 0
    };
}

// =============================================================================
// GUARDS IN PATTERNS
// =============================================================================

def classify_number(int n) -> string {
    return match n {
        x if x < 0 => "negative",
        0 => "zero",
        x if x > 0 && x < 10 => "small positive",
        x if x >= 10 && x < 100 => "medium positive",
        x if x >= 100 => "large positive",
        _ => "unknown"
    };
}

def match_with_guard(State state) -> bool {
    return match state {
        State.Moving(dir) if dir.length() > 10.0 => true,
        State.Attacking(damage, _) if damage > 50 => true,
        State.Stunned(duration, _) if duration > 5.0 => true,
        _ => false
    };
}

def match_complex_guard(Option[int] opt, int threshold) -> string {
    return match opt {
        Option.Some(v) if v > threshold && v % 2 == 0 => "large even",
        Option.Some(v) if v > threshold => "large odd",
        Option.Some(v) if v <= threshold && v > 0 => "small positive",
        Option.Some(0) => "zero",
        Option.Some(v) if v < 0 => "negative",
        Option.None => "nothing"
    };
}

// =============================================================================
// OR PATTERNS
// =============================================================================

def match_or_pattern(int n) -> string {
    return match n {
        0 | 1 => "zero or one",
        2 | 3 | 5 | 7 => "small prime",
        4 | 6 | 8 | 9 => "small composite",
        _ => "larger"
    };
}

def match_state_or(State state) -> bool {
    return match state {
        State.Idle | State.Dead => false,
        State.Moving(_) | State.Jumping(_) | State.Attacking(_, _) => true,
        _ => false
    };
}

// =============================================================================
// BINDING PATTERNS
// =============================================================================

def match_with_binding(tuple[int, int, int] point) -> string {
    return match point {
        (0, 0, 0) => "origin",
        (x, 0, 0) => "on x-axis at {x}",
        (0, y, 0) => "on y-axis at {y}",
        (0, 0, z) => "on z-axis at {z}",
        (x, y, 0) => "on xy-plane at ({x}, {y})",
        (x, 0, z) => "on xz-plane at ({x}, {z})",
        (0, y, z) => "on yz-plane at ({y}, {z})",
        (x, y, z) => "in space at ({x}, {y}, {z})"
    };
}

def match_rest_pattern(list[int] items) -> string {
    return match items {
        [] => "empty",
        [single] => "one: {single}",
        [first, second] => "two: {first}, {second}",
        [first, second, ...rest] => "many: {first}, {second}, and {rest.len} more"
    };
}

// =============================================================================
// MATCH AS EXPRESSION
// =============================================================================

def get_multiplier(State state) -> float {
    float mult = match state {
        State.Idle => 1.0,
        State.Moving(_) => 1.2,
        State.Jumping(_) => 0.8,
        State.Attacking(_, _) => 1.5,
        State.Stunned(_, _) => 0.0,
        State.Dead => 0.0
    };
    return mult;
}

def inline_match(Option[int] opt) -> int {
    return (match opt { Option.Some(v) => v, Option.None => 0 }) * 2;
}

// =============================================================================
// MATCH IN CONTROL FLOW
// =============================================================================

def process_items(list[Option[int]] items) {
    for item in items {
        match item {
            Option.Some(v) if v > 0 => {
                process_positive(v);
            },
            Option.Some(v) if v < 0 => {
                process_negative(v);
            },
            Option.Some(0) => {
                // Skip zero
                continue;
            },
            Option.None => {
                // Stop on none
                break;
            }
        }
    }
}

def match_with_return(Result[int, string] res) -> int {
    match res {
        Result.Ok(v) => return v,
        Result.Err(msg) => {
            log_error(msg);
            return -1;
        }
    }
}

// =============================================================================
// EXHAUSTIVENESS
// =============================================================================

def exhaustive_bool(bool b) -> int {
    // Must cover both cases
    return match b {
        true => 1,
        false => 0
    };
}

def exhaustive_option(Option[int] opt) -> int {
    // Must cover Some and None
    return match opt {
        Option.Some(v) => v,
        Option.None => 0
    };
}

def non_exhaustive_with_wildcard(State state) -> string {
    // Wildcard covers remaining cases
    return match state {
        State.Idle => "idle",
        State.Dead => "dead",
        _ => "active"
    };
}

// =============================================================================
// IS OPERATOR
// =============================================================================

def check_state(State state) {
    if state is State.Idle {
        print("idle");
    }

    if state is State.Moving(dir) {
        print("moving: {dir}");
    }

    if state is State.Attacking(damage, _) {
        print("damage: {damage}");
    }

    if not (state is State.Dead) {
        print("alive");
    }
}

def is_in_condition(Option[int] opt) -> bool {
    return opt is Option.Some(v) && v > 0;
}

// =============================================================================
// TYPE MATCHING
// =============================================================================

def match_type(any value) -> string {
    return match value {
        v: int => "integer: {v}",
        v: float => "float: {v}",
        v: string => "string: {v}",
        v: bool => "bool: {v}",
        _ => "unknown"
    };
}
