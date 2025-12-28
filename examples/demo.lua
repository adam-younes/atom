-- Lua Demo: Game Entity System
-- Demonstrates: tables, metatables, coroutines, closures, error handling, pattern simulation

local State = { Idle = "idle", Dead = "dead" }
local function Moving(pos) return { type = "moving", pos = pos } end
local function Attacking(dmg) return { type = "attacking", dmg = dmg } end
local function Stunned(dur) return { type = "stunned", dur = dur } end

local ItemType = { Weapon = "weapon", Armor = "armor", Potion = "potion" }
local function Item(name, kind, value) return { name = name, kind = kind, value = value } end

local function Stats(hp, atk, def)
    return { hp = hp or 100, max_hp = hp or 100, attack = atk or 10, defense = def or 5, state = State.Idle, shells = {} }
end

local function Inventory() return { items = {}, gold = 0 } end

-- Shell-like behavior via mixin pattern
local Damageable = {
    take_damage = function(self, amount)
        if self.state == State.Dead then return false end
        local actual = math.max(amount - self.defense, 1)
        self.hp = math.max(self.hp - actual, 0)
        if self.hp == 0 then self.state = State.Dead end
        return true
    end,
    heal = function(self, amount)
        local healed = math.min(self.max_hp - self.hp, amount)
        self.hp = self.hp + healed
        return healed
    end,
    is_alive = function(self) return self.hp > 0 end
}

local BuffSystem = {
    buffs = {},
    add_buff = function(self, name, dur, effect)
        self.buffs = self.buffs or {}
        table.insert(self.buffs, { name = name, dur = dur, effect = effect })
    end,
    update = function(self, dt)
        local active = {}
        for _, b in ipairs(self.buffs or {}) do
            if b.dur - dt > 0 then table.insert(active, { name = b.name, dur = b.dur - dt, effect = b.effect }) end
        end
        self.buffs = active
    end,
    apply_all = function(self, stats)
        for _, b in ipairs(self.buffs or {}) do b.effect(stats) end
    end
}

local Combat = {
    attack_target = function(self, target)
        if self.state == State.Dead then return nil, "Cannot attack while dead" end
        if type(self.state) == "table" and self.state.type == "stunned" then return nil, "Cannot attack while stunned" end
        local dmg = self.attack + math.random(0, 4)
        target:take_damage(dmg)
        self.state = Attacking(dmg)
        return dmg
    end
}

local Statemachine = {
    transition = function(self, new_state)
        if self.state == State.Dead then return end
        if type(new_state) == "table" and new_state.type == "stunned" and new_state.dur <= 0 then return end
        self.state = new_state
    end,
    update = function(self, dt)
        local s = self.state
        if type(s) == "table" then
            if s.type == "stunned" then
                self.state = s.dur > dt and Stunned(s.dur - dt) or State.Idle
            elseif s.type == "attacking" then
                self.state = State.Idle
            end
        end
    end
}

local InventoryManager = {
    add_item = function(self, item)
        if not self.inventory then return false end
        if #self.inventory.items >= 20 then return false end
        table.insert(self.inventory.items, item)
        return true
    end,
    use_item = function(self, idx)
        if not self.inventory or idx < 1 or idx > #self.inventory.items then return nil, "Invalid slot" end
        local item = self.inventory.items[idx]
        if item.kind == ItemType.Potion then
            local healed = self:heal(item.value)
            table.remove(self.inventory.items, idx)
            return string.format("Healed %d HP", healed)
        elseif item.kind == ItemType.Weapon then
            return string.format("Equipped %s (+%d ATK)", item.name, item.value)
        else
            return string.format("Equipped %s (+%d DEF)", item.name, item.value)
        end
    end,
    get_summary = function(self)
        if not self.inventory then return "No inventory" end
        local counts = { weapons = 0, armor = 0, potions = 0 }
        for _, item in ipairs(self.inventory.items) do
            if item.kind == ItemType.Weapon then counts.weapons = counts.weapons + 1
            elseif item.kind == ItemType.Armor then counts.armor = counts.armor + 1
            else counts.potions = counts.potions + 1 end
        end
        return string.format("W:%d A:%d P:%d", counts.weapons, counts.armor, counts.potions)
    end
}

-- Shell attachment via mixin merging
local function attach_shell(entity, shell)
    for k, v in pairs(shell) do
        if type(v) == "function" then entity[k] = v
        elseif entity[k] == nil then entity[k] = v end
    end
    table.insert(entity.shells, shell)
    return entity
end

-- Coroutine-based spawner
local function spawn_player(name)
    return coroutine.create(function()
        local player = Stats(100, 15, 8)
        attach_shell(player, Damageable)
        attach_shell(player, BuffSystem)
        attach_shell(player, Combat)
        attach_shell(player, Statemachine)
        coroutine.yield(0.1) -- wait simulation
        return player
    end)
end

-- Coroutine-based combat
local function combat_sequence(attacker, defender)
    return coroutine.create(function()
        for round = 1, 3 do
            local dmg, err = attacker:attack_target(defender)
            if not dmg then return nil, err end
            if not defender:is_alive() then return string.format("Victory in round %d!", round) end
            coroutine.yield(0.5) -- wait simulation
        end
        return "Combat ended"
    end)
end

-- Main simulation
local function main()
    local player_coro = spawn_player("Hero")
    coroutine.resume(player_coro) -- tick
    local _, player = coroutine.resume(player_coro)

    local enemy = Stats(50, 8, 2)
    attach_shell(enemy, Damageable)
    attach_shell(enemy, Combat)
    attach_shell(enemy, Statemachine)

    player:add_buff("Rage", 10.0, function(s) s.attack = s.attack + 5 end)

    local battle = combat_sequence(player, enemy)
    local outcome, err
    repeat
        _, outcome, err = coroutine.resume(battle)
    until coroutine.status(battle) == "dead"

    if outcome then print(outcome)
    else print("Error: " .. (err or "unknown")) end
end

main()
