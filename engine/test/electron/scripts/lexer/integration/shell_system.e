// Shell system integration test
// Tests all shell-related lexical features

// =============================================================================
// SHELL DEFINITIONS
// =============================================================================

shell Movable {
    requires {
        vec3 position;
        float speed;
    }

    def move(vec3 direction) {
        core.position += direction.normalize() * core.speed;
    }

    def teleport(vec3 target) {
        core.position = target;
    }

    def get_position() -> vec3 {
        return core.position;
    }
}

shell Rotatable {
    requires {
        vec3 rotation;
        float rotation_speed;
    }

    def rotate(vec3 axis, float angle) {
        core.rotation += axis * angle * core.rotation_speed;
    }

    def look_at(vec3 target) {
        vec3 dir = target - inner.position;
        core.rotation = dir.to_euler();
    }
}

shell Scalable {
    requires {
        vec3 scale;
    }

    def set_scale(vec3 new_scale) {
        core.scale = new_scale;
    }

    def uniform_scale(float factor) {
        core.scale *= factor;
    }
}

shell Transform3D {
    requires {
        vec3 position;
        vec3 rotation;
        vec3 scale;
    }

    // Combines Movable, Rotatable, Scalable
    def set_transform(vec3 pos, vec3 rot, vec3 scl) {
        core.position = pos;
        core.rotation = rot;
        core.scale = scl;
    }

    def get_matrix() -> mat4 {
        return mat4.trs(core.position, core.rotation, core.scale);
    }
}

// =============================================================================
// SEALED AND UNIQUE SHELLS
// =============================================================================

sealed shell Singleton {
    requires {
        int id;
    }

    // Only one instance allowed per core
}

unique shell OnePerType {
    requires {
        string type_name;
    }

    // Unique shell, replaces existing
}

strict shell MustHaveAll {
    requires {
        int a;
        int b;
        int c;
    }

    // All requirements must be satisfied at attach time
}

// =============================================================================
// SHELL WITH INNER/OUTER/SIBLING ACCESS
// =============================================================================

shell HealthBar {
    requires {
        int health;
        int max_health;
    }

    def render() {
        float percent = core.health as float / core.max_health as float;
        draw_bar(inner.position, percent);
    }
}

shell DamageDisplay {
    requires {
        int last_damage;
    }

    def show_damage() {
        vec3 pos = inner.position + vec3(0, 2, 0);
        draw_text(pos, "-{core.last_damage}", Color.red());
    }
}

shell HealthUI {
    requires {
        int health;
    }

    def update() {
        // Access sibling shells
        if sibling HealthBar {
            sibling::HealthBar.render();
        }

        // Access all siblings of a type
        for dmg in sibling_all DamageDisplay {
            dmg.show_damage();
        }

        // Access outer shell
        if outer Transform3D {
            vec3 pos = outer::Transform3D.get_position();
            draw_health_at(pos);
        }
    }
}

// =============================================================================
// SHELL ATTACHMENT OPERATIONS
// =============================================================================

core TestEntity {
    public int id;
    public string name;
    public vec3 position;
    public vec3 rotation;
    public vec3 scale;
    public float speed;
    public float rotation_speed;
    public int health;
    public int max_health;
}

def test_shell_attachment() {
    TestEntity e = TestEntity.new();

    // Basic push
    e <- Movable;

    // Chain push
    e <- Movable <- Rotatable <- Scalable;

    // Force push (replace existing)
    e <<- Transform3D;

    // Remove single instance
    e -> Movable;

    // Remove all instances
    e ->> Scalable;

    // Query existence
    if e >- Movable {
        print("Has Movable");
    }

    if not (e >- Rotatable) {
        print("Missing Rotatable");
    }

    // Conditional push
    if should_be_movable {
        e <- Movable;
    }

    // Push in loop
    for shell_type in required_shells {
        e <- shell_type;
    }
}

// =============================================================================
// SHELL METHOD CALLS
// =============================================================================

def test_shell_methods() {
    TestEntity e = TestEntity.new();
    e <- Movable <- Rotatable <- Scalable;

    // Direct method call
    e::Movable.move(vec3(1, 0, 0));

    // Method call on query
    if e >- Rotatable {
        e::Rotatable.rotate(vec3(0, 1, 0), 45.0);
    }

    // Chained method calls
    vec3 pos = e::Movable.get_position();

    // Method call with complex args
    e::Transform3D.set_transform(
        vec3(0, 0, 0),
        vec3(0, 0, 0),
        vec3(1, 1, 1)
    );

    // Safe method call
    e?.Movable?.move(vec3(0, 1, 0));

    // Method call with result
    result[vec3, string] r = e::Movable.try_move(vec3(1, 0, 0));
    match r {
        success(new_pos) => print("Moved to {new_pos}"),
        error(msg) => print("Move failed: {msg}")
    }
}

// =============================================================================
// SHELL LIFECYCLE
// =============================================================================

shell WithLifecycle {
    requires {
        bool initialized;
    }

    def on_attach() {
        print("Shell attached");
        core.initialized = true;
    }

    def on_detach() {
        print("Shell detaching");
        core.initialized = false;
    }

    on_cancel {
        print("Shell attachment cancelled");
        cleanup();
    }
}

// =============================================================================
// COMPLEX SHELL PATTERNS
// =============================================================================

shell Buffable {
    requires {
        list[string] active_buffs;
    }

    def add_buff(string name) {
        core.active_buffs.push(name);

        // Add corresponding shell
        match name {
            "speed" => core <- SpeedBuff,
            "strength" => core <- StrengthBuff,
            "shield" => core <- ShieldBuff,
            _ => { }
        }
    }

    def remove_buff(string name) {
        int idx = core.active_buffs.find(name);
        if idx >= 0 {
            core.active_buffs.remove(idx);

            match name {
                "speed" => core -> SpeedBuff,
                "strength" => core -> StrengthBuff,
                "shield" => core -> ShieldBuff,
                _ => { }
            }
        }
    }
}

shell SpeedBuff {
    requires {
        float speed;
    }

    const MULTIPLIER = 1.5;

    def get_modified_speed() -> float {
        return core.speed * MULTIPLIER;
    }
}

shell StrengthBuff {
    requires {
        int strength;
    }

    const BONUS = 10;

    def get_modified_strength() -> int {
        return core.strength + BONUS;
    }
}

shell ShieldBuff {
    requires {
        int health;
    }

    int shield_amount = 50;

    def absorb_damage(int amount) -> int {
        int absorbed = amount.min(shield_amount);
        shield_amount -= absorbed;

        if shield_amount <= 0 {
            core -> ShieldBuff;
        }

        return amount - absorbed;
    }
}
