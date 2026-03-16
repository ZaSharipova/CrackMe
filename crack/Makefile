CC      = g++
TARGET  = crack
# CFLAGS = -ggdb3 -g -std=c++17 -O0 \
#     -Iinclude -Wall -Wextra -Weffc++ -Wc++14-compat -Wmissing-declarations \
#     -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion \
#     -Wctor-dtor-privacy -Wempty-body -Wfloat-equal \
#     -Wformat-nonliteral -Wformat-security -Wformat=2 \
#     -Winline -Wnon-virtual-dtor -Woverloaded-virtual \
#     -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls \
#     -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-overflow=2 \
#     -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef \
#     -Wunreachable-code -Wunused -Wvariadic-macros \
#     -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs \
#     -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector \
#     -fstrict-overflow -fno-omit-frame-pointer -Wlarger-than=8192 \
#     -fPIE -Werror=vla \
#     -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \

CFLAGS = \
	$(shell sdl2-config --cflags) \
    $(shell pkg-config --cflags SDL2_mixer SDL2_image SDL2_ttf)

LIBS    = $(shell sdl2-config --libs) \
          $(shell pkg-config --libs SDL2_mixer SDL2_image SDL2_ttf)

all: $(TARGET)

$(TARGET): main.cpp Crack.cpp
	@$(CC) -o $(TARGET) main.cpp Crack.cpp $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)
.PHONY: clean