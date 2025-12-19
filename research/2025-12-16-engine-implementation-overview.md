---
date: 2025-12-16T17:21:34-06:00
researcher: Claude
git_commit: 5db872d2874ab6ae80570255c8956f1164a7a8da
branch: main
repository: atom
topic: "Overall Engine Implementation - Motivation, Limitations, and Implementation Status"
tags: [research, codebase, engine, architecture, wayland, opengl, ecs, scripting]
status: complete
last_updated: 2025-12-16
last_updated_by: Claude
---

# Research: Atom Game Engine Implementation Overview

**Date**: 2025-12-16T17:21:34-06:00
**Researcher**: Claude
**Git Commit**: 5db872d2874ab6ae80570255c8956f1164a7a8da
**Branch**: main
**Repository**: atom

## Research Question

Research the overall implementation of the engine, understanding the motivation behind decisions, the limitations of the codebase as it is, and what is implemented vs. what is not.

## Summary

Atom is a Linux-native Wayland game engine written in C99, designed with simplicity and explicitness as core principles. The engine implements a data-oriented component-based architecture with direct scene-level component storage. Currently, the engine provides a functional foundation with Wayland windowing, OpenGL 4.5 rendering, an entity-component system, input handling, OBJ mesh loading, and a basic movement system. A scripting language called "Electron" is in early development. The engine deliberately avoids complexity in favor of straightforward, understandable code.

---

## Detailed Findings

### 1. Core Architecture

**File:** `engine/src/engine.c`, `engine/include/engine.h`

#### Design Philosophy
The engine follows a **callback-driven architecture** where the game layer provides four function pointers:
- `init`: Called once during startup
- `update`: Called each frame with delta time
- `render`: Called each frame after update
- `cleanup`: Called before shutdown

This design keeps the engine core simple and the game logic separate, with the engine handling platform-specific details (Wayland, EGL, input) internally.

#### Main Entry Point
```c
typedef struct {
  const char *title;
  int width;
  int height;
} atom_config;

int atom_run(atom_config *config, atom_callbacks *callbacks);
```

The single `atom_run()` function handles all initialization, runs the main loop, and performs cleanup. This monolithic approach avoids complex state management at the cost of flexibility.

#### Main Loop Structure
The engine uses a non-blocking event loop (`engine/src/engine.c:91-109`):
1. `wl_display_dispatch_pending()` - Process Wayland events
2. Calculate delta time using `CLOCK_MONOTONIC`
3. `input_update(dt)` - Process input callbacks
4. `callbacks->update(dt)` - Game update
5. `callbacks->render()` - Game rendering
6. `eglSwapBuffers()` - Present frame
7. `wl_display_flush()` - Flush Wayland requests

---

### 2. Entity-Component System

**Files:** `engine/src/scene/entity.c`, `engine/src/scene/scene.c`, `engine/include/scene/`

#### Entity System
- **Entity IDs**: 64-bit unsigned integers (`uint64_t`)
- **Allocation**: Sequential, monotonically increasing (never reused)
- **Maximum**: 65,536 entities (`MAX_ENTITIES`)
- **Tracking**: Global boolean array `entity_alive[MAX_ENTITIES]`

```c
entity_id entity_create(void);      // Returns next available ID
void entity_destroy(entity_id id);  // Marks as dead (doesn't reclaim ID)
bool entity_is_alive(entity_id id); // Query alive status
```

#### Scene Structure
The scene serves as the central container for all component arrays:

```c
typedef struct {
  transform_component *transforms;       // capacity: 256
  mesh_renderer_component *mesh_renderers; // capacity: 256
  light_component *lights;               // capacity: 64
  camera_component *cameras;             // capacity: 16
  controller_component *controllers;     // capacity: 64
  entity_id active_camera;
} scene;
```

Each component type has its own dynamically-resizing array with `count` and `capacity` tracking.

#### Component Lookup
Components are retrieved via **linear search** through their respective arrays:
```c
transform_component* scene_get_transform(scene *s, entity_id id) {
  for (size_t i = 0; i < s->transform_count; i++) {
    if (s->transforms[i].entity == id) return &s->transforms[i];
  }
  return NULL;
}
```
This is O(n) per lookup, prioritizing simplicity over performance.

---

### 3. Components

**Files:** `engine/include/components/`, `engine/src/components/`

#### Transform Component
- **Fields**: entity, parent, position (vec3), rotation (vec3), scale (vec3), local_matrix, world_matrix, dirty flag
- **Features**: Supports parent-child hierarchies via `parent` entity ID
- **Updates**: `scene_update_transforms()` iterates all transforms and recalculates matrices when dirty

#### Mesh Renderer Component
- **Fields**: entity, mesh pointer, material_id, OpenGL handles (VAO, VBOs, EBO), initialized flag
- **GPU Resources**: Position VBO, normal VBO, index EBO
- **Cleanup**: `mesh_renderer_component_cleanup()` deletes GL resources

