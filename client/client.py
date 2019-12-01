import sys
from PyQt5 import QtWidgets
from controller import Controller


if __name__ == "__main__":
    print("Starting client for Go game")
    app = QtWidgets.QApplication(sys.argv)

    controller = Controller()

    app.exec_()
