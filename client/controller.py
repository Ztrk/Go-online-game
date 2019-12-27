from PyQt5 import QtWidgets, uic
from game import Game
from board_view import BoardView

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, controller, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        uic.loadUi("mainwindow.ui", self)
        self.new_game_button.clicked.connect(controller.on_new_game_button)
        self.settings_button.clicked.connect(controller.on_settings_button)

class Controller:
    def __init__(self):
        self.window = MainWindow(self)
        self.board_view = BoardView(self.window.graphics_view, self)
        self.game = Game(self.board_view)
        self.window.show()

    def on_new_game_button(self):
        self.game.new_game()

    def on_settings_button(self):
        self.game.move(2, 2)

    def on_board_click(self, row, column):
        self.game.move(row, column)
