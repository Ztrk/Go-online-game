#ifndef GO_H_GUARD
#define GO_H_GUARD

#include <stdbool.h>

#include "server.h"

#define BOARD_SIZE 19

enum Field { NONE, BLACK, WHITE };

typedef struct Game {
    struct Client *black_player;
    struct Client *white_player;
    enum Field board[BOARD_SIZE][BOARD_SIZE];
} Game;

void move(Game *game, int row, int column, enum Field player);
bool is_valid_move(Game *game, int row, int column);
void init_board(Game *game);

#endif
