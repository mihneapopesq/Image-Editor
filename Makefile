# Compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Define targets
TARGETS = image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c -g -lm -o image_editor

pack:
	zip -FSr 312CA_PopescuMihneaGabriel_Tema3.zip README.md Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean