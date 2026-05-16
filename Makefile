CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Wno-format-truncation -g
LIBS = -lm
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0 2>/dev/null || echo "")
GTK_LIBS = $(shell pkg-config --libs gtk+-3.0 2>/dev/null || echo "-lgtk-3 -lgdk-3 -lgio-2.0 -lglib-2.0")

TARGET_CLI = medical_system
TARGET_GUI = medical_system_gui

.PHONY: all cli gui clean run

all: cli gui

cli: $(TARGET_CLI)

gui: $(TARGET_GUI)

$(TARGET_CLI): medical_system.c
	$(CC) $(CFLAGS) -o $@ medical_system.c $(LIBS)

$(TARGET_GUI): main.c medical_system_gui.c medical_system.c
	$(CC) $(CFLAGS) -DGTK_BUILD $(GTK_CFLAGS) -o $@ main.c medical_system_gui.c medical_system.c $(GTK_LIBS) $(LIBS)

run-cli: $(TARGET_CLI)
	./$(TARGET_CLI)

run-gui: $(TARGET_GUI)
	./$(TARGET_GUI)

clean:
	rm -f $(TARGET_CLI) $(TARGET_GUI)
