CC = gcc
CFLAGS = -O2 -std=c99 -I./include -Wall -Wextra
PKG = $(shell pkg-config --cflags --libs wayland-client wayland-egl egl glesv2)
LDFLAGS = -lm

BINDIR = bin
TARGET = $(BINDIR)/atom

SRCS = src/main.c src/mesh/mesh.c src/mesh/obj_loader.c src/opengl/glad.c src/window/xdg-shell-protocol.c
OBJS = $(SRCS:src/%.c=$(BINDIR)/obj/%.o)

all: $(TARGET)

$(TARGET): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $^ -o $@ $(PKG) $(LDFLAGS)

$(BINDIR)/obj/%.o: src/%.c | $(BINDIR)/obj $(BINDIR)/obj/mesh $(BINDIR)/obj/opengl $(BINDIR)/obj/window
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR) $(BINDIR)/obj $(BINDIR)/obj/mesh $(BINDIR)/obj/opengl $(BINDIR)/obj/window:
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BINDIR)

.PHONY: all run clean

