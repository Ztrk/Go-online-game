import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QPushButton
from board import Board, Field

def onButtonClick():
    print("Button clicked")

if __name__ == "__main__":
    print("Starting client for Go game")
    app = QApplication(sys.argv)
    game_board = Board(19)
    game_board.move(1, 1, Field.WHITE)

    window = QMainWindow()
    window.setWindowTitle("Go game")
    button = QPushButton("Label in go game")
    button.clicked.connect(onButtonClick)
    window.setCentralWidget(button)
    window.show()

    app.exec_()
