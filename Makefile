GAMENAME = rt76800
CC = gcc
RM = rm -rf
INCLUDE = 

all: build
	
build:
	$(CC) -o $(GAMENAME) $$(sdl-config --cflags --libs) *.c

run:
	./$(GAMENAME)

clean:
	$(RM) ./$(GAMENAME) *.o