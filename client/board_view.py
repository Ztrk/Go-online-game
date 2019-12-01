from PyQt5 import QtGui, QtWidgets
from board import Field

class BoardElement(QtWidgets.QGraphicsPixmapItem):
    def __init__(self, pixmap, row, column, *args, **kwargs):
        super(BoardElement, self).__init__(*args, **kwargs)

        self.setPixmap(pixmap)
        self.setPos(26 * row, 26 * column)

class BoardView:
    def __init__(self, graphics_view):
        self.graphics_view = graphics_view

        self.white = QtGui.QPixmap("./resources/26px-Go_w.svg.png")
        self.black = QtGui.QPixmap("./resources/26px-Go_b.svg.png")
        self.none = QtGui.QPixmap("./resources/Go_board_diagram_image.svg.png")
        self.scene = QtWidgets.QGraphicsScene()
        board_size = 19
        self.board_view = [[BoardElement(self.none, i, j) for j in range(board_size)]
                                                          for i in range(board_size)]
        for row in self.board_view:
            for e in row:
                self.scene.addItem(e)

        self.graphics_view.setScene(self.scene)

    def update(self, game_board):
        for row, row_view in zip(game_board.board, self.board_view):
            for field, field_view in zip(row, row_view):
                if field == Field.NONE:
                    field_view.setPixmap(self.none)
                elif field == Field.WHITE:
                    field_view.setPixmap(self.white)
                else:
                    field_view.setPixmap(self.black)
