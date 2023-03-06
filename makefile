CC = gcc
CFLAGS = -g -Wall -pedantic -pthread
EXE = main

SRCPATH = ./src
INCPATH = ./includes
OBJPATH = ./obj

SRC_FILES = $(wildcard $(SRCPATH)/*.c)
OBJ_FILES = $(patsubst $(SRCPATH)/%.c,$(OBJPATH)/%.o,$(SRC_FILES))

all: clean folders $(EXE)

folders:
	mkdir -p $(OBJPATH)

$(EXE): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $(EXE)

$(OBJPATH)/%.o: $(SRCPATH)/%.c
	$(CC) $(CFLAGS) -I $(INCPATH) -c $< -o $@

valgrind:
	valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(EXE)

clean:
	rm -rf $(OBJPATH)
	rm -f $(EXE)
