import sys
from PyQt5 import QtWidgets, uic
from board import Board, Field
from board_view import BoardView

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        uic.loadUi("mainwindow.ui", self)
        self.new_game_button.clicked.connect(on_button_click)
        self.settings_button.clicked.connect(on_settings_button)
        self.board_view = BoardView(self.graphics_view)


def on_button_click():
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

    game_board = Board(19)
    game_board.move(4, 14, Field.WHITE)
    window.board_view.update(game_board)


def on_settings_button():
    game_board = Board(19)
    game_board.move(2, 2, Field.BLACK)
    window.board_view.update(game_board)

if __name__ == "__main__":
    print("Starting client for Go game")
    app = QtWidgets.QApplication(sys.argv)

    window = MainWindow()
    window.show()

    app.exec_()
