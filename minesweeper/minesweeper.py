"""
@Name: minesweeper.py
@Author: pvilje
@Version: 1.0
"""


from random import randint
from math import ceil
class MineSweeper:
    """ 
    MineSweeper:

    chars used for internal gamelogic:
    . - not a mine
    x - mine
    - - opened cell (not a mine)
    1-8 - number of adjacent mines, hidden cell.
    1-8- - number of adjacent mines, opened cell.
    """
    def __init__(self):
        self.rows = 0
        self.columns = 0
        self.mines = 0
        self.boxes = []

        # game variables that needs to be reset each round
        self.cells_left = 0
        self.first_columns = []
        self.last_columns = []
        self.first_rows = []
        self.last_rows = []
    
    def NewGame(self, rows=9, columns=9, mines=10):
        """
        Start a new game

        Args:
            rows (int, optional): [Number of rows in the grid]. Defaults to 9.
            columns (int, optional): [Number of columns in the grid]. Defaults to 9.
            mines (int, optional): [Number of mines in the grid]. Defaults to 10.

        Returns:
            [type]: [description]
        """        
        try:
            self.rows = int(rows)
            self.columns = int(columns)
            self.mines = int(mines)
        except ValueError:
            return {
                "error-code": 1,
                "info": "invalid new game data supplied"
                }
        # Create a new array for mines
        self.boxes.clear()
        self.first_columns.clear()
        self.last_columns.clear()
        self.first_rows.clear()
        self.last_rows.clear()
    
        num_boxes = self.rows * self.columns
        for idx in range(0, num_boxes):
            self.boxes.append(".")

        # identify the edges
        idx = 0
        while idx < self.columns:
            self.first_rows.append(idx)
            idx += 1
        
        idx = len(self.boxes) - 1
        while idx > (len(self.boxes) - 1 - len(self.first_rows)):
            self.last_rows.append(idx)
            idx -= 1
        
        idx = 0
        while idx < len(self.boxes):
            self.first_columns.append(idx)
            idx += self.columns

        idx = self.columns - 1
        while idx < len(self.boxes):
            self.last_columns.append(idx)
            idx += self.columns

        # Place mines
        num_mines = self.mines
        while num_mines > 0:
            mine_spot = randint(0, len(self.boxes) - 1)
            if self.boxes[mine_spot] != "x":
                self.boxes[mine_spot] = "x"
                num_mines -= 1
        # Reset game variables
        self.cells_left = len(self.boxes) - self.mines

        # Place adjacent mine numbers
        for idx, cell in enumerate(self.boxes):
            def _increase_one(value):
                """
                Try to increase the value of an integer, ignore if it is a mine, set to 1 if it is the first time it is found.
                """                
                if value == "x":
                    return value
                try: 
                    value += 1
                except TypeError:
                    value = 1
                return value

            if cell == "x":

                # check if we are in the first column
                if idx in self.first_columns:
                    self.boxes[idx + 1] = _increase_one(self.boxes[idx + 1]) 
                    if idx in self.first_rows:
                        self.boxes[idx + self.columns] = _increase_one(self.boxes[idx + self.columns])
                        self.boxes[idx + self.columns + 1] = _increase_one(self.boxes[idx + self.columns + 1])
                    elif idx in self.last_rows:
                        self.boxes[idx - self.columns] = _increase_one(self.boxes[idx - self.columns])
                        self.boxes[idx - self.columns + 1] = _increase_one(self.boxes[idx - self.columns + 1])
                    else:
                        self.boxes[idx + self.columns] = _increase_one(self.boxes[idx + self.columns])
                        self.boxes[idx + self.columns + 1] = _increase_one(self.boxes[idx + self.columns + 1])
                        self.boxes[idx - self.columns] = _increase_one(self.boxes[idx - self.columns])
                        self.boxes[idx - self.columns + 1] = _increase_one(self.boxes[idx - self.columns + 1])
                
                # check if we are in the last column
                elif idx in self.last_columns:
                    self.boxes[idx - 1] = _increase_one(self.boxes[idx - 1]) 
                    if idx in self.first_rows:
                        self.boxes[idx + self.columns] = _increase_one(self.boxes[idx + self.columns])
                        self.boxes[idx + self.columns - 1] = _increase_one(self.boxes[idx + self.columns - 1])
                    elif idx in self.last_rows:
                        self.boxes[idx - self.columns] = _increase_one(self.boxes[idx - self.columns])
                        self.boxes[idx - self.columns - 1] = _increase_one(self.boxes[idx - self.columns - 1])
                    else:
                        self.boxes[idx + self.columns] = _increase_one(self.boxes[idx + self.columns])
                        self.boxes[idx + self.columns - 1] = _increase_one(self.boxes[idx + self.columns - 1])
                        self.boxes[idx - self.columns] = _increase_one(self.boxes[idx - self.columns])
                        self.boxes[idx - self.columns - 1] = _increase_one(self.boxes[idx - self.columns - 1])
               
                # check if we are in the first row
                elif idx in self.first_rows:
                    self.boxes[idx + self.columns] = _increase_one(self.boxes[idx + self.columns]) 
                    if idx in self.first_columns:
                        self.boxes[idx + 1] = _increase_one(self.boxes[idx + 1])
                        self.boxes[idx + self.columns + 1] = _increase_one(self.boxes[idx + self.columns + 1])
                    elif idx in self.last_columns:
                        self.boxes[idx - 1] = _increase_one(self.boxes[idx - 1])
                        self.boxes[idx + self.columns - 1] = _increase_one(self.boxes[idx + self.columns - 1])
                    else:
                        self.boxes[idx + 1] = _increase_one(self.boxes[idx + 1])
                        self.boxes[idx + self.columns + 1] = _increase_one(self.boxes[idx + self.columns + 1])
                        self.boxes[idx - 1] = _increase_one(self.boxes[idx - 1])
                        self.boxes[idx + self.columns - 1] = _increase_one(self.boxes[idx + self.columns - 1])
                
                # check if we are in the last row
                elif idx in self.last_rows:
                    self.boxes[idx - self.columns] = _increase_one(self.boxes[idx - self.columns]) 
                    if idx in self.first_columns:
                        self.boxes[idx + 1] = _increase_one(self.boxes[idx + 1])
                        self.boxes[idx - self.columns + 1] = _increase_one(self.boxes[idx - self.columns + 1])
                    elif idx in self.last_columns:
                        self.boxes[idx - 1] = _increase_one(self.boxes[idx - 1])
                        self.boxes[idx - self.columns - 1] = _increase_one(self.boxes[idx - self.columns - 1])
                    else:
                        self.boxes[idx + 1] = _increase_one(self.boxes[idx + 1])
                        self.boxes[idx - self.columns + 1] = _increase_one(self.boxes[idx - self.columns + 1])
                        self.boxes[idx - 1] = _increase_one(self.boxes[idx - 1])
                        self.boxes[idx - self.columns - 1] = _increase_one(self.boxes[idx - self.columns - 1])
                
                # We are somewhere in the middle
                else:
                    self.boxes[idx - 1] = _increase_one(self.boxes[idx - 1])
                    self.boxes[idx + 1] = _increase_one(self.boxes[idx + 1])
                    self.boxes[idx - self.columns] = _increase_one(self.boxes[idx - self.columns])
                    self.boxes[idx - self.columns + 1] = _increase_one(self.boxes[idx - self.columns + 1])
                    self.boxes[idx - self.columns - 1] = _increase_one(self.boxes[idx - self.columns - 1])
                    self.boxes[idx + self.columns] = _increase_one(self.boxes[idx + self.columns])
                    self.boxes[idx + self.columns + 1] = _increase_one(self.boxes[idx + self.columns + 1])
                    self.boxes[idx + self.columns - 1] = _increase_one(self.boxes[idx + self.columns - 1])

    def guess(self, button):
        """
        Evaluate the results of a click, is it a mine or not? let's find out! 

        Args:
            button (int): array index for the cell that was clicked

        Returns:
            [array_index]: array of array_indexes of cells that can be opened thanks to this click
            [text_array_index]: array of texts that for the cells that can be opened thanks to this click
        """        
        try:
            button = int(button)
        except ValueError:
            return {
                "error-code": 2,
                "info": "invalid guess data"
                }
        to_check = self.boxes[button] 
        
        result_buttons = []
        result_texts = []

        if to_check == ".": # not a mine
            result_buttons, result_texts = self._open_non_mines(button)
            nbr_results = list(set(result_buttons.copy()))
            self.cells_left -= len(nbr_results) 
                    
            return result_buttons, result_texts
        
        if to_check == "x":
            return [button], [to_check]
        
        else:
            self.cells_left -= 1
            self.boxes[button] = str(self.boxes[button]) + "-"
            return [button], [to_check]
        
    
    def _coordinate_to_array_index(self, x, y):
        """
        convert x, y coordinates to an array index

        Args:
            x (int): x coordinate
            y (int): y coordinate

        Returns:
            int: array index corresponding to the supplied x, y coordinate.
        """        
        return  (y - 1) * self.columns + x - 1 
    
    def _array_index_to_coordinate(self, idx):
        """
        Convert array index to x, y coordinates

        Args:
            idx (int): array index

        Returns:
            [int,int]: x, y coordinates corresponding to the supplied array index.
        """        
        x = (idx + 1) % self.columns
        if x == 0:
            x = self.columns
        
        y = ceil((idx + 1) / self.columns)
        return x, y

    def _open_non_mines(self, current):
        """
        Find all cells that can be opened after a "blank" cell has been found

        Args:
            current (int): the cell to start checking around.
        """        
        
        def _check_results(idx):
            """
            internal logic for evaluating individual cells.
            """            
            button = None
            text = None
            if self.boxes[idx] != "x":
                if self.boxes[idx] == ".":
                    button = idx
                    text = " "
                    self.boxes[idx] = "-"
                elif "-" not in str(self.boxes[idx]):
                    button = idx
                    text = self.boxes[idx]
                    self.boxes[idx] = str(self.boxes[idx]) + "-"

            return button, text
        
        result_buttons = []
        result_texts = []
        result_buttons.append(current)
        result_texts.append(" ")
        self.boxes[current] = "-"
        keep_checking = True
        while(keep_checking):
            board = self.boxes.copy()
            for idx, cell in enumerate(board):
                if cell == "-":
                    x, y = self._array_index_to_coordinate(idx)
                    # row above
                    if y-1 > 0:
                        current = self._coordinate_to_array_index(x, y-1)
                        button, text = _check_results(current)
                        if button is not None and text is not None:
                            result_buttons.append(button)
                            result_texts.append(text)

                        if x-1 > 0:
                            current = self._coordinate_to_array_index(x-1, y-1)
                            button, text = _check_results(current)
                            if button is not None and text is not None:
                                result_buttons.append(button)
                                result_texts.append(text)

                        if x+1 <= self.columns:
                            current = self._coordinate_to_array_index(x+1, y-1)
                            button, text = _check_results(current)
                            if button is not None and text is not None:
                                result_buttons.append(button)
                                result_texts.append(text)
                            
                    # row below
                    if y+1 <= self.rows:
                        current = self._coordinate_to_array_index(x, y+1)
                        button, text = _check_results(current)
                        if button is not None and text is not None:
                            result_buttons.append(button)
                            result_texts.append(text)
                        
                        if x-1 > 0:
                            current = self._coordinate_to_array_index(x-1, y+1)
                            button, text = _check_results(current)
                            if button is not None and text is not None:
                                result_buttons.append(button)
                                result_texts.append(text)

                        if x+1 <= self.columns:
                            current = self._coordinate_to_array_index(x+1, y+1)
                            button, text = _check_results(current)
                            if button is not None and text is not None:
                                result_buttons.append(button)
                                result_texts.append(text)

                    # column left
                    if x-1 > 0:
                        current = self._coordinate_to_array_index(x-1, y)
                        button, text = _check_results(current)
                        if button is not None and text is not None:
                            result_buttons.append(button)
                            result_texts.append(text)

                    # column right
                    if x+1 <= self.columns:
                        current = self._coordinate_to_array_index(x+1, y)
                        button, text = _check_results(current)
                        if button is not None and text is not None:
                            result_buttons.append(button)
                            result_texts.append(text)

            if board == self.boxes:
                keep_checking = False
        return result_buttons, result_texts
