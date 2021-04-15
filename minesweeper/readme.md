# Minesweeper

## Run game

Launch by double-clicking Minesweeper.pyw
Uses tkinter for graphics.

## Gameplay (from wikipedia)

In Minesweeper, mines (that resemble naval mines in the classic theme) are scattered throughout a board, which is divided into cells. Cells have three states: uncovered, covered and flagged. A covered cell is blank and clickable, while an uncovered cell is exposed. Flagged cells are those marked by the player to indicate a potential mine location.

A player left-clicks a cell to uncover it. If a player uncovers a mined cell, the game ends, as there is only 1 life per game. Otherwise, the uncovered cells displays either a number, indicating the quantity of mines diagonally or orthogonally adjacent to it, or a blank tile (or "0"), and all adjacent non-mined cells will automatically be uncovered. Right-clicking on a cell will flag it, causing a flag to appear on it. Flagged cells are still considered covered, and a player can click on them to uncover them, although typically they must first be unflagged with an additional right-click.

The first click in any game will never be a mine.[1]

To win the game, players must uncover all non-mine cells, at which point, the timer is stopped. Flagging all the mined cells is not required.

### Differences in gameplay

1. Currently there is nothing that prevents the first click from being a mine. You just have to hope for the best.

## Suggested grid sizes

In the original Minesweeper game by Microsoft the following grid sizes are used:
| Difficulty   | Rows  | Columns | Mines |
| :----------- | :---: | :-----: | :---: |
| Beginner     |   9   |    9    |  10   |
| Intermediate |  16   |   16    |  40   |
| Expert       |  30   |   16    |  99   |
