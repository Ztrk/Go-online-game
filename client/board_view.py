from PyQt5 import QtGui, QtWidgets
from game import Field

class BoardElement(QtWidgets.QGraphicsPixmapItem):
    def __init__(self, pixmap, row, column, controller, *args, **kwargs):
        super(BoardElement, self).__init__(*args, **kwargs)

        self.setPixmap(pixmap)
        self.row = row
        self.column = column
        self.setPos(26 * column, 26 * row)
        self.controller = controller

    def mousePressEvent(self, event):
        print("Clicked board event", self.row, self.column)
        self.controller.on_board_click(self.row, self.column)

class BoardView:
    def __init__(self, graphics_view, controller):
        self.graphics_view = graphics_view

        self.controller = controller
        self.white = QtGui.QPixmap("./resources/26px-Go_w.svg.png")
        self.black = QtGui.QPixmap("./resources/26px-Go_b.svg.png")
        self.none = QtGui.QPixmap("./resources/Go_board_diagram_image.svg.png")
        self.scene = QtWidgets.QGraphicsScene()
        self.board_size = 0
        self.board_view = []

        self.graphics_view.setScene(self.scene)

    def update(self, game_board):
        if game_board is None:
            self.create_board(0)
            return

        if game_board.board_size != self.board_size:
            self.create_board(game_board.board_size)

        for row, row_view in zip(game_board.board, self.board_view):
            for field, field_view in zip(row, row_view):
                if field == Field.NONE:
                    field_view.setPixmap(self.none)
                elif field == Field.WHITE:
                    field_view.setPixmap(self.white)
                else:
                    field_view.setPixmap(self.black)

    def create_board(self, board_size):
        self.board_size = board_size
        for row in self.board_view:
            for item in row:
                self.scene.removeItem(item)

        self.board_view = [[BoardElement(self.none, i, j, self.controller)
                            for j in range(board_size)] for i in range(board_size)]

        for row in self.board_view:
            for item in row:
                self.scene.addItem(item)
