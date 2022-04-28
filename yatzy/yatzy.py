"""
@Name: Yatzy
@Author: pvilje
@Version: 1.1 
"""
import re
from random import randint
import curses

class Player:
    """
    A player class to keep track of scores.
    """
    def __init__(self, name="Player"):

        self.name = name
        self.all_done = False
        
        self.score = {
            "one": 0,
            "two": 0,
            "three": 0,
            "four": 0,
            "five": 0,
            "six": 0,
            "tot-a": 0,
            "bonus-a": 0,
            "total-a": 0,
            "pair": 0,
            "twopair": 0,
            "threesome": 0,
            "foursome": 0,
            "smallstraight": 0,
            "largestraight": 0,
            "fullhouse": 0,
            "chance": 0,
            "yatzy": 0,
            "tot-b": 0,
            "total": 0
        }

        self.saved = {
            "one": False,
            "two": False,
            "three": False,
            "four": False,
            "five": False,
            "six": False,
            "pair": False,
            "twopair": False,
            "threesome": False,
            "foursome": False,
            "smallstraight": False,
            "largestraight": False,
            "fullhouse": False,
            "chance": False,
            "yatzy": False
        }

        self.saved_indicator = {
            "one": "",
            "two": "",
            "three": "",
            "four": "",
            "five": "",
            "six": "",
            "pair": "",
            "twopair": "",
            "threesome": "",
            "foursome": "",
            "smallstraight": "",
            "largestraight": "",
            "fullhouse": "",
            "chance": "",
            "yatzy": ""
        }
    
    def save(self, option, value):
        """
        Save the score.

        Args:
            option (string): What to save
            value (int): the score
        """
        sum = int(value)
        if not self.saved[option]:
            self.saved[option] = True
            self.saved_indicator[option] = "*"
            self.score[option] = value
            return 0
        return 1
    
    def update_score(self):
        """
        Go through all the scores and update to totals
        """
        # Upper section
        self.score["tot-a"] = self.score["one"] + self.score["two"] + self.score["three"] + self.score["four"] + self.score["five"] + self.score["six"]
        if self.score["tot-a"] >= 63:
            self.score["bonus-a"] = 50
        self.score["total-a"] = self.score["tot-a"] + self.score["bonus-a"]
        
        # Lower section
        self.score["tot-b"] = self.score["pair"] + self.score["twopair"] + self.score["threesome"] + self.score["foursome"] + self.score["smallstraight"] + self.score["largestraight"] + self.score["fullhouse"] + self.score["chance"] + self.score["yatzy"]
        
        self.score["total"] = self.score["total-a"] + self.score["tot-b"]
        
        all_are_done = True
        for option in self.saved:
            if not self.saved[option]:
                all_are_done = False
        self.all_done = all_are_done


