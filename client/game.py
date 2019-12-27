from enum import Enum
from connection import Connection

class Client:
    """Class responsible for maintaining client state, creating connection and game"""

    def __init__(self, view):
        self.connection = Connection("127.0.0.1", 1234, self)
        self.view = view
        self.game = Game(19, self.view)
        self.game.player = Field.BLACK

    def move(self, row, column):
        self.game.move_player(row, column)
        self.connection.send("MOVE " + str(row) + " " + str(column) + "\n")

    def new_game(self):
        self.game = Game(19, self.view)
        self.game.player = Field.BLACK
        self.connection.send("NEW GAME\n")

class Game:
    """Class responsible for maintaining a game state"""

    def __init__(self, board_size, view):
        self.board_size = board_size
        self.board = [[Field.NONE for i in range(board_size)] for i in range(board_size)]
        self.player = Field.NONE
        self.view = view
        self.view.update(self)

    def move_player(self, row, column):
        self.board[row][column] = self.player
        self.view.update(self)

    def move_enemy(self, row, column):
        self.board[row][column] = self.player.other()
        self.view.update(self)

class Field(Enum):
    """Enum class representing field on the board"""
    NONE = 0
    WHITE = 1
    BLACK = 2

    def other(self):
        if self == Field.WHITE:
            return Field.BLACK
        if self == Field.BLACK:
            return Field.WHITE
        return Field.NONE
