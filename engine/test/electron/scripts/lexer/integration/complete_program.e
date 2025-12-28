// Complete program integration test
// This file contains a comprehensive program using all language features

// =============================================================================
// IMPORTS
// =============================================================================

import math;
import physics;
from graphics import Renderer, Texture, Shader;
from audio import Sound, Music;

// =============================================================================
// CONSTANTS
// =============================================================================

@comptime const MAX_ENTITIES = 65536;
@comptime const WORLD_SIZE = 1024.0;
const GRAVITY = vec3(0.0, -9.81, 0.0);
const PI = 3.14159265358979;
const PLAYER_SPEED = 5.0;
const JUMP_FORCE = 10.0;

// =============================================================================
// ENUMS
// =============================================================================

enum State {
    Idle,
    Walking(float),
    Running(float),
    Jumping(vec3),
    Falling(float),
    Attacking(int),
    Dead
}

enum ItemType {
    Weapon(int, float),
    Armor(int),
    Potion(int),
    Key(string)
}

enum Result[T, E] {
    Ok(T),
    Err(E)
}

// =============================================================================
// STRUCTS
// =============================================================================

struct Color {
    public float r;
    public float g;
    public float b;
    public float a;

    def static white() -> Color {
        return (Color) { r: 1.0, g: 1.0, b: 1.0, a: 1.0 };
    }

    def static black() -> Color {
        return (Color) { r: 0.0, g: 0.0, b: 0.0, a: 1.0 };
    }

    def lerp(Color other, float t) -> Color {
        return (Color) {
            r: this.r + (other.r - this.r) * t,
            g: this.g + (other.g - this.g) * t,
            b: this.b + (other.b - this.b) * t,
            a: this.a + (other.a - this.a) * t
        };
    }
}

struct Transform {
    public vec3 position;
    public vec3 rotation;
    public vec3 scale;
}

struct Stats {
    public int health;
    public int max_health;
    public int mana;
    public int max_mana;
    public int strength;
    public int defense;
    public float speed;
}

// =============================================================================
// CORES
// =============================================================================

core Entity {
    readonly int id;
    public string name;
    public Transform transform;
    public bool active;

    def init(string name) {
        this.name = name;
        this.transform = (Transform) {
            position: vec3(0, 0, 0),
            rotation: vec3(0, 0, 0),
            scale: vec3(1, 1, 1)
        };
        this.active = true;
    }
}

core Player : Entity {
    public Stats stats;
    public State state;
    public list[ItemType] inventory;
    public dict[string -> int] skills;

    def init(string name, Stats stats) {
        super.init(name);
        this.stats = stats;
        this.state = State.Idle;
        this.inventory = [];
        this.skills = {};
    }
}

core Enemy : Entity {
    public Stats stats;
    public State state;
    public float aggro_range;
    public entity? target;

    def init(string name, Stats stats, float aggro_range) {
        super.init(name);
        this.stats = stats;
        this.state = State.Idle;
        this.aggro_range = aggro_range;
        this.target = null;
    }
}

// =============================================================================
// SHELLS
// =============================================================================

shell Damageable {
    requires {
        int health;
        int max_health;
    }

    def take_damage(int amount) -> result[int, string] {
        if core.health <= 0 {
            return error("Already dead");
        }

        int actual = amount.min(core.health);
        core.health -= actual;

        if core.health <= 0 {
            on_death();
        }

        return success(actual);
    }

    def heal(int amount) -> int {
        int actual = amount.min(core.max_health - core.health);
        core.health += actual;
        return actual;
    }

    def on_death() {
        // Override in specific shells
    }
}

shell Combat {
    requires {
        int strength;
        int defense;
        State state;
    }

    def attack(entity target) -> result[int, string] {
        if core.state is State.Dead {
            return error("Cannot attack while dead");
        }

        if not (target >- Damageable) {
            return error("Target is not damageable");
        }

        int base_damage = core.strength;
        int target_defense = 0;

        if target >- Combat {
            target_defense = target::Combat.get_defense();
        }

        int final_damage = (base_damage - target_defense).max(1);
        return target::Damageable.take_damage(final_damage);
    }

    def get_defense() -> int {
        return core.defense;
    }
}

shell Movement {
    requires {
        Transform transform;
        float speed;
        State state;
    }

    def move(vec3 direction) {
        if core.state is State.Dead {
            return;
        }

        vec3 velocity = direction.normalize() * core.speed;
        core.transform.position += velocity;

        if direction.length() > 0.1 {
            core.state = State.Walking(direction.length());
        } else {
            core.state = State.Idle;
        }
    }

    def teleport(vec3 position) {
        core.transform.position = position;
    }
}

