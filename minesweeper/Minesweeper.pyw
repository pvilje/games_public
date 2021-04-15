"""
@Name: Minesweeper.pyw
@Author: pvilje
@Version: 0.9
"""

import tkinter as tk
from tkinter.constants import DISABLED
from typing import Text
from minesweeper import MineSweeper


class Game(tk.Frame):

    def __init__(self, my_parent):
        """
        :param my_parent:
        :return:
        """
        tk.Frame.__init__(self, my_parent)
        self.game = MineSweeper()
        self._root = my_parent
        # TK inter stuff
        #############################

        # Initialize window frames
        self.f_menulist = tk.Frame(self._root, pady=5)
        self.f_header = tk.Frame(self._root, pady=5)
        self.f_game = tk.Frame(self._root, pady=5)
        
        # Variables
        self.iv_num_rows = tk.IntVar()
        self.iv_num_columns = tk.IntVar()
        self.iv_num_mines = tk.IntVar()
        self.iv_mines_left = tk.IntVar()
        self.sv_timer = tk.StringVar()
        self.sv_timer.set("00:00")
        
        # Buttons
        self.b_new_game = tk.Button(self.f_header, text="New Game", command=self.New_game)
        self.b_beginner = tk.Button(self.f_menulist, text="Beginner", command=self._set_beginner)
        self.b_intermediate = tk.Button(self.f_menulist, text="intermediate", command=self._set_intermediate)
        self.b_expert = tk.Button(self.f_menulist, text="Expert", command=self._set_expert)

        # Labels
        self.l_grid_width = tk.Label(self.f_header, text="Width")
        self.l_grid_height = tk.Label(self.f_header, text="Height")
        self.l_grid_mines = tk.Label(self.f_header, text="Mines")
        self.l_mines_left = tk.Label(self.f_header, text="Mines left")
        
        # Input fields
        self.e_grid_width = tk.Entry(self.f_header, textvariable=self.iv_num_columns, width=3) 
        self.e_grid_height = tk.Entry(self.f_header, textvariable=self.iv_num_rows, width=3) 
        self.e_grid_mines = tk.Entry(self.f_header, textvariable=self.iv_num_mines, width=3)
        self.e_mines_left = tk.Entry(self.f_header, textvariable=self.iv_mines_left, width=3, state=DISABLED, disabledbackground="black", disabledforeground="#00ff00")
        self.e_timer = tk.Entry(self.f_header, textvariable=self.sv_timer, width=5, state=DISABLED, disabledbackground="black", disabledforeground="#00ff00")

        """
        DRAW ALL THE CONTENTS
        """
        # Main frames
        self.f_menulist.grid(row=0, column=0)
        self.f_header.grid(row=1, column=0)
        self.f_game.grid(row=2, column=0)
        
        self.b_beginner.grid(row=0, column=0)
        self.b_intermediate.grid(row=0, column=1)
        self.b_expert.grid(row=0, column=2)
        self.l_grid_width.grid(row=0, column=0)
        self.e_grid_width.grid(row=0, column=1)
        self.l_grid_height.grid(row=0, column=2)
        self.e_grid_height.grid(row=0, column=3)
        self.l_grid_mines.grid(row=0, column=4)
        self.e_grid_mines.grid(row=0, column=5)
        self.b_new_game.grid(row=0, column=6)
        self.e_timer.grid(row=0, column=7, padx=10)
        self.l_mines_left.grid(row=0, column=8)
        self.e_mines_left.grid(row=0, column=9)

        self.mines = []

        # Default vales
        self.iv_num_columns.set(9)
        self.iv_num_rows.set(9)
        self.iv_num_mines.set(10)
        self._default_button_color = ""
        self._after_id = ""

    def _set_beginner(self):
        """
        Setup the game to beginner defaults.
        """        
        self.iv_num_rows.set(9)
        self.iv_num_columns.set(9)
        self.iv_num_mines.set(10)
    
    def _set_intermediate(self):
        """
        Setup the game to intermediate defaults.
        """        
        self.iv_num_rows.set(16)
        self.iv_num_columns.set(16)
        self.iv_num_mines.set(40)

    def _set_expert(self):
        """
        Setup the game to expert defaults.
        """        
        self.iv_num_rows.set(16)
        self.iv_num_columns.set(30)
        self.iv_num_mines.set(99)

    def timer(self):
        """
        Update the time spent
        """        

        current = self.sv_timer.get()
        minutes, seconds = current.split(":")
        seconds = int(seconds) + 1
        minutes = int(minutes)
        if seconds >= 60:
            seconds -= 60
            minutes += 1
        minutes = "{:02d}".format(minutes)
        seconds = "{:02d}".format(seconds)
        self.sv_timer.set("{}:{}".format(minutes, seconds))
        self._after_id = self.after(1000, self.timer)
        
    def New_game(self):
        """
        Start a new game
        """
        if len(self._after_id) > 0:
            self.after_cancel(self._after_id)
        self.sv_timer.set("00:00")
        self.after(1000, self.timer)
        
        # self.timer()
        self.f_game.grid_forget()
        self.f_game.destroy()
        self.f_game = tk.Frame(self._root, pady=5)
        self.f_game.grid(row=2, column=0)
        try:
            width = int(self.iv_num_columns.get())
            height = int(self.iv_num_rows.get())
            mines = int(self.iv_num_mines.get())
            self.e_mines_left["state"] = "normal"
            self.iv_mines_left.set(mines)
            self.e_mines_left["state"] = "disabled"
            self.game.NewGame(rows=height, columns=width, mines=mines)
            self.draw_mines()

        except tk._tkinter.TclError:
            pass

    def draw_mines(self):
        """
        Draw all the cells! 
        """        
        def left_click(cell_index):
            """
            Functionality when leftclicking a button

            Args:
                cell_index (int): the cell that was clicked
            """            
            result, text = self.game.guess(cell_index)
            # See if the game is complete!
            if self.game.cells_left == 0:
                self.after_cancel(self._after_id)
                for item in self.f_game.grid_slaves():
                    if type(item) == tk.Button:
                        item["background"] = "green"
                        item["state"] = "disabled"
                self.iv_mines_left.set(0)

            # See if it's game over!
            if len(text) == 1 and text[0] == "x":
                self.after_cancel(self._after_id)
                posx = self.mines[result[0]].grid_info()["row"]
                posy = self.mines[result[0]].grid_info()["column"]
                self.mines[result[0]].grid_forget()
                self.mines[result[0]] = tk.Label(self.f_game, text="X", width=3, bg="#ff0000").grid(row=posx, column=posy, padx=3, pady=3)
                for button in self.f_game.grid_slaves():
                    if type(button) == tk.Button:
                        button["state"] = "disabled"
            else:
                for text_index, idx in enumerate(result):
                    if self.mines[idx] is not None:
                        posx = self.mines[idx].grid_info()["row"]
                        posy = self.mines[idx].grid_info()["column"]
                        self.mines[idx].grid_forget()
                        self.mines[idx] = tk.Label(self.f_game, text=text[text_index], width=3).grid(row=posx, column=posy, padx=3, pady=3)
        
        def right_click(cell_index):
            """
            Functionality when right-clicking a button

            Args:
                cell_index (int): The cell that was clicked
            """            
            if self.mines[cell_index]["state"] == "disabled":
                self.mines[cell_index].configure(bg=self._default_button_color)    
                self.mines[cell_index]["state"] = "normal"
                self.e_mines_left["state"] = "normal"
                self.iv_mines_left.set(self.iv_mines_left.get() + 1)
                self.e_mines_left["state"] = "disabled"
            else:
                self._default_button_color = self.mines[cell_index]["bg"]
                self.mines[cell_index]["state"] = "disabled"
                self.mines[cell_index].configure(bg="#ff9999")
                self.e_mines_left["state"] = "normal"
                self.iv_mines_left.set(self.iv_mines_left.get() - 1)
                self.e_mines_left["state"] = "disabled"

        
        self.mines.clear()
        for idx, mine in enumerate(self.game.boxes):
            button = tk.Button(self.f_game, width=3, command=lambda i=idx: left_click(i))   
            button.bind("<Button-2>", lambda e, i = idx: right_click(i))  # Todo: middle click counts as right, for now
            button.bind("<Button-3>", lambda e, i = idx: right_click(i))
            self.mines.append(button)
        
        Row = 0
        Col = 0
        for idx, cell in enumerate(self.mines, start=1):
            cell.grid(row=Row, column=Col, padx=1, pady=1)
            if idx % (self.game.columns) != 0:
                Col += 1
            else:
                Row += 1
                Col = 0


root = tk.Tk()
root.title("MineSweeper clone")
game = Game(root)
root.mainloop()
