#include "go.h"

void move(Game *game, int row, int column, enum Field player) {
    game->board[row][column] = player;
}

bool is_valid_move(Game *game, int row, int column) {
    if (row < 0 || row >= BOARD_SIZE)
        return false;
    if (column < 0 || column >= BOARD_SIZE)
        return false;
    if (game->board[row][column] != NONE)
        return false;
    return true;
}

void init_board(Game *game) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            game->board[i][j] = NONE;
        }
    }
}
