import re
from connection import Connection
from game import Game, Field

class Client:
    """Class responsible for maintaining client state, creating connection and game"""

    def __init__(self, board_view, connection_view, game_state_view):
        self.connection = Connection("127.0.0.1", 1234, self, connection_view)
        self.board_view = board_view
        self.connection_view = connection_view
        self.game_state_view = game_state_view
        self.next_move = None
        self.game = None

    def handle_message(self, message):
        if message.startswith("MOVE "):
            match = re.match(r"^MOVE (\d+) (\d+)", message)
            if match is not None:
                row = int(match.group(1))
                column = int(match.group(2))
                print("move", row, column)
                self.game.move(row, column)
                self.handle_captured_field(message)
            elif message.startswith("MOVE OK"):
                if self.next_move[0] != -1:
                    self.game.move(*self.next_move)
                else:
                    self.game.pass_turn()
                self.next_move = None
                self.handle_captured_field(message)
            elif message.startswith("MOVE PASS"):
                self.game.pass_turn()
            elif message.endswith("INVALID\n"):
                self.next_move = None
                self.game_state_view.print_message("Move invalid")
            else:
                print("Error: Move message not recognized")
        elif message.startswith("GAME CREATED"):
            if message.endswith("BLACK\n"):
                self.create_new_game(Field.BLACK)
            else:
                self.create_new_game(Field.WHITE)
        elif message.startswith("WIN"):
            self.game.result = self.game.player
        elif message.startswith("LOSE"):
            self.game.result = self.game.player.other()
        elif message.startswith("IN QUEUE"):
            self.end_game()
            self.game_state_view.print_message("Waiting for other player")
        elif message == "INVALID MESSAGE\n":
            print("Error: client sent invalid message")
        else:
            print("Error: Message not recognized")

        self.game_state_view.on_game_state_change(self.game)

    def handle_captured_field(self, message):
        captured_match = re.search(r"CAPTURED ((\d+) (\d+)(?:, (\d+) (\d+))*)$", message)
        if captured_match is not None:
            print("Group captured", captured_match.group(1))
            stones = captured_match.group(1).split(',')
            for stone in stones:
                row, column = stone.split()
                row = int(row)
                column = int(column)
                self.game.set_board(row, column, Field.NONE)
            self.board_view.update(self.game)

    def move(self, row, column):
        if self.next_move is None and self.game.result == Field.NONE:
            self.connection.send("MOVE " + str(row) + " " + str(column) + "\n")
            self.next_move = (row, column)

    def pass_turn(self):
        if self.next_move is None and self.game.result == Field.NONE:
            self.next_move = (-1, -1)
            self.connection.send("MOVE PASS\n")

    def request_new_game(self):
        self.connection.send("NEW GAME\n")

    def create_new_game(self, color):
        self.game = Game(19, self.board_view)
        self.game.player = color

    def end_game(self):
        self.game = None
        self.board_view.update(self.game)
        self.game_state_view.on_game_state_change(self.game)

    def set_connection(self, host, port):
        self.connection = Connection(host, port, self, self.connection_view)
        self.end_game()
