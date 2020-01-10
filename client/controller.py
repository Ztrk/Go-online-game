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
        self.settings = None
        self.window.show()

    def on_new_game_button(self):
        self.client.new_game()

    def on_settings_button(self):
        self.settings = uic.loadUi("settings.ui")
        self.settings.host_line_edit.setText(self.client.connection.host)
        self.settings.port_spin_box.setValue(self.client.connection.port)
        self.settings.accepted.connect(self.on_settings_accepted)
        self.settings.open()

    def on_settings_accepted(self):
        print("Setting accepted")
        self.client.set_connection(self.settings.host_line_edit.text(),
                                   self.settings.port_spin_box.value())
        self.settings = None

    def on_board_click(self, row, column):
        self.client.move(row, column)
