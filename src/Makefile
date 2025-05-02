CC = gcc
CFLAGS = -O2 -std=c99 -I../include
PKG   = $(shell pkg-config --cflags --libs wayland-client wayland-egl egl glesv2)

SRCS = main.c graphics/gl.c window/xdg-shell-protocol.c
OBJS = $(SRCS:.c=.o)

main: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(PKG)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o window/*.o graphics/*.o main

