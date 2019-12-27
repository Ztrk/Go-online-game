import re
from PyQt5 import QtCore, QtNetwork
import game

class Connection:
    def __init__(self, hostname, port, game):
        print("Connecting to", hostname, "at port", port)
        self.tcp_socket = QtNetwork.QTcpSocket()
        self.tcp_socket.readyRead.connect(self.receive)
        self.tcp_socket.connectToHost(hostname, port)
        self.game = game

    def handle_message(self, message):
        match = re.match(r"^MOVE (\d+) (\d+)", message)
        if match is not None:
            row = int(match.group(1))
            column = int(match.group(2))
            print("move", row, column)
            self.game.board.move(row, column, game.Field.WHITE)
        elif message == "INVALID MESSAGE\n":
            print("Error: client sent invalid message")
        else:
            print("Error: Invalid message")

    def send(self, data):
        self.tcp_socket.write(QtCore.QByteArray(bytes(data, encoding = "UTF-8")))

    def receive(self):
        data = self.tcp_socket.readLine()
        while not data.isEmpty():
            print(str(data.data(), "UTF-8"))
            self.handle_message(str(data.data(), encoding = "UTF-8"))
            data = self.tcp_socket.readLine()

    def __del__(self):
        print("Disconnecting from server")
        self.tcp_socket.disconnectFromHost()
