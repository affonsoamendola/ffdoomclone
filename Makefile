GAMENAME = rt76800
CC = gcc
RM = rm -rf
INCLUDE = /usr/include/SDL

all: build
	
build:
	$(CC)  *.c  -g -o $(GAMENAME) -I$(INCLUDE) -lSDL -lSDL_image -lm

run:
	./$(GAMENAME)

clean:
	$(RM) ./$(GAMENAME) *.o