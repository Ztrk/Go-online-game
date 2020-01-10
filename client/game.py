from enum import Enum

class Game:
    """Class responsible for maintaining a game state"""

    def __init__(self, board_size, view):
        self.board_size = board_size
        self.board = [[Field.NONE for i in range(board_size)] for i in range(board_size)]
        self.player = Field.NONE
        self.next_move = (-1, -1)
        self.view = view
        self.view.update(self)

    def move_player(self, row, column):
        self.board[row][column] = self.player
        self.view.update(self)

    def move_enemy(self, row, column):
        self.board[row][column] = self.player.other()
        self.view.update(self)

    def do_move(self, row, column, new_color):
        self.board[row][column] = new_color

    def set_next_move(self, row, column):
        if row >= 0 and self.board[row][column] == Field.NONE:
            self.next_move = (row, column)
        else:
            self.next_move = (-1, -1)

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
