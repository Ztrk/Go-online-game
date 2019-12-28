from PyQt5 import QtCore, QtNetwork

class Connection:
    def __init__(self, hostname, port, client):
        print("Connecting to", hostname, "at port", port)
        self.tcp_socket = QtNetwork.QTcpSocket()
        self.tcp_socket.readyRead.connect(self.receive)
        self.tcp_socket.connectToHost(hostname, port)
        self.client = client

    def send(self, data):
        self.tcp_socket.write(QtCore.QByteArray(bytes(data, encoding = "UTF-8")))

    def receive(self):
        data = self.tcp_socket.readLine()
        while not data.isEmpty():
            print(str(data.data(), "UTF-8"))
            self.client.handle_message(str(data.data(), encoding = "UTF-8"))
            data = self.tcp_socket.readLine()

    def __del__(self):
        print("Disconnecting from server")
        self.tcp_socket.disconnectFromHost()
