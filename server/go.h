#ifndef GO_H_GUARD
#define GO_H_GUARD

#include <stdbool.h>

#include "server.h"

#define BOARD_SIZE 19

enum Field { NONE, BLACK, WHITE };

const int neighbours[4][2];

typedef struct Game {
    struct Client *black_player;
    struct Client *white_player;
    struct Client *next_player;
    enum Field board[BOARD_SIZE][BOARD_SIZE];
} Game;

void move(Game *game, int row, int column);
void set_board(Game *game, int row, int column, struct Client *client);
bool is_valid_move(Game *game, int row, int column, struct Client *player);
bool valid_coordinates(int row, int column);
void init_board(Game *game);
struct Client *other_player(struct Client *player);
int compute_liberties(Game *game, int row, int column);
int compute_liberties_util(enum Field board[BOARD_SIZE][BOARD_SIZE], int row, int column);

#endif
