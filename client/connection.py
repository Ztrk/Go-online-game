from PyQt5 import QtCore, QtNetwork

class Connection:
    def __init__(self, hostname, port):
        print("Connecting to", hostname, "at port", port)
        self.tcp_socket = QtNetwork.QTcpSocket()
        self.tcp_socket.connectToHost(hostname, port)

    def send(self, data):
        self.tcp_socket.write(QtCore.QByteArray(bytes(data, "UTF-8")))

    def __del__(self):
        print("Disconnecting from server")
        self.tcp_socket.disconnectFromHost()
