from enum import Enum

class Game:
    """Class responsible for maintaining a game state"""

    def __init__(self, board_size, view):
        self.board_size = board_size
        self.board = [[Field.NONE for i in range(board_size)] for i in range(board_size)]
        self.player = Field.NONE
        self.to_move = Field.BLACK
        self.result = Field.NONE
        self.view = view
        self.view.update(self)

    def move(self, row, column):
        self.board[row][column] = self.to_move
        self.to_move = self.to_move.other()
        self.view.update(self)

    def pass_turn(self):
        self.to_move = self.to_move.other()

    def set_board(self, row, column, new_color):
        self.board[row][column] = new_color


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