class Yatzy:
    """
    The game class
    """
    def __init__(self, nbr_players=1):

        # Setup curses
        self.stdscr = curses.initscr()
        max_y, max_x = self.stdscr.getmaxyx()
        begin_x = max_x-23 
        begin_y = 0
        height = 25; width = 23
        self.scorescr = curses.newwin(height, width, begin_y, begin_x)
        begin_x = 0
        begin_y = 0
        height = 30; width = 50
        self.gamescr = curses.newwin(height, width, begin_y, begin_x)

        self.dice = [0,0,0,0,0]
        self.player = []
        for player in range(0, nbr_players):
            self.player.append(Player(name="Player {}".format(player + 1)))
        self.throw = ""
        self.throws = 0
        self.current_player = 0
        self.keepers = 0b0  # Used as a bitmask to see which dices to keep

        self.option_table = {
            "a": "one",
            "b": "two",
            "c": "three",
            "d": "four",
            "e": "five",
            "f": "six",
            "g": "pair",
            "h": "twopair",
            "i": "threesome",
            "j": "foursome",
            "k": "smallstraight",
            "l": "largestraight",
            "m": "fullhouse",
            "n": "chance",
            "o": "yatzy"
        }
    
    def reset_round(self):
        """
        Reset all round variables
        """
        self.current_player += 1
        if self.current_player > len(self.player):
            self.current_player = 1
        self.throws = 0
        self.dice = [0,0,0,0,0]
        self.keepers = 0

    def new_round(self):
        """
        Start a round:
        """
        while not self.player[len(self.player) - 1].all_done:
            self.reset_round()
            self.print_score()
            while self.throws <= 3:
                self.throws += 1
                self.roll_dice()
                self.draw_dice()
                if self.throws < 3:
                    self.ask_keep_dice()
            self.save_score()
            self.print_score()
        self.game_over()
    
    def game_over(self):
        # Game over
        # restore curses
        curses.endwin()

        print("Game over, scores:")
        for player in self.player:
            print(f"{player.name}: {player.score['total']} points")
        quit()

    def ask_keep_dice(self):
        """
        Check what dice to keep
        """
        self.gamescr.addstr(17,0,"state dice numbers to keep, seperate with commas.")
        regex = r"[^1-5, ]|[1-5]{2,}"
        to_keep = "dummydata"
        # are we quitting? 
        while re.findall(regex, to_keep):
            msg = "What dice to you want to keep? =>: "
            self.gamescr.addstr(16,0,msg)
            to_keep = self.gamescr.getstr(16,len(msg)).decode(encoding="utf-8")
            self._refresh_game()
            if to_keep == "q" or to_keep == "quit":
                for idx in range(0, len(self.player)): self.player[idx].all_done = True
                self.game_over()
        self.keepers = 0b0
        if to_keep != "":
            for keeper in to_keep.split(","):
                self.keepers += 2 ** (int(keeper) - 1)
        # mask = 0b11111
        if self.keepers == 0b11111:
            self.throws = 3 

    def roll_dice(self):
        """
        Roll the dice
        """
        keep = self.keepers
        dice = 0
        keep_dice = 0b1
        while keep_dice <= 0b10000:
            if not keep_dice & keep:
                self.dice[dice] = randint(1, 6)
            dice += 1
            keep_dice = keep_dice * 2
    
    def draw_dice(self):
        """
        Print the dice
        """
        dice = [
            [" ----- ", "|     |", "|  x  |", "|     |", " ----- "],
            [" ----- ", "|    x|", "|     |", "|x    |", " ----- "],
            [" ----- ", "|    x|", "|  x  |", "|x    |", " ----- "],
            [" ----- ", "|x   x|", "|     |", "|x   x|", " ----- "],
            [" ----- ", "|x   x|", "|  x  |", "|x   x|", " ----- "],
            [" ----- ", "|x   x|", "|x   x|", "|x   x|", " ----- "],
            ]
        space = " "
        
        throw = ["", "", "", "", "", ""]
        for single_dice in self.dice:
            for row in range(0, 5):
                throw[row] += dice[single_dice - 1][row] + space
        self.throw = ""
        for row in throw:
            self.throw += "\n" + row
        self.throw += " dice 1  dice 2  dice 3  dice 4  dice 5"
        self.gamescr.clear()
        self.gamescr.addstr(5,0,f"{self.player[self.current_player - 1].name} Throw: {self.throws}")
        self.gamescr.addstr(6,0,"You rolled:")
        self.draw_throw(7)

    def draw_throw(self, row):
        """
        Draw only the dices
        takes 6 lines
        """
        self.gamescr.addstr(row,0,self.throw)

    def save_score(self):
        """
        Save the score
        """
        self.evaluate_scores()
        self.select_score()
        self.player[self.current_player - 1].update_score()

    def evaluate_scores(self):
        """
        Evaluate the throw and see what scores it generates.
        """
        self.temp_scores = {
            "one": 0,
            "two": 0,
            "three": 0,
            "four": 0,
            "five": 0,
            "six": 0,
            "pair": 0,
            "twopair": 0,
            "threesome": 0,
            "foursome": 0,
            "smallstraight": 0,
            "largestraight": 0,
            "fullhouse": 0,
            "chance": 0,
            "yatzy": 0
        }
        # Ones to sixes
        self.temp_scores["one"] = self.dice.count(1) * 1
        self.temp_scores["two"] = self.dice.count(2) * 2
        self.temp_scores["three"] = self.dice.count(3) * 3
        self.temp_scores["four"] = self.dice.count(4) * 4
        self.temp_scores["five"] = self.dice.count(5) * 5
        self.temp_scores["six"] = self.dice.count(6) * 6

        # Pairs -> yatzy.
        for value in [1, 2, 3, 4, 5, 6]:
            if self.dice.count(value) >= 2:
                self.temp_scores["pair"] = value * 2
            if self.dice.count(value) >= 3:
                self.temp_scores["threesome"] = value * 3
            if self.dice.count(value) >= 4:
                self.temp_scores["foursome"] = value * 4
            if self.dice.count(value) == 5:
                self.temp_scores["yatzy"] = 50
        
        # Chance
        for dice in self.dice:
            self.temp_scores["chance"] += dice
        
        sorted_dice = self.dice.copy()
        sorted_dice.sort()

        # Two pair
        pair1 = 0
        pair2 = 0
        for idx in range(6, 0 , -1):
            if self.dice.count(idx) >= 2:
                if pair1 == 0:
                    pair1 = idx*2
                else:
                    pair2 = idx*2
        if (pair1 != 0) and (pair2 != 0):
            self.temp_scores["twopair"] = pair1 + pair2

        # Straights
        if (
            (sorted_dice[0] == sorted_dice[1] - 1) and
            (sorted_dice[1] == sorted_dice[2] - 1) and
            (sorted_dice[2] == sorted_dice[3] - 1) and
            (sorted_dice[3] == sorted_dice[4] - 1)):
            if sorted_dice[0] == 1:
                self.temp_scores["smallstraight"] = 15
            if sorted_dice[0] == 2:
                self.temp_scores["largestraight"] = 20

        # Full house
        if (
            (sorted_dice[0] != sorted_dice[4]) and
            (
                (sorted_dice[0] == sorted_dice[1] == sorted_dice[2] and sorted_dice[3] == sorted_dice[4]) or
                (sorted_dice[0] == sorted_dice[1] and sorted_dice[2] == sorted_dice[3] == sorted_dice[4]))):
                for dice in sorted_dice:
                    self.temp_scores["fullhouse"] += dice

    def select_score(self):
        """
        Select what to save as
        """
        saved = False
        regex = r"[^a-o]|[a-o]{2,}"
        while not saved:
            self.gamescr.clear()
            self.draw_throw(1)
            self._print_score_line(10,"(a)", "one", "One")
            self._print_score_line(11,"(b)", "two", "Two")
            self._print_score_line(12,"(c)", "three", "Three")
            self._print_score_line(13,"(d)", "four", "Four")
            self._print_score_line(14,"(e)", "five", "Five")
            self._print_score_line(15,"(f)", "six", "Six")
            self._print_score_line(16,"(g)", "pair", "Pair")
            self._print_score_line(17,"(h)", "twopair", "two pair")
            self._print_score_line(18,"(i)", "threesome", "3 of a kind")
            self._print_score_line(19,"(j)", "foursome", "4 of a kind")
            self._print_score_line(20,"(k)", "smallstraight", "Sm. Straight")
            self._print_score_line(21,"(l)", "largestraight", "Lg. Straight")
            self._print_score_line(22,"(m)", "fullhouse", "Full house")
            self._print_score_line(23,"(n)", "chance", "Chance")
            self._print_score_line(24,"(o)", "yatzy", "Yatzy")
            msg = "What to you want to save as?"
            self.gamescr.addstr(28,0, msg)
            save_score_as = "dummydata"
            while re.findall(regex, save_score_as):
                save_score_as = self.gamescr.getstr(28, len(msg)).decode(encoding="utf-8")
                self._refresh_game()
                if save_score_as == "":
                    save_score_as = "dummydata"

            result = self.player[self.current_player - 1].save(
                self.option_table[save_score_as], 
                self.temp_scores[self.option_table[save_score_as]])
            if not result:
                saved = True
    
    def print_score(self):
        """
        Print the current scores!
        """
        self.scorescr.clear()
        self.scorescr.addstr(0,0,f"Scoreboard for {self.player[self.current_player - 1].name}")
        self.scorescr.addstr(1,0,f"one\t\t{self.player[self.current_player - 1].score['one']} {self.player[self.current_player - 1].saved_indicator['one']}")
        self.scorescr.addstr(2,0,f"two\t\t{self.player[self.current_player - 1].score['two']} {self.player[self.current_player - 1].saved_indicator['two']}")
        self.scorescr.addstr(3,0,f"three\t\t{self.player[self.current_player - 1].score['three']} {self.player[self.current_player - 1].saved_indicator['three']}")
        self.scorescr.addstr(4,0,f"four\t\t{self.player[self.current_player - 1].score['four']} {self.player[self.current_player - 1].saved_indicator['four']}")
        self.scorescr.addstr(5,0,f"five\t\t{self.player[self.current_player - 1].score['five']} {self.player[self.current_player - 1].saved_indicator['five']}")
        self.scorescr.addstr(6,0,f"six\t\t{self.player[self.current_player - 1].score['six']} {self.player[self.current_player - 1].saved_indicator['six']}")
        self.scorescr.addstr(7,0,"="*22)
        self.scorescr.addstr(8,0,f"Sum A\t\t{self.player[self.current_player - 1].score['tot-a']}")
        self.scorescr.addstr(9,0,f"Bonus A\t\t{self.player[self.current_player - 1].score['bonus-a']}")
        self.scorescr.addstr(10,0,f"Total A\t\t{self.player[self.current_player - 1].score['total-a']}")
        self.scorescr.addstr(11,0,"="*22)
        self.scorescr.addstr(12,0,f"Pair\t\t{self.player[self.current_player - 1].score['pair']} {self.player[self.current_player - 1].saved_indicator['pair']}")
        self.scorescr.addstr(13,0,f"Two Pair\t{self.player[self.current_player - 1].score['twopair']} {self.player[self.current_player - 1].saved_indicator['twopair']}")
        self.scorescr.addstr(14,0,f"3 of a kind\t{self.player[self.current_player - 1].score['threesome']} {self.player[self.current_player - 1].saved_indicator['threesome']}")
        self.scorescr.addstr(15,0,f"4 of a kind\t{self.player[self.current_player - 1].score['foursome']} {self.player[self.current_player - 1].saved_indicator['foursome']}")
        self.scorescr.addstr(16,0,f"Sm straight\t{self.player[self.current_player - 1].score['smallstraight']} {self.player[self.current_player - 1].saved_indicator['smallstraight']}")
        self.scorescr.addstr(17,0,f"Lg straight\t{self.player[self.current_player - 1].score['largestraight']} {self.player[self.current_player - 1].saved_indicator['largestraight']}")
        self.scorescr.addstr(18,0,f"Full house\t{self.player[self.current_player - 1].score['fullhouse']} {self.player[self.current_player - 1].saved_indicator['fullhouse']}")
        self.scorescr.addstr(19,0,f"Chance\t\t{self.player[self.current_player - 1].score['chance']} {self.player[self.current_player - 1].saved_indicator['chance']}")
        self.scorescr.addstr(20,0,f"Yatzy\t\t{self.player[self.current_player - 1].score['yatzy']} {self.player[self.current_player - 1].saved_indicator['yatzy']}")
        self.scorescr.addstr(21,0,"="*22)
        self.scorescr.addstr(22,0,f"Total B\t\t{self.player[self.current_player - 1].score['tot-b']}")
        self.scorescr.addstr(23,0,f"Total\t\t{self.player[self.current_player - 1].score['total']}")
        self.scorescr.addstr(24,0,"="*22)
        self.scorescr.refresh()
    
    def _print_score_line(self, row:int, letter, option, pretty_string=None):
        """
        Helper function to print out the options to save and their scores

        Args:
            letter (string): the char to identify the option (used as input)
            option (string): identifier to the player score list
            pretty_string (string, optional): Reader friendly option string. Defaults to None.
        """                
        if pretty_string is None:
            pretty_string = option
        if len(pretty_string) <= 10:
            extra_tab = "\t"
        else:
            extra_tab = ""

        if self.player[self.current_player - 1].saved[option]:
            # show the saved data
            self.gamescr.addstr(row, 0, f"{letter} {pretty_string}:{extra_tab}\t{self.player[self.current_player - 1].score[option]} {self.player[self.current_player - 1].saved_indicator[option]}")
        else:
            # show the calculated data.
            self.gamescr.addstr(row, 0, f"{letter} {pretty_string}:{extra_tab}\t{self.temp_scores[option]}")
    
    def _refresh_game(self):
        self.gamescr.refresh()

def main():
    yatzy = Yatzy(nbr_players=1)
    yatzy.new_round()

if __name__ == "__main__":
    main()
