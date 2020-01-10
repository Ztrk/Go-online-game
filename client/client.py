import re
from connection import Connection
from game import Game, Field

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
            elif message.startswith("MOVE OK"):
                self.game.move_player(*self.game.next_move)
            elif message.endswith("INVALID\n"):
                print("Error: Move invalid")
                return
            else:
                print("Error: Move message not recognized")
                return

            captured_match = re.search(r"CAPTURED ((\d+) (\d+)(?:, (\d+) (\d+))*)$", message)
            if captured_match is not None:
                print("Group captured", captured_match.group(1))
                stones = captured_match.group(1).split(',')
                for stone in stones:
                    row, column = stone.split()
                    row = int(row)
                    column = int(column)
                    self.game.do_move(row, column, Field.NONE)
                self.view.update(self.game)
        elif message.startswith("GAME CREATED"):
            if message.endswith("BLACK\n"):
                self.game.player = Field.BLACK
            else:
                self.game.player = Field.WHITE
        elif message == "INVALID MESSAGE\n":
            print("Error: client sent invalid message")
        else:
            print("Error: Message not recognized")

    def move(self, row, column):
        self.game.set_next_move(row, column)
        self.connection.send("MOVE " + str(row) + " " + str(column) + "\n")

    def new_game(self):
        self.game = Game(19, self.view)
        self.game.player = Field.BLACK
        self.connection.send("NEW GAME\n")

    def set_connection(self, host, port):
        self.connection = Connection(host, port, self)
        self.game = Game(19, self.view)
        self.game.player = Field.BLACK
