from PyQt5 import QtWidgets, uic
from board import Board, Field
from board_view import BoardView

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, controller, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        uic.loadUi("mainwindow.ui", self)
        self.new_game_button.clicked.connect(controller.on_button_click)
        self.settings_button.clicked.connect(controller.on_settings_button)

class Controller:
    def __init__(self):
        self.window = MainWindow(self)
        self.board_view = BoardView(self.window.graphics_view, self)
        self.game_board = Board(19, self.board_view)
        self.window.show()

    def on_button_click(self):
        print("Button clicked")
        dialog = QtWidgets.QDialog()
        dialog.setWindowTitle("Dialog")

        buttons = QtWidgets.QDialogButtonBox.Ok | QtWidgets.QDialogButtonBox.Cancel

        button_box = QtWidgets.QDialogButtonBox(buttons)
        button_box.accepted.connect(dialog.accept)
        button_box.rejected.connect(dialog.reject)

        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(button_box)
        dialog.setLayout(layout)

        dialog.exec_()

        self.game_board.move(4, 14, Field.WHITE)

    def on_settings_button(self):
        self.game_board.move(2, 2, Field.BLACK)

    def on_board_click(self, row, column):
        self.game_board.move(row, column, Field.BLACK)
