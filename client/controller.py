from PyQt5 import QtWidgets, uic
from game import Client
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
        self.client = Client(self.board_view)
        self.window.show()

    def on_new_game_button(self):
        self.client.new_game()

    def on_settings_button(self):
        self.client.move(2, 2)

    def on_board_click(self, row, column):
        self.client.move(row, column)
