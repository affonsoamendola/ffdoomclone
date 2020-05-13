CC = gcc
RM = rm -rf

BINARY := ffdoomclone
SOURCES := $(wildcard *.c)
OBJECTS := $(patsubst %.c,%.o,$(SOURCES))
DEPENDS := $(patsubst %.c,%.d,$(SOURCES))
CCFLAGS := -I/usr/include/SDL2 -D_REENTRANT -I../ff-stb/C -g
LDFLAGS := -lSDL2_image -pthread -lSDL2 -lc -lm
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