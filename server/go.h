#ifndef GO_H_GUARD
#define GO_H_GUARD

#include "server.h"

#define BOARD_SIZE 19

typedef struct Game {
    Client *black_player;
    Client *white_player;
    char board[BOARD_SIZE][BOARD_SIZE];
} Game;

void move(Game *game, int row, int column, char player);

#endif
