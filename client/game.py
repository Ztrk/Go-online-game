import re
from enum import Enum
from connection import Connection

class Client:
    """Class responsible for maintaining client state, creating connection and game"""

    def __init__(self, view):
        self.connection = Connection("127.0.0.1", 1234, self)
        self.view = view
        self.game = Game(19, self.view)
        self.game.player = Field.BLACK

    def handle_message(self, message):
        if message.startswith("MOVE "):
            match = re.match(r"^MOVE (\d+) (\d+)", message)
            if match is not None:
                row = int(match.group(1))
                column = int(match.group(2))
                print("move", row, column)
                self.game.move_enemy(row, column)
            elif message.endswith("OK\n"):
                self.game.next_move = (-1, -1)
            elif message.endswith("INVALID\n"):
                pass
            else:
                print("Error: Invalid move message")
        elif message.startswith("GAME CREATED"):
            if message.endswith("BLACK\n"):
                self.game.player = Field.BLACK
            else:
                self.game.player = Field.WHITE
        elif message == "INVALID MESSAGE\n":
            print("Error: client sent invalid message")
        else:
            print("Error: Invalid message")

    def move(self, row, column):
        self.game.set_next_move(row, column)
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
        self.next_move = (-1, -1)
        self.view = view
        self.view.update(self)

    def move_player(self, row, column):
        self.board[row][column] = self.player
        self.view.update(self)

    def move_enemy(self, row, column):
        self.board[row][column] = self.player.other()
        self.view.update(self)

    def set_next_move(self, row, column):
        if self.next_move[0] >= 0:
            self.board[self.next_move[0]][self.next_move[1]] = Field.NONE
            self.view.update(self)
        if row >= 0 and self.board[row][column] == Field.NONE:
            self.next_move = (row, column)
            self.move_player(row, column)
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
