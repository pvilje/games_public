"""
@Name: Yatzy
@Author: Patvil
@Version: 1.0 
"""
import re
from random import randint

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
        self.dice = [0,0,0,0,0]
        self.player = []
        for player in range(0, nbr_players):
            self.player.append(Player(name="Player {}".format(player + 1)))
        self.throws = 0
        self.current_player = 0
        self.log_debug = False
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
        self.throws = 1
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
                self.log("Calling roll_dice")
                self.roll_dice()
                self.log("Calling draw dice")
                self.draw_dice()
                if self.throws < 3:
                    self.ask_keep_dice()
                self.throws += 1
            self.save_score()
            self.print_score()
            input("Press any key for next round:")

        # Game over
        print("\n")
        print("="*64)
        print("Game over, scores:")
        for player in self.player:
            print("{}: {} points".format(player.name, player.score["total"]))

    def ask_keep_dice(self):
        """
        Check what dice to keep
        """
        print("state dice numbers to keep, seperate with commas.")
        regex = r"[^1-5, ]|[1-5]{2,}"
        to_keep = "dummydata"
        while re.findall(regex, to_keep):
            to_keep = input("What dice to you want to keep? =>: ")
        self.keepers = 0b0
        self.log("will save:  {}".format(to_keep))
        if to_keep != "":
            for keeper in to_keep.split(","):
                self.keepers += 2 ** (int(keeper) - 1)
        # mask = 0b11111
        self.log("Keepers: {}".format(bin(self.keepers)))
        if self.keepers == 0b11111:
            self.throws = 3 
        # self.keepers = mask - self.keepers
        # self.log("Masked keepers: {}".format(bin(self.keepers)))
        

    def roll_dice(self):
        """
        Roll the dice
        """
        keep = self.keepers
        self.log("Will keep {}".format(keep))
        dice = 0
        keep_dice = 0b1
        while keep_dice <= 0b10000:
            self.log("Checking dice {}".format(dice + 1))
            if not keep_dice & keep:
                self.dice[dice] = randint(1, 6)
                self.log("Rerolling dice {}".format(dice + 1))
            else:
                self.log("Keeping dice {}".format(dice + 1))
            dice += 1
            keep_dice = keep_dice * 2
        self.log("Rolled dices {}".format(self.dice))
    
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
        result = ""
        for row in throw:
            result += "\n" + row
        result += " dice 1  dice 2  dice 3  dice 4  dice 5"
        print("You rolled:")
        print("{} Throw: {}".format(self.player[self.current_player - 1].name, self.throws))
        print(result)

    def save_score(self):
        """
        Save the score
        """
        self.evaluate_scores()
        self.select_score()
        self.player[self.current_player - 1].update_score()


    def log(self, msg):
        """
        Output debug info to consol.

        Args:
            msg (string): The string to print
        """
        if not self.log_debug:
            return
        print("Debug: {}".format(msg))

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
        # Print the results:
        self._print_score_line("(a)", "one", "One")
        self._print_score_line("(b)", "two", "Two")
        self._print_score_line("(c)", "three", "Three")
        self._print_score_line("(d)", "four", "Four")
        self._print_score_line("(e)", "five", "Five")
        self._print_score_line("(f)", "six", "Six")
        self._print_score_line("(g)", "pair", "Pair")
        self._print_score_line("(h)", "twopair", "two pair")
        self._print_score_line("(i)", "threesome", "3 of a kind")
        self._print_score_line("(j)", "foursome", "4 of a kind")
        self._print_score_line("(k)", "smallstraight", "Sm. Straight")
        self._print_score_line("(l)", "largestraight", "Lg. Straight")
        self._print_score_line("(m)", "fullhouse", "Full house")
        self._print_score_line("(n)", "chance", "Chance")
        self._print_score_line("(o)", "yatzy", "Yatzy")

        regex = r"[^a-o]|[a-o]{2,}"
        while not saved:
            save_score_as = "dummydata"
            while re.findall(regex, save_score_as):
                save_score_as = input("What to you want to save as? ")
                if save_score_as == "":
                    save_score_as = "dummydata"

            result = self.player[self.current_player - 1].save(
                self.option_table[save_score_as], 
                self.temp_scores[self.option_table[save_score_as]])
            if not result:
                saved = True
            else:
                print("option taken")
    
    def print_score(self):
        """
        Print the current scores!
        """
        print("Scoreboard for {}".format(self.player[self.current_player - 1].name))
        print("one\t\t{} {}".format(self.player[self.current_player - 1].score["one"], self.player[self.current_player - 1].saved_indicator["one"]))
        print("two\t\t{} {}".format(self.player[self.current_player - 1].score["two"], self.player[self.current_player - 1].saved_indicator["two"]))
        print("three\t\t{} {}".format(self.player[self.current_player - 1].score["three"], self.player[self.current_player - 1].saved_indicator["three"]))
        print("four\t\t{} {}".format(self.player[self.current_player - 1].score["four"], self.player[self.current_player - 1].saved_indicator["four"]))
        print("five\t\t{} {}".format(self.player[self.current_player - 1].score["five"], self.player[self.current_player - 1].saved_indicator["five"]))
        print("six\t\t{} {}".format(self.player[self.current_player - 1].score["six"], self.player[self.current_player - 1].saved_indicator["six"]))
        print("="*22)
        print("Sum A\t\t{}".format(self.player[self.current_player - 1].score["tot-a"]))
        print("Bonus A\t\t{}".format(self.player[self.current_player - 1].score["bonus-a"]))
        print("Total A\t\t{}".format(self.player[self.current_player - 1].score["total-a"]))
        print("="*22)
        print("Pair\t\t{} {}".format(self.player[self.current_player - 1].score["pair"], self.player[self.current_player - 1].saved_indicator["pair"]))
        print("Two Pair\t{} {}".format(self.player[self.current_player - 1].score["twopair"], self.player[self.current_player - 1].saved_indicator["twopair"]))
        print("3 of a kind\t{} {}".format(self.player[self.current_player - 1].score["threesome"], self.player[self.current_player - 1].saved_indicator["threesome"]))
        print("4 of a kind\t{} {}".format(self.player[self.current_player - 1].score["foursome"], self.player[self.current_player - 1].saved_indicator["foursome"]))
        print("Sm straight\t{} {}".format(self.player[self.current_player - 1].score["smallstraight"], self.player[self.current_player - 1].saved_indicator["smallstraight"]))
        print("Lg straight\t{} {}".format(self.player[self.current_player - 1].score["largestraight"], self.player[self.current_player - 1].saved_indicator["largestraight"]))
        print("Full house\t{} {}".format(self.player[self.current_player - 1].score["fullhouse"], self.player[self.current_player - 1].saved_indicator["fullhouse"]))
        print("Chance\t\t{} {}".format(self.player[self.current_player - 1].score["chance"], self.player[self.current_player - 1].saved_indicator["chance"]))
        print("Yatzy\t\t{} {}".format(self.player[self.current_player - 1].score["yatzy"], self.player[self.current_player - 1].saved_indicator["yatzy"]))
        print("="*22)
        print("Total B\t\t{}".format(self.player[self.current_player - 1].score["tot-b"]))
        print("Total\t\t{}".format(self.player[self.current_player - 1].score["total"]))
        print("="*22)
    
    def _print_score_line(self, letter, option, pretty_string=None):
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
            print ("{} {}:{}\t{} {}".format(letter, pretty_string, extra_tab, self.player[self.current_player - 1].score[option], self.player[self.current_player - 1].saved_indicator[option]))
        else:
            # show the calculated data.
            print ("{} {}:{}\t{}".format(letter, pretty_string, extra_tab, self.temp_scores[option]))

if __name__ == "__main__":
    yatzy = Yatzy(nbr_players=1)
    # yatzy.log_debug = True
    yatzy.new_round()
