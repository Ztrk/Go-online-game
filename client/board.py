from enum import Enum

class Board:
    """Class responsible for maintaining a board state"""

    def __init__(self, board_size, view):
        self.board_size = board_size
        self.board = [[Field.NONE for i in range(board_size)] for i in range(board_size)]
        self.view = view

    def move(self, row, column, new_field):
        self.board[row][column] = new_field
        self.view.update(self)

class Field(Enum):
    """Enum class representing field on the board"""
    NONE = 0
    WHITE = 1
    BLACK = 2
