#include "go.h"

void move(Game *game, int row, int column, char player) {
    game->board[row][column] = player;
}
