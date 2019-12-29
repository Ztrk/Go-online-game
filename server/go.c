#include "go.h"
#include <stdio.h>

const int neighbours[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};

void move(Game *game, int row, int column) {
    set_board(game, row, column, game->next_player);
    // TODO: kill enemy groups
    game->next_player = other_player(game->next_player);
}

void set_board(Game *game, int row, int column, struct Client *player) {
    if (player == game->black_player)
        game->board[row][column] = BLACK;
    else
        game->board[row][column] = WHITE;
}

bool is_valid_move(Game *game, int row, int column, struct Client *player) {
    if (game->next_player != player)
        return false;
    if (!valid_coordinates(row, column))
        return false;
    if (game->board[row][column] != NONE)
        return false;

    // Check if move is a suicide
    set_board(game, row, column, player);
    if (compute_liberties(game, row, column) == 0) {
        printf("Suicide move %d %d\n", row, column);
        // TODO: move could kill enemy group
        // for n in neighbours
        //      if n is enemy and compute_liberties(n) == 0
        //          move is ok
        game->board[row][column] = NONE;
        return false;
    }
    game->board[row][column] = NONE;
    return true;
}

bool valid_coordinates(int row, int column) {
    if (row < 0 || row >= BOARD_SIZE)
        return false;
    if (column < 0 || column >= BOARD_SIZE)
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

int compute_liberties(Game *game, int row, int column) {
    enum Field new_board[BOARD_SIZE][BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            new_board[i][j] = game->board[i][j];
            printf("%d ", new_board[i][j]);
        }
        printf("\n");
    }
    int result = compute_liberties_util(new_board, row, column);
    printf("Liberties: %d\n", result);
    return result;
}

int compute_liberties_util(enum Field board[BOARD_SIZE][BOARD_SIZE], int row, int column) {
    const int VISITED = -1;
    printf("Visiting %d %d\n", row, column);
    if (board[row][column] == NONE) {
        board[row][column] = VISITED;
        return 1;
    }
    enum Field color = board[row][column];
    board[row][column] = VISITED;
    int result = 0;
    for (int i = 0; i < 4; ++i) {
        int next_row = row + neighbours[i][0];
        int next_column = column + neighbours[i][1];
        if (valid_coordinates(next_row, next_column)
            && (board[next_row][next_column] == NONE
            || board[next_row][next_column] == color)) {
                result += compute_liberties_util(board, next_row, next_column);
        }
    }
    return result;
}
