CC=clang
CFLAGS=-Wall -g

SRCDIR=src
OBJDIR=obj

SRC=$(wildcard $(SRCDIR)/*.c)
OBJ=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))

$(info $(shell mkdir -p $(OBJDIR)))

all: $(OBJ)
	$(CC) $(OBJ) -o main

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@