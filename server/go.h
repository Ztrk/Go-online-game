#ifndef GO_H_GUARD
#define GO_H_GUARD

#include <stdbool.h>

#include "server.h"

#define BOARD_SIZE 19

enum Field { NONE, BLACK, WHITE };

typedef struct Game {
    struct Client *black_player;
    struct Client *white_player;
    struct Client *next_player;
    enum Field board[BOARD_SIZE][BOARD_SIZE];
} Game;

void move(Game *game, int row, int column);
bool is_valid_move(Game *game, int row, int column, struct Client *player);
void init_board(Game *game);
struct Client *other_player(struct Client *player);

#endif
