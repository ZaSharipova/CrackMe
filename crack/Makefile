CC      = gcc
TARGET  = crack
CFLAGS  = $(shell sdl2-config --cflags) \
          $(shell pkg-config --cflags SDL2_mixer SDL2_image SDL2_ttf)
LIBS    = $(shell sdl2-config --libs) \
          $(shell pkg-config --libs SDL2_mixer SDL2_image SDL2_ttf)
all: $(TARGET)
$(TARGET): main.c Crack.c
	$(CC) -o $(TARGET) main.c Crack.c $(CFLAGS) $(LIBS)
clean:
	rm -f $(TARGET)
.PHONY: clean