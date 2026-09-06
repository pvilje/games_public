#!/bin/bash
#gcc minesweeper.c -o Minesweeper -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
# zig cc minesweeper.c -o Minesweeper -std=c23 -lraylib
zig cc minesweeper.c -std=c23 -I ./raylib -L ./raylib -lc -lraylib -o Minesweeper