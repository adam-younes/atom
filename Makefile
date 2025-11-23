CC = gcc
CFLAGS = -O2 -std=c99 -Wall -Wextra
PKG = $(shell pkg-config --cflags --libs wayland-client wayland-cursor wayland-egl egl glesv2)
LDFLAGS = -lm

BINDIR = bin
ENGINE_LIB = $(BINDIR)/libatom.a
GAME_TARGET = $(BINDIR)/atom_game

ENGINE_SRCS = engine/src/engine.c engine/src/scene/entity.c engine/src/scene/scene.c engine/src/input/input.c engine/src/components/transform.c engine/src/components/mesh_renderer.c engine/src/components/light.c engine/src/components/camera.c engine/src/components/controller.c engine/src/systems/movement.c engine/src/assets/mesh/mesh.c engine/src/assets/mesh/obj_loader.c engine/src/lib/opengl/opengl.c engine/src/lib/opengl/shader.c engine/src/lib/opengl/glad.c engine/src/window/xdg-shell-protocol.c engine/src/window/pointer-constraints-unstable-v1-protocol.c engine/src/window/relative-pointer-unstable-v1-protocol.c
ENGINE_OBJS = $(ENGINE_SRCS:engine/src/%.c=$(BINDIR)/obj/engine/%.o)

GAME_SRCS = game/src/main.c
GAME_OBJS = $(GAME_SRCS:game/src/%.c=$(BINDIR)/obj/game/%.o)

all: $(GAME_TARGET)

$(ENGINE_LIB): $(ENGINE_OBJS) | $(BINDIR)
	ar rcs $@ $^

$(GAME_TARGET): $(GAME_OBJS) $(ENGINE_LIB) | $(BINDIR)
	$(CC) $(CFLAGS) $(GAME_OBJS) -o $@ -L$(BINDIR) -latom $(PKG) $(LDFLAGS)

$(BINDIR)/obj/engine/%.o: engine/src/%.c | $(BINDIR)/obj/engine $(BINDIR)/obj/engine/scene $(BINDIR)/obj/engine/input $(BINDIR)/obj/engine/components $(BINDIR)/obj/engine/systems $(BINDIR)/obj/engine/assets/mesh $(BINDIR)/obj/engine/lib/opengl $(BINDIR)/obj/engine/window
	$(CC) $(CFLAGS) -I./engine/include -c $< -o $@

$(BINDIR)/obj/game/%.o: game/src/%.c | $(BINDIR)/obj/game
	$(CC) $(CFLAGS) -I./engine/include -c $< -o $@

$(BINDIR) $(BINDIR)/obj $(BINDIR)/obj/engine $(BINDIR)/obj/engine/scene $(BINDIR)/obj/engine/input $(BINDIR)/obj/engine/components $(BINDIR)/obj/engine/systems $(BINDIR)/obj/engine/assets $(BINDIR)/obj/engine/assets/mesh $(BINDIR)/obj/engine/lib $(BINDIR)/obj/engine/lib/opengl $(BINDIR)/obj/engine/window $(BINDIR)/obj/game:
	mkdir -p $@

run: $(GAME_TARGET)
	./$(GAME_TARGET)

clean:
	rm -rf $(BINDIR)

.PHONY: all run clean