#### Camera Component
- **Fields**: entity, FOV, aspect ratio, near/far planes, view_matrix, projection_matrix
- **Functions**: `camera_component_update_view()` (look-at), `camera_component_update_projection()` (perspective)

#### Light Component
- **Fields**: entity, position (vec3), color (vec3), intensity (float)
- **Usage**: Position/color uploaded as shader uniforms

#### Controller Component
- **Fields**: entity, target_entity, move_speed, sprint_multiplier, mouse_sensitivity, pitch, yaw, use_pitch_yaw
- **Functions**: `controller_get_forward()`, `controller_get_right()`, `controller_rotate()`
- **Purpose**: First-person camera control with mouse look

---

### 4. Systems

**Files:** `engine/include/systems/movement.h`, `engine/src/systems/movement.c`

#### Movement System
The only implemented system provides six directional movement functions:
- `movement_system_move_forward/backward/left/right/up/down(scene*, controller_component*, float dt)`

Each function:
1. Retrieves target entity's transform via `scene_get_transform()`
2. Calculates direction using `controller_get_forward()` or `controller_get_right()`
3. Applies movement based on speed and delta time
4. Handles sprint multiplier when Shift is pressed
5. Marks transform dirty for matrix recalculation

#### System Invocation Pattern
Systems are **not automatically scheduled**. The game layer manually calls system functions through input callbacks:
```c
static void cam_move_forward(float dt) {
  controller_component *ctrl = scene_get_controller(&game_scene, controller_entity);
  if (ctrl) movement_system_move_forward(&game_scene, ctrl, dt);
}
```

---

### 5. Window/Wayland Integration

**Files:** `engine/include/window/window.h`, `engine/src/engine.c`

#### Protocols Used
1. **xdg-shell**: Desktop window management (toplevel windows)
2. **pointer-constraints-unstable-v1**: Mouse locking for FPS-style controls
3. **relative-pointer-unstable-v1**: Raw mouse motion deltas

#### EGL/OpenGL Setup
- **API**: OpenGL 4.5 Core Profile
- **Color**: 8-bit RGBA
- **Depth**: 24-bit
- **Function Loading**: GLAD with `eglGetProcAddress`

#### Event Handling
- **Keyboard**: Maps Linux keycodes to engine key codes (W, A, S, D, Q, E, Space, Shift, Escape)
- **Mouse**: Relative motion via `zwp_relative_pointer_v1`, pointer locking via `zwp_locked_pointer_v1`
- **Window Close**: Sets `running = false` to exit main loop

---

### 6. Rendering

**Files:** `engine/src/lib/opengl/`, `engine/include/opengl/`

#### Shader System
- **Loading**: `make_program_from_files(vs_path, fs_path)` compiles and links shaders
- **Attribute Binding**: Hard-coded `aPos` at location 0
- **Error Handling**: Exits on shader compilation/linking failure

#### Scene Rendering
`scene_render()` iterates all mesh renderers:
1. Gets active camera for view/projection matrices
2. For each mesh renderer with valid mesh and initialization:
   - Gets associated transform for model matrix
   - Computes normal matrix (transpose of inverse model)
   - Uploads matrices to shader uniforms
   - Binds VAO and draws with `glDrawElements()`

#### Shaders Available
- `phong.vert/frag`: Smooth Phong shading with ambient, diffuse, specular
- `phong_flat.vert/frag`: Flat-shaded variant

---

### 7. Asset Loading

**Files:** `engine/src/assets/mesh/`, `engine/include/assets/mesh.h`

#### Mesh Structure
```c
typedef struct {
  float *positions;    // 3 floats per vertex
  float *normals;      // 3 floats per vertex
  float *texcoords;    // 2 floats per vertex (optional)
  uint32_t *indices;
  size_t *vert_count;
  size_t *idx_count;
} mesh;
```

#### OBJ Loader Features
- Full OBJ support: positions, texture coordinates, normals
- Vertex deduplication via FNV-1a hash table with quadratic probing
- Support for negative (relative) indices
- Dynamic array growth (doubling strategy)
- Lazy allocation for texcoords/normals (only if present in file)

#### Normal Generation
- `generate_normals()`: Smooth normals via weighted averaging
- `generate_flat_normals()`: Flat normals (one per triangle)

---

### 8. Input System

**Files:** `engine/include/input/input.h`, `engine/src/input/input.c`

#### Supported Keys
9 keys: W, A, S, D, Q, E, Space, Shift, Escape

#### Callback Types
- `on_press`: Fires once on key down transition
- `on_release`: Fires once on key up transition
- `on_hold`: Fires every frame while key is held (receives delta time)
- `mouse_move_handler`: Receives dx, dy deltas

#### Pointer Constraints
- `input_set_mouse_locked(bool)`: Toggles pointer lock
- Mouse motion only processed when locked (for FPS-style controls)

---

### 9. Electron Scripting Language (In Development)

