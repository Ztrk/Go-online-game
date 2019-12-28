#include "go.h"

void move(Game *game, int row, int column) {
    if (game->next_player == game->black_player)
        game->board[row][column] = BLACK;
    else
        game->board[row][column] = WHITE;
    game->next_player = other_player(game->next_player);
}

bool is_valid_move(Game *game, int row, int column, struct Client *player) {
    if (game->next_player != player)
        return false;
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

struct Client *other_player(struct Client *player) {
    if (player == player->game->black_player)
        return player->game->white_player;
    else
        return player->game->black_player;
}