shell AI {
    requires {
        Transform transform;
        float aggro_range;
        entity? target;
        State state;
    }

    def update(float delta) {
        if core.state is State.Dead {
            return;
        }

        // Find nearest player if no target
        if core.target == null {
            core.target = find_nearest_player();
        }

        // Check if target in range
        if core.target != null {
            float dist = distance(core.transform.position, core.target.transform.position);
            if dist <= core.aggro_range {
                pursue_target();
            } else {
                patrol();
            }
        } else {
            patrol();
        }
    }

    def find_nearest_player() -> entity? {
        // Implementation would search entities
        return null;
    }

    def pursue_target() {
        if core.target == null { return; }

        vec3 dir = core.target.transform.position - core.transform.position;
        if core >- Movement {
            core::Movement.move(dir);
        }
    }

    def patrol() {
        // Patrol logic
    }
}

shell StatusEffect {
    requires {
        Stats stats;
    }

    list[tuple[string, float, float]] effects; // name, remaining_time, multiplier

    def apply(string name, float duration, float multiplier) {
        effects.push((name, duration, multiplier));
    }

    def update(float delta) {
        for int i in (effects.len - 1)..=0 {
            effects[i].1 -= delta;
            if effects[i].1 <= 0 {
                effects.remove(i);
            }
        }
    }

    def get_modifier(string stat_name) -> float {
        float total = 1.0;
        for effect in effects {
            if effect.0 == stat_name {
                total *= effect.2;
            }
        }
        return total;
    }
}

// =============================================================================
// FUNCTIONS
// =============================================================================

def distance(vec3 a, vec3 b) -> float {
    vec3 diff = b - a;
    return diff.length();
}

def clamp(float value, float min_val, float max_val) -> float {
    if value < min_val { return min_val; }
    if value > max_val { return max_val; }
    return value;
}

def lerp(float a, float b, float t) -> float {
    return a + (b - a) * t;
}

def spawn_player(string name) -> coro[Player] {
    Stats stats = (Stats) {
        health: 100,
        max_health: 100,
        mana: 50,
        max_mana: 50,
        strength: 10,
        defense: 5,
        speed: PLAYER_SPEED
    };

    Player player = Player.new(name, stats);
    player <- Damageable <- Combat <- Movement <- StatusEffect;

    yield wait(0.1); // Wait for initialization

    return player;
}

def spawn_enemy(string name, vec3 position, float difficulty) -> coro[Enemy] {
    int health = (100 * difficulty) as int;
    int strength = (10 * difficulty) as int;

    Stats stats = (Stats) {
        health: health,
        max_health: health,
        mana: 0,
        max_mana: 0,
        strength: strength,
        defense: 2,
        speed: 3.0
    };

    Enemy enemy = Enemy.new(name, stats, 10.0);
    enemy.transform.position = position;
    enemy <- Damageable <- Combat <- Movement <- AI;

    yield wait(0.05);

    return enemy;
}

// =============================================================================
// GAME LOOP
// =============================================================================

def game_init() -> result[bool, string] {
    print("Initializing game...");

    // Create player
    coro[Player] player_coro = spawn spawn_player("Hero");
    Player player = yield player_coro;

    if player == null {
        return error("Failed to create player");
    }

    // Spawn enemies
    for int i in 0..10 {
        float x = (i % 5) as float * 10.0;
        float z = (i / 5) as float * 10.0;
        vec3 pos = vec3(x, 0, z);

        coro[Enemy] enemy_coro = spawn spawn_enemy("Goblin_{i}", pos, 1.0 + i * 0.1);
        Enemy enemy = yield enemy_coro;

        if enemy == null {
            print("Warning: Failed to spawn enemy {i}");
        }
    }

    print("Game initialized successfully!");
    return success(true);
}

def game_update(float delta) {
    // Update all entities
    for entity in get_all_entities() {
        if entity >- AI {
            entity::AI.update(delta);
        }

        if entity >- StatusEffect {
            entity::StatusEffect.update(delta);
        }
    }

    // Handle combat
    for player in get_players() {
        for enemy in get_enemies_in_range(player, 2.0) {
            if input_attack() && player >- Combat {
                match player::Combat.attack(enemy) {
                    success(damage) => print("Dealt {damage} damage!"),
                    error(msg) => print("Attack failed: {msg}")
                }
            }
        }
    }
}

def game_render() {
    // Clear screen
    clear(Color.black());

    // Render all visible entities
    for entity in get_visible_entities() {
        render_entity(entity);
    }

    // Render UI
    render_ui();
}

// =============================================================================
// MAIN
// =============================================================================

def main() -> int {
    print("=== Electron Game Demo ===");

    // Initialize
    match game_init() {
        success(_) => { },
        error(msg) => {
            print("Fatal error: {msg}");
            return 1;
        }
    }

    // Game loop
    float last_time = get_time();
    bool running = true;

    while running {
        float current_time = get_time();
        float delta = current_time - last_time;
        last_time = current_time;

        // Handle input
        if input_quit() {
            running = false;
            continue;
        }

        // Update
        game_update(delta);

        // Render
        game_render();

        // Frame limit
        yield wait(1.0 / 60.0);
    }

    print("Game ended.");
    return 0;
}