**Files:** `engine/include/electron/`, `engine/src/electron/`, `engine/test/electron/`

#### Implementation Status
| Component | Status |
|-----------|--------|
| Error Handling | **Implemented** - Full error context, colored output, source display |
| Lexer | **Partial** - Interface defined, file loading ready, tokenization unimplemented |
| Parser | Not started (empty file) |
| AST | Not started (empty file) |
| Bytecode | Not started (empty file) |
| Compiler | Not started (empty file) |
| VM | Not started (empty file) |

#### Language Syntax (from test scripts)
```electron
// hello-world.e
print("Hello World");

// functions.e
void test() {
  print("Hello World!");
}
```

The language appears to target C-like syntax with explicit types.

---

## Implementation Status Summary

### What Is Implemented

| Feature | Status | Notes |
|---------|--------|-------|
| Wayland Windowing | Complete | xdg-shell, pointer constraints |
| OpenGL 4.5 Core | Complete | Via EGL and GLAD |
| Entity System | Complete | Sequential IDs, 65k max |
| Component System | Complete | 5 component types |
| Transform Hierarchy | Complete | Parent-child relationships |
| Movement System | Complete | 6-axis movement with sprint |
| Input System | Complete | Keyboard + mouse |
| OBJ Loading | Complete | With deduplication |
| Phong Shading | Complete | Smooth and flat variants |
| Camera System | Complete | First-person controller |

### What Is Not Implemented

Based on the extensive TODO.md, these major features are **not implemented**:

**Rendering:**
- PBR materials
- Shadow mapping
- Post-processing effects
- Deferred rendering
- Particle systems
- Terrain rendering

**Assets:**
- GLB/GLTF/FBX loaders (stub files exist)
- Texture loading
- Audio system
- Asset hot-reloading

**Physics:**
- Collision detection
- Rigid body dynamics
- Raycasting

**Animation:**
- Skeletal animation
- Animation blending

**Scripting:**
- Electron language (only error handling implemented)

**Networking:**
- Multiplayer support

**Tools:**
- Editor/level design tools
- Debug visualization

---

## Architecture Documentation

### Design Decisions and Motivations

1. **Monolithic `atom_run()` Entry Point**
   - Single function handles all platform setup
   - Game layer only provides callbacks
   - Simplifies game code at cost of engine flexibility

2. **Linear Component Lookup**
   - O(n) search prioritizes code simplicity
   - Suitable for small-to-medium entity counts
   - No complex data structures or caching

3. **No System Scheduler**
   - Systems are called manually from game code
   - Explicit control over execution order
   - Avoids hidden dependencies

4. **Sequential Entity IDs Without Reuse**
   - Simple incrementing counter
   - Dead entity IDs are never reclaimed
   - May exhaust 65k limit in long sessions with frequent spawn/destroy

5. **Wayland-Only, No X11**
   - Modern Linux focus
   - Simpler codebase (single windowing backend)
   - Limits compatibility with older systems

6. **Direct OpenGL Without Abstraction Layer**
   - No Vulkan support
   - Shader management is minimal
   - GPU resources managed directly in components

### Architectural Constraints

1. **Entity Limit**: 65,536 entities maximum (non-reusable IDs)
2. **Component Lookup**: O(n) per entity per frame
3. **Single Scene**: No multi-scene or scene streaming
4. **Fixed Component Types**: Adding new components requires engine modification
5. **No Serialization**: Scene state cannot be saved/loaded
6. **Single Thread**: No parallel systems or job scheduling

---

## Code References

- Engine entry point: `engine/src/engine.c:18` (`atom_run`)
- Main loop: `engine/src/engine.c:91-109`
- Entity creation: `engine/src/scene/entity.c:6-13`
- Component storage: `engine/include/scene/scene.h:8-30`
- Transform update: `engine/src/scene/scene.c:152-163`
- Scene rendering: `engine/src/scene/scene.c:165-187`
- OBJ loader: `engine/src/assets/mesh/obj_loader.c:95-293`
- Movement system: `engine/src/systems/movement.c:5-85`
- Input handling: `engine/src/input/input.c:17-73`
- Wayland connection: `engine/src/engine.c:19-32`
- EGL setup: `engine/src/engine.c:50-78`
- Shader loading: `engine/src/lib/opengl/shader.c:37-67`
- Electron error handling: `engine/src/electron/error.c:1-155`
- Game example: `game/src/main.c:1-262`

---

## Related Research

No previous research documents found in this repository.

---

## Open Questions

1. **Entity ID Exhaustion**: How will the engine handle long-running games that create/destroy many entities over time?

2. **Component Type Extension**: What is the intended mechanism for adding custom component types from the game layer?

3. **Electron Integration**: How will the scripting language interface with the entity-component system once implemented?

4. **Multi-Scene Support**: Is scene streaming or multiple simultaneous scenes planned?

5. **Build System**: The Makefile doesn't include Electron files - is there a separate build configuration planned?
