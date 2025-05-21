CC = gcc
CFLAGS = -O2 -std=c99 -I../include
PKG   = $(shell pkg-config --cflags --libs wayland-client wayland-egl egl glesv2)
LDFLAGS = -lm

SRCS = main.c mesh/mesh.c mesh/obj_loader.c opengl/glad.c window/xdg-shell-protocol.c
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(PKG) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o mesh/*.o window/*.o opengl/*.o main

