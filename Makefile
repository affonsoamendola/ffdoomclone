CC = gcc
RM = rm -rf

BINARY := ffdoomclone
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))
CCFLAGS := `pkg-config --static --cflags sdl2 SDL2_image` -I../ff-stb/C -g
LDFLAGS := `pkg-config --static --libs sdl2 SDL2_image`
# ADD MORE WARNINGS!
WARNING := 

# .PHONY means these rules get executed even if
# files of those names exist.
.PHONY: all clean

# The first rule is the default, ie. "make",
# "make all" and "make parking" mean the same
all: $(BINARY)

run: $(BINARY)
	./$(BINARY)

clean:
	$(RM) *.o *.d $(BINARY)

# Linking the executable from the object files
$(BINARY): $(OBJECTS)
	$(CC) $(WARNING) $(CCFLAGS) $(LDFLAGS) $^ -o $@

-include $(DEPENDS)

%.o: %.c Makefile
	$(CC) $(WARNING) $(CCFLAGS) $(LDFLAGS) -MMD -MP -c $< -o $@