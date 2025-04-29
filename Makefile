# Choose your platform: uncomment one of these:
#PLATFORM := WIN32
#PLATFORM := X11
PLATFORM := WAYLAND

CC      := gcc
CFLAGS  := -Iinclude -Wall -Wextra -g
LDFLAGS :=

SRC_COMMON := src/main.c

ifeq ($(PLATFORM),WAYLAND)
	CFLAGS  += -D__linux__
	SRC_BACKEND := src/window/window.c
	LDFLAGS += $(shell pkg-config --cflags --libs wayland-client)
endif

SRCS := $(SRC_COMMON) $(SRC_BACKEND)
OBJS := $(SRCS:.c=.o)

.PHONY: all clean

all: bin/test_window

bin/test_window: $(OBJS) | bin
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin:
	mkdir -p bin

%.o: %.c wayland-logo.h window/window.h math/la.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf *.o src/*.o bin src/window/*.o

