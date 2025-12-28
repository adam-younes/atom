// Electron Demo: Game Entity System
// Demonstrates: cores, shells, enums, pattern matching, result types, coroutines, collections

enum State { Idle, Moving(vec3), Attacking(int), Stunned(float), Dead }
enum ItemType { Weapon(int), Armor(int), Potion(int) }

struct Item { public string name; public ItemType kind; }

core Stats {
    public int hp = 100;
    public int max_hp = 100;
    public int attack = 10;
    public int defense = 5;
    public State state = State.Idle;
}

core Inventory {
    public list[Item] items = [];
    public int gold = 0;
}

shell Damageable {
    requires { int hp, int max_hp, int defense, State state }

    def take_damage(int amount) -> bool {
        if core.state is State.Dead { return false; }
        int actual = (amount - core.defense).max(1);
        core.hp = (core.hp - actual).max(0);
        if core.hp == 0 { core.state = State.Dead; }
        return true;
    }

    def heal(int amount) -> int {
        int healed = (core.max_hp - core.hp).min(amount);
        core.hp += healed;
        return healed;
    }

    def is_alive() -> bool { return core.hp > 0; }
}

shell BuffSystem {
    list[tuple[string, float, func[(Stats) -> void]]] buffs = [];

    def add_buff(string name, float dur, func[(Stats) -> void] effect) {
        buffs.push((name, dur, effect));
    }

    def update(float dt) {
        list[tuple[string, float, func[(Stats) -> void]]] active = [];
        for tuple[string, float, func[(Stats) -> void]] (name, dur, effect) in buffs {
            if dur - dt > 0 { active.push((name, dur - dt, effect)); }
        }
        buffs = active;
    }

    def apply_all(Stats s) {
        for tuple[string, float, func[(Stats) -> void]] (_, _, effect) in buffs {
            effect(s);
        }
    }
}

shell Combat {
    requires { int attack, State state }
    requires inner { Damageable }

    def attack_target(Stats target) -> result[int, string] {
        if core.state is State.Dead { return error("Cannot attack while dead"); }
        if core.state is State.Stunned(_) { return error("Cannot attack while stunned"); }
        int dmg = core.attack + (0..5).to_list()[0];
        target.take_damage(dmg);
        core.state = State.Attacking(dmg);
        return success(dmg);
    }
}

shell Statemachine {
    requires { State state }

    def transition(State new_state) {
        match (core.state, new_state) {
            (State.Dead, _) => { },
            (_, State.Stunned(dur)) if dur <= 0 => { },
            (_, s) => { core.state = s; }
        }
    }

    def update(float dt) {
        match core.state {
            State.Stunned(remaining) if remaining > 0 => {
                core.state = State.Stunned(remaining - dt);
            },
            State.Stunned(_) => { core.state = State.Idle; },
            State.Attacking(_) => { core.state = State.Idle; },
            _ => { }
        }
    }
}

shell InventoryManager {
    requires entity { Inventory }

    def add_item(Item item) -> bool {
        Inventory inv = sibling[Inventory];
        if inv.items.length >= 20 { return false; }
        inv.items.push(item);
        return true;
    }

    def use_item(int idx) -> result[string, string] {
        Inventory inv = sibling[Inventory];
        if idx < 0 || idx >= inv.items.length { return error("Invalid slot"); }
        Item item = inv.items[idx];
        match item.kind {
            ItemType.Potion(heal) => {
                int healed = inner[Damageable].heal(heal);
                inv.items.remove(idx);
                return success("Healed {healed} HP");
            },
            ItemType.Weapon(dmg) => { return success("Equipped {item.name} (+{dmg} ATK)"); },
            ItemType.Armor(def) => { return success("Equipped {item.name} (+{def} DEF)"); }
        }
    }

    def get_summary() -> string {
        Inventory inv = sibling[Inventory];
        dict[string -> int] counts;
        for Item item in inv.items {
            string key = match item.kind {
                ItemType.Weapon(_) => "weapons",
                ItemType.Armor(_) => "armor",
                ItemType.Potion(_) => "potions"
            };
            counts[key] = (counts[key] ?? 0) + 1;
        }
        return "W:{counts["weapons"] ?? 0} A:{counts["armor"] ?? 0} P:{counts["potions"] ?? 0}";
    }
}

def spawn_player(string name) -> coro[Stats] {
    Stats player = (Stats) { hp: 100, max_hp: 100, attack: 15, defense: 8 };
    player <- Damageable <- BuffSystem <- Combat <- Statemachine;
    yield wait(0.1);
    return player;
}

def combat_sequence(Stats attacker, Stats defender) -> coro[result[string, string]] {
    for int round in 1..=3 {
        result[int, string] res = attacker.attack_target(defender);
        if res is error(msg) { return error(msg); }
        if res is success(dmg) {
            if !defender.is_alive() { return success("Victory in round {round}!"); }
        }
        yield wait(0.5);
    }
    return success("Combat ended");
}

def main() {
    Stats player = yield spawn spawn_player("Hero");
    Stats enemy = (Stats) { hp: 50, attack: 8, defense: 2 };
    enemy <- Damageable <- Combat <- Statemachine;

    player.add_buff("Rage", 10.0, (Stats s) { s.attack += 5; });

    coro[result[string, string]] battle = spawn combat_sequence(player, enemy);
    result[string, string] outcome = yield battle;

    match outcome {
        success(msg) => print(msg),
        error(msg) => print("Error: {msg}")
    }
}
