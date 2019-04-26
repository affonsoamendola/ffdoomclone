GAMENAME = rt76800
CC = gcc
RM = rm -rf
INCLUDE = /usr/include/SDL

all: build
	
build:
	$(CC) -g -o $(GAMENAME) -I$(INCLUDE) -lSDL -lm *.c

run:
	./$(GAMENAME)

clean:
	$(RM) ./$(GAMENAME) *.o