"""
@Name: Hangman
@Author: pvilje
@Version: 1.0
"""

import re
import requests

class Hangman:
    """
    Simple Hangman command prompt game
    """

    def __init__(self) -> None:
        """
        hangman command prompt game
        """
        self.word = ""
        self.letters_to_find = []
        self.guesses_correct = []
        self.guesses_wrong = []
        self.num_wrong_guesses = 0
        self.guess_max = 10
        self.man = ["","","","","",""]
        self.cheat = False  # Indended for development only... But I'm not judging anyone :)
        self.url = ""

    def _select_source(self):
        """
        Select the source for the word
        """
        welcome_string = [
            "Welcome to hangman",
            "",
            "Select the source for the word to guess:",
            "a) Manual input",
            "b) Random English wikipedia article name",
            "c) Random Swedish wikipedia article name",
        ]

        for line in welcome_string:
            print(line)

        choice_made = False
        regex = r"[^a-c]"
        while not choice_made:
            source = input("Please select your source: ")
            if not re.findall(regex, source):  # only letters in range was presented.
                if len(source) == 1:  # only one letter was input
                    choice_made = True
        # Manual input
        if source == "a":
            while len(self.word) < 1:
                self.word = input("Please enter the word to use: ")
            self.url = ""

        # Wikipedia
        if source == "b":
            self.word = self._get_wikipedia_article("https://en.wikipedia.org/wiki/Special:Random")
        
        if source == "c":
            self.word = self._get_wikipedia_article("https://sv.wikipedia.org/wiki/Special:Slumpsida")


    def _get_wikipedia_article(self, url):
        """
        Helper function to get a wikipedia article

        Args:
            url (string): The url to visit
        """
        article = requests.get(url)
        while "%" in article.url:  # We don't want any weird character url encodings...
            print(article.url)
            article = requests.get(url)
        if self.cheat:
            print(article.url)
        self.url = article.url
        return article.url.split("wiki/")[1].replace("_", " ")


    def new_game(self):
        """
        Start a new game
        """
        self._select_source()
        if self.word == "":
            print("No word, cannot start a new game")
            return
        else:
            regex = r"[^\d\wåäö]"
            for letter in self.word:
                if letter.upper() not in self.letters_to_find and not re.findall(regex, letter):
                    self.letters_to_find.append(letter.upper())
        self._draw_wordhints()
        self._draw_hangman()

        while len(self.letters_to_find) != len(self.guesses_correct):
            if self.cheat:
                to_find = ""
                for letter in self.letters_to_find:
                    if letter.upper() not in self.guesses_correct:
                        to_find += "'{}' ".format(letter)
                print("Letters still to find: {}".format(to_find))
            guess = input("Guess a letter: ")
            if len(guess) == 1:
                if guess.upper() not in self.guesses_wrong and guess.upper() not in self.guesses_correct:
                    self._evaluate_guess(guess)
                    self._draw_wordhints()
                    self._draw_hangman()
                else:
                    print("You have already guessed that.")
            if self.num_wrong_guesses >= self.guess_max:
                print("GAME OVER")
                print("The correct word was: {}".format(self.word))
                if len(self.url) > 0:
                    print("Want to know more? check out {}".format(self.url))
                return
        print("Congratulations! you found the word: {}".format(self.word))
        if len(self.url) > 0:
            print("Want to know more? check out {}".format(self.url))


    
    def _evaluate_guess(self, guess):
        """
        Evalutate the current guess

        Args:
            guess (string): The guessed character
        """
        if guess.lower() in self.word.lower():
            self.guesses_correct.append(guess.upper())
            print("\n\n{} is a correct guess! ".format(guess))
        else:
            self.guesses_wrong.append(guess.upper())
            self.num_wrong_guesses += 1
            print("\n\n{} is unfortunately not a correct guess! ".format(guess))

    def _draw_wordhints(self):
        """
        Draw the word hints (underscores or already correct chars)
        """
        hint_string = ""
        regex = r"[^\d\wåäö]"
        for letter in self.word:
            if letter.upper() in self.guesses_correct or re.findall(regex, letter):
                hint_string += letter
            else:
                hint_string += "_"
        print ("\n Your word so far: {}".format(hint_string))

    def _draw_hangman(self):
        """
        Draw the image of the hanged man.
        [
            "     ______    ", # 0
            "    |      |   ", # 1
            "    |      O   ", # 2
            "    |     /|\  ", # 3
            "   _|_    / \  ", # 4
            " /     \       "] # 5
        """
        if self.num_wrong_guesses == 0:
            self.man = [
                "               ", # 0
                "               ", # 1
                "               ", # 2
                "               ", # 3
                "               ", # 4
                "               "] # 5
        if self.num_wrong_guesses == 1:
            self.man[4] = "   _ _         "
            self.man[5] = " /     \       "
        
        if self.num_wrong_guesses == 2:
            self.man[1] = "    |          "
            self.man[2] = "    |          "
            self.man[3] = "    |          "
            self.man[4] = "   _|_         "
        
        if self.num_wrong_guesses == 3:
            self.man[0] = "     ______    "
        
        if self.num_wrong_guesses == 4:
            self.man[1] = "    |      |   "
        
        if self.num_wrong_guesses == 5:
            self.man[2] = "    |      O   "
        
        if self.num_wrong_guesses == 6:
            self.man[3] = "    |      |   "
        
        if self.num_wrong_guesses == 7:
            self.man[4] = "   _|_    /    "
        
        if self.num_wrong_guesses == 8:
            self.man[4] = "   _|_    / \  "
        
        if self.num_wrong_guesses == 9:
            self.man[3] = "    |     /|   "
        
        if self.num_wrong_guesses == 10:
            self.man[3] = "    |     /|\  "
        
        print("\n")
        for line in self.man:
            print(line)
        
        guesses = ""
        for guess in self.guesses_wrong:
            guesses += "{} ".format(guess)
        print(guesses)



if __name__ == "__main__":
    game = Hangman()
    # game.cheat = True  # Do not enable this, would make it boring :) 
    game.new_game()