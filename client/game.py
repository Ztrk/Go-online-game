from enum import Enum
from connection import Connection

class Game:
    def __init__(self, view):
        self.connection = Connection("127.0.0.1", 1234)
        self.view = view
        self.board = Board(19)
        self.player = Field.BLACK
    
    def move(self, row, column):
        self.board.move(row, column, self.player)
        self.view.update(self.board)
        self.connection.send("MOVE " + str(row) + " " + str(column) + "\n")

class Board:
    """Class responsible for maintaining a board state"""

    def __init__(self, board_size):
        self.board_size = board_size
        self.board = [[Field.NONE for i in range(board_size)] for i in range(board_size)]

    def move(self, row, column, new_field):
        self.board[row][column] = new_field

class Field(Enum):
    """Enum class representing field on the board"""
    NONE = 0
    WHITE = 1
    BLACK = 2
