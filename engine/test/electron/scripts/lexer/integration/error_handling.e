// Error handling integration test
// Tests all error handling lexical features

// =============================================================================
// RESULT TYPES
// =============================================================================

def parse_int(string s) -> result[int, string] {
    if s.is_empty() {
        return error("Empty string");
    }

    for char c in s {
        if not c.is_digit() {
            return error("Invalid character: {c}");
        }
    }

    int value = internal_parse(s);
    return success(value);
}

def divide(int a, int b) -> result[int, string] {
    if b == 0 {
        return error("Division by zero");
    }
    return success(a / b);
}

def safe_sqrt(float x) -> result[float, string] {
    if x < 0 {
        return error("Cannot take square root of negative number");
    }
    return success(math.sqrt(x));
}

// =============================================================================
// ERROR PROPAGATION
// =============================================================================

def calculate(string a, string b) -> result[int, string] {
    int x = parse_int(a)?;
    int y = parse_int(b)?;
    int result = divide(x, y)?;
    return success(result);
}

def complex_calculation(string input) -> result[float, string] {
    list[string] parts = input.split(",");

    if parts.len != 2 {
        return error("Expected two values");
    }

    int a = parse_int(parts[0])?;
    int b = parse_int(parts[1])?;

    float sum = (a + b) as float;
    float root = safe_sqrt(sum)?;

    return success(root);
}

def chain_propagation() -> result[int, string] {
    // Each ? propagates errors up
    int a = get_value_a()?;
    int b = get_value_b()?;
    int c = compute(a, b)?;
    int d = validate(c)?;
    return success(d);
}

// =============================================================================
// MATCHING ON RESULTS
// =============================================================================

def handle_result(result[int, string] res) {
    match res {
        success(value) => {
            print("Got value: {value}");
            process(value);
        },
        error(msg) => {
            print("Error: {msg}");
            log_error(msg);
        }
    }
}

def transform_result(result[int, string] res) -> result[float, string] {
    return match res {
        success(v) => success(v as float * 2.0),
        error(e) => error(e)
    };
}

def combine_results(result[int, string] a, result[int, string] b) -> result[int, string] {
    match (a, b) {
        (success(x), success(y)) => success(x + y),
        (error(e), _) => error(e),
        (_, error(e)) => error(e)
    }
}

// =============================================================================
// NULL HANDLING
// =============================================================================

def find_user(int id) -> User? {
    if id < 0 {
        return null;
    }
    return database.lookup(id);
}

def get_user_name(int id) -> string? {
    User? user = find_user(id);

    if user == null {
        return null;
    }

    return user.name;
}

def safe_access_chain() {
    // Safe member access
    string? name = user?.profile?.display_name;

    // With method calls
    int? length = text?.trim()?.len;

    // Chained safe access
    string? value = config?.section("main")?.get("key");
}

// =============================================================================
// NULL COALESCING
// =============================================================================

def with_default(string? value) -> string {
    return value ?? "default";
}

def coalesce_chain(string? a, string? b, string? c) -> string {
    return a ?? b ?? c ?? "fallback";
}

def coalesce_with_function() -> int {
    return cached_value ?? compute_value() ?? 0;
}

def coalesce_in_expression() {
    int x = maybe_value ?? 0;
    string s = optional_string ?? "";

    // In function call
    process(value ?? default_value);

    // In assignment
    result = first_choice ?? second_choice ?? third_choice;
}

// =============================================================================
// ALLOWS KEYWORD
// =============================================================================

def function_allows_null() allows null {
    if random() < 0.5 {
        return null;
    }
    return some_value();
}

def function_allows_error() allows Error {
    if bad_condition {
        throw Error("Something went wrong");
    }
    return compute();
}

// =============================================================================
// COMBINED ERROR HANDLING PATTERNS
// =============================================================================

def load_and_process(string path) -> result[ProcessedData, string] {
    // Try to load file
    File? file = File.open(path);
    if file == null {
        return error("Could not open file: {path}");
    }
    defer file.close();

    // Parse content
    string content = file.read_all() ?? "";
    if content.is_empty() {
        return error("File is empty");
    }

    // Parse JSON
    result[Json, string] json_result = Json.parse(content);
    Json data = match json_result {
        success(d) => d,
        error(e) => return error("Parse error: {e}")
    };

    // Validate
    if not validate_schema(data) {
        return error("Invalid data schema");
    }

    // Process
    ProcessedData processed = process_json(data)?;

    return success(processed);
}

def retry_on_error(int max_attempts) -> result[Data, string] {
    string? last_error = null;

    for int attempt in 0..max_attempts {
        match try_operation() {
            success(data) => return success(data),
            error(e) => {
                last_error = e;
                print("Attempt {attempt + 1} failed: {e}");
                yield wait(1.0 * attempt); // Exponential backoff
            }
        }
    }

    return error(last_error ?? "Unknown error after {max_attempts} attempts");
}

def fallback_chain() -> result[Config, string] {
    // Try multiple sources
    Config? config = load_from_env();
    if config != null {
        return success(config);
    }

    config = load_from_file("config.json")?;
    if config != null {
        return success(config);
    }

    config = load_from_remote()?;
    if config != null {
        return success(config);
    }

    return error("No configuration source available");
}

// =============================================================================
// ERROR CONTEXT
// =============================================================================

def with_context() -> result[Data, ErrorWithContext] {
    result[RawData, string] raw = fetch_data();

    match raw {
        success(d) => {
            return process(d);
        },
        error(msg) => {
            return error(ErrorWithContext {
                message: msg,
                source: "fetch_data",
                timestamp: now(),
                context: get_current_context()
            });
        }
    }
}

def map_error(result[T, E1] res, func[(E1) -> E2] mapper) -> result[T, E2] {
    return match res {
        success(v) => success(v),
        error(e) => error(mapper(e))
    };
}

def and_then(result[T, E] res, func[(T) -> result[U, E]] f) -> result[U, E] {
    return match res {
        success(v) => f(v),
        error(e) => error(e)
    };
}
