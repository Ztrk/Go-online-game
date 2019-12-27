from enum import Enum
from connection import Connection

class Game:
    def __init__(self, view):
        self.connection = Connection("127.0.0.1", 1234, self)
        self.view = view
        self.board = Board(19, self.view)
        self.player = Field.BLACK

    def move(self, row, column):
        self.board.move(row, column, self.player)
        self.connection.send("MOVE " + str(row) + " " + str(column) + "\n")

    def new_game(self):
        self.connection.send("NEW GAME\n")
        self.board = Board(19, self.view)

class Board:
    """Class responsible for maintaining a board state"""

    def __init__(self, board_size, view):
        self.board_size = board_size
        self.board = [[Field.NONE for i in range(board_size)] for i in range(board_size)]
        self.view = view
        self.view.update(self)

    def move(self, row, column, new_field):
        self.board[row][column] = new_field
        self.view.update(self)

class Field(Enum):
    """Enum class representing field on the board"""
    NONE = 0
    WHITE = 1
    BLACK = 2
