Electron Language Specification Analysis

  Critical Issues

  1. Core Literal Syntax Inconsistency (Section 7.3 vs Grammar)

  Spec text (line 1156-1168):
  Player p = (Player) {
      health: 100,
      position: vec3(0, 0, 0)
  };

  Grammar (line 2488-2489):
  coreLiteral     → IDENTIFIER "{" (fieldInit ("," fieldInit)*)? "}"

  Problem: The grammar says IDENTIFIER "{" ... but the examples use cast-style syntax (Player) { ... }. These are incompatible. The grammar would parse Player { ... } not (Player) { ... }.


  ---
  2. String Type Semantics Contradiction (Section 2.8 vs 2.11)

  Section 2.8 (line 355-362): States strings are "reference types" that can be nullable.

  Section 2.11 (line 458): States strings are "Immutable Reference" - passed by reference but cannot be modified.

  Problem: If strings are immutable references, the default value "" (line 2574) implies every uninitialized string shares a reference to the same empty string. This is fine, but the spec doesn't clarify:
  - Are string comparisons reference equality or value equality?
  - Does string concatenation create new interned strings or fresh allocations?
  - What happens with interpolation - is the result interned?

  ---
  3. Tuple Type Indexing Ambiguity (Section 2.4)

  Spec (line 260-262):
  int x = point[0];    // 10
  int y = point[1];    // 20

  Problem: The index must be a compile-time constant for type safety (since elements have different types), but this is never stated. What happens with:
  int i = get_index();
  ??? x = point[i];  // What type is x?

  ---
  4. Range Type Size Mismatch (Section 2.3 vs Appendix C)

  Section 2.3 (line 202): States range is 9 bytes (2 × int + bool = 4 + 4 + 1).

  Appendix C (line 2557): Confirms 9 bytes.

  Problem: Without padding, this creates alignment issues. Most platforms would require 12 bytes (4-byte alignment). The spec should clarify memory layout.

  ---
  5. struct Keyword Reserved Conflict (Section 1.3 vs 7.6)

  Section 1.3 (line 71): struct is listed as a reserved keyword.

  Section 7.6 (line 1207-1228): Structs are fully defined with struct keyword.

  Status: This is consistent, but the spec never clarifies why structs exist separately from cores given their similarity. The distinction (value vs reference, no shells) should be in Section 7 near the top.

  ---
  6. Shell requires Type-Checking Gap (Section 8.2)

  Spec (line 1347-1360):
  shell Movable {
      requires { vec3 position, vec3 velocity }
  }

  Problem: The spec never defines when this check occurs:
  - Compile-time? Then you need whole-program analysis or explicit core annotations.
  - Runtime? Then player <- Movable can panic, but this isn't listed in Section 10.5's panic conditions.

  The shell requirements check is listed under "Shell requirements mismatch → Error at attachment time" (line 1849), but "Error" vs "Panic" is ambiguous.

  ---
  7. inner Call Return Values Undefined (Section 8.6)

  Spec (line 1500-1526): Shows inner.method() delegation but never addresses return values.

  Problem: What happens here?
  shell Armored {
      def get_defense() -> int {
          return inner.get_defense() + 10;  // What if no handler below?
      }
  }

  If inner.method() returns void implicitly when no handler exists, what does inner.get_defense() return? 0? null? Panic?

  ---
  8. Coroutine Return Value Semantics Incomplete (Section 11.7)

  Spec (line 1962-1964):
  coro[list[vec3]] path_coro = spawn compute_path(a, b);
  list[vec3] path = yield path_coro;  // Wait and get result

  Problem: What happens if you:
  1. yield on a cancelled coroutine?
  2. yield on an already-completed coroutine?
  3. Try to get the result value multiple times?

  ---
  9. Error Propagation Type Inference Gap (Section 10.2)

  Spec (line 1757): success(5) infers result[int, ?] where ? is determined by context.

  Problem: What if context doesn't provide the error type?
  func f = () {
      return success(5);  // What's the error type?
  };

  The spec doesn't address this edge case.

  ---
  10. Grammar Missing varDecl Statement Rule

  Grammar (line 2447):
  statement       → exprStmt | varDecl | assignment | ...

  Problem: varDecl is referenced but never defined in the grammar. The spec shows examples:
  int x = 5;
  But the grammar production for this is missing.

  ---
  Major Issues

  11. Shell Composition and inner Interaction Unclear (Section 8.7)

  When using shell composition:
  shell Character: Movable, Damageable { }

  Problem: If Character is on a core and you call inner.move(), does it:
  - Call the next shell in the stack (outside Character)?
  - Call Movable's implementation (composed inside Character)?
  - Something else?

  The delegation semantics with composition are undefined.

  ---
  12. Entity Core Attachment vs Shell Attachment Confusion (Section 13.3)

  Spec (line 2149-2151):
  player.add((PlayerData) { hp: 100, ... });
  player.add((Transform) { ... });

  Later (line 2174-2176):
  PlayerData data = player![PlayerData];
  data <- Controllable;

  Problem: Shells attach to cores, but the spec never explains:
  - Can the same shell attach to multiple cores on the same entity?
  - What happens if Controllable requires fields from both PlayerData and Transform?
  - Can a shell "span" multiple cores?

  ---
  13. @change Decorator Field Reference Ambiguity (Section 13.2)

  Spec (line 2100-2108):
  shell HealthUI {
      @change(hp)
      def on_hp_changed(int old_value, int new_value) { }
  }

  Problem: hp refers to core.hp, but what if:
  - The shell has its own hp field?
  - Multiple cores with hp are attached?
  - The field is nested (@change(player.hp))?

  ---
  14. List Destructuring Rest Pattern Type (Section 5.5)

  Spec (line 873-877):
  match items {
      [first, ...rest] => "first: {first}, remaining: {rest.length}",
  }

  Problem: What is the type of rest? Presumably list[T] where T is the element type, but this is never stated. What about:
  [first, second, ...rest, last]  // Is this allowed?

  ---
  15. Dictionary Key Hashability Under-specified (Section 2.6)

  Spec (line 315-316): "Keys must be hashable: primitives, strings, or enums without data."

  Problem: What about:
  - Tuples of hashable types?
  - Structs with all hashable fields?
  - Cores with readonly fields only?
  - Type aliases to hashable types?

  ---
  16. Function Overloading Not Addressed

  The spec never mentions function overloading. Given:
  def foo(int x) -> int { return x; }
  def foo(float x) -> float { return x; }

  Is this allowed? Forbidden? The spec is silent.

  ---
  17. @comptime Evaluation Scope (Section 3.2)

  Spec (line 516):
  @comptime const TAU = PI * 2;  // OK: compile-time arithmetic

  Problem: What operations are valid at comptime?
  - String methods ("foo".length)?
  - Function calls to @comptime functions?
  - Conditional expressions?

  ---
  18. Generic Type Parameters Missing

  The spec mentions result[T, E] and coro[T] as built-in generics, but there's no way to define user generics. Given the list[T] syntax exists, users might expect:
  core Container[T] {
      public T value;
  }

  But this isn't addressed as either allowed or forbidden.

  ---
  19. Operator Precedence for ? Missing (Section 4.8 / Appendix B)

  The ? operator for error propagation is listed in Section 4.8 (line 654) but not in the precedence table (Appendix B). Where does it bind?

  a + b?  // Is this (a + b)? or a + (b?)

  ---
  20. Swizzle Assignment Left-to-Right Semantics (Section 2.2)

  Spec (line 191-193):
  v.xy = vec2(5.0, 6.0);  // OK
  v.xx = vec2(1.0, 2.0);  // Error: aliasing pattern

  Problem: What about:
  v.xz = v.zx;  // Swap x and z - is this aliasing?

  This reads from and writes to overlapping components.

  ---
  Minor Issues

  21. Octal Literals Not Mentioned (Section 1.4)

  Hex (0xFF) and binary (0b1010) are specified, but octal (0o777) is not mentioned. Is it supported?

  ---
  22. Empty Tuple Prohibition (Section 2.4)

  Spec (line 291): "Tuples must have at least two elements."

  Problem: One-element tuples like (5,) are useful in some languages. This should explicitly show what happens syntactically if you try (5) - it's just a grouped expression, not a tuple.

  ---
  23. Struct Field Visibility (Section 7.6)

  All struct examples show public fields. Can structs have private fields? If so, who can access them given structs can't have shells?

  ---
  24. Collection Literal Type Inference (Section 2.6)

  list[int] numbers = [1, 2, 3];

  What about:
  list numbers = [1, 2, 3];  // Can type be inferred?

  ---
  25. defer and return Value Interaction (Section 5.4)

  Spec (line 799): Shows defer with early return, but what about:
  def foo() -> int {
      defer { return 5; }  // Can defer contain return?
      return 10;
  }

  ---
  26. Shell State Initialization Timing (Section 8.3)

  shell Cooldown {
      float time_remaining = 0.0;
  }

  When exactly is time_remaining initialized? At shell push time? At first method call?

  ---
  27. yield in Non-Coroutine Functions (Section 11)

  What happens if you use yield in a regular function?
  def not_a_coro() {
      yield wait(1.0);  // Compile error? Runtime error?
  }

  ---
  28. Module Namespace Conflicts (Section 12)

  import "utils/math" as m
  from "utils/helpers" import clamp

  What if clamp is defined in both modules and you import both?

  ---
  29. readonly in Structs (Section 7.6)

  The readonly keyword is shown for cores but never mentioned for structs. Is it allowed?

  ---
  30. Match Exhaustiveness for Enums with Data (Section 5.5)

  enum State { Idle, Attacking(int) }

  match state {
      State.Idle => ...,
      State.Attacking(0) => ...,
      // Is this exhaustive? Attacking can have any int
  }

  ---
  Grammar Errors

  31. Missing assignment Production

  Line 2447 references assignment but it's never defined.

  32. Missing yieldStmt Production

  Line 2449 references yieldStmt but it's never defined.

  33. Missing matchStmt Production

  Line 2449 references matchStmt but it's never defined.

  34. Missing returnStmt Production

  Line 2449 references returnStmt but it's never defined.

  35. innerCall in Wrong Location

  Line 2477:
  innerCall       → "inner" ("!" | "?")? "." IDENTIFIER "(" argList? ")"

  This is listed under postfix alternatives, but inner isn't a postfix - it's a keyword that starts an expression. This should be under primary.

  ---
  Semantic Gaps

  36. Default Parameter Evaluation Order

  def foo(int a = bar(), int b = baz()) { }

  Are defaults evaluated left-to-right? Once per call? Once globally?

  ---
  37. Core Field Initialization Order

  core Foo {
      int a = bar();
      int b = a + 1;  // Can b reference a?
  }

  ---
  38. Short-Circuit in Pattern Guards

  match x {
      n if expensive() && n > 0 => ...
  }

  Is n > 0 evaluated if expensive() returns false?

  ---
  39. null in Concatenation

  string? s = null;
  string msg = "Hello " + s;  // Compile error? Runtime null coalesce?

  ---
  40. Entity Lifetime and Core References

  entity e = entity.spawn();
  PlayerData data = e![PlayerData];
  entity.destroy(e);
  // Is data now a dangling reference?

  ---
  Missing Features Implied but Not Specified

  41. No Integer Sizes Beyond 32-bit

  Only int (32-bit) exists. No int64, int16, int8, uint* variants. This limits numeric range significantly.

  42. No Character Type

  string exists but no char. How do you work with individual characters?

  43. No Bitfield/Flags Support

  No built-in way to define flag enums or bitfields despite having bitwise operators.

  44. No Variadic Functions

  def printf(string fmt, ...) { }  // Not supported

  45. No Attributes/Annotations Beyond Decorators

  Decorators are limited to a fixed set. No way to define custom metadata.

  ---
  Potential Implementation Issues

  46. Shell Dispatch Cache Invalidation (Appendix E)

  The cache invalidation strategy shown uses a version counter. With 32-bit counter and frequent shell changes, wraparound could cause stale cache hits after ~4 billion changes.

  47. Hot Reload Coroutine Restart

  Spec (line 2210-2212): "If structure changed: restart from beginning with warning logged."

  This could cause severe gameplay issues if a coroutine was mid-animation or mid-transition.

  48. Linear Component Lookup

  The research document notes O(n) component lookup. With the shell dispatch system adding more lookups, this could become a bottleneck.

  ---
  Documentation Gaps

  49. No Memory Model Specification

  No discussion of:
  - Garbage collection vs manual memory
  - Reference counting
  - Ownership semantics

  50. No Thread Safety Discussion

  Given this is for game engines, no mention of:
  - Thread-safe collection variants
  - Atomic operations
  - Mutex/synchronization primitives

  ---

● Summary

  I identified 50 issues in the Electron specification:

  | Category              | Count |
  |-----------------------|-------|
  | Critical Issues       | 10    |
  | Major Issues          | 10    |
  | Minor Issues          | 10    |
  | Grammar Errors        | 5     |
  | Semantic Gaps         | 5     |
  | Missing Features      | 5     |
  | Implementation Issues | 3     |
  | Documentation Gaps    | 2     |

  Most Critical to Address:

  1. Core literal syntax mismatch between examples and grammar (Section 7.3 vs Appendix A)
  2. Shell requires checking semantics - compile-time vs runtime is undefined
  3. inner call return values when no handler exists - completely undefined
  4. Grammar is incomplete - missing productions for varDecl, assignment, yieldStmt, matchStmt, returnStmt
  5. Operator precedence for ? is missing from the table
  6. Tuple indexing must use compile-time constants but this isn't stated
  7. Entity/core reference lifetime after entity.destroy() - dangling reference semantics undefined

  The specification is well-structured and covers a lot of ground, but the shell delegation system and entity-core-shell relationship need significant clarification before implementation can proceed safely.
