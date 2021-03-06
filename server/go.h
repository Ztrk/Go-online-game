#ifndef GO_H_GUARD
#define GO_H_GUARD

#include <stdbool.h>

#include "server.h"

#define BOARD_SIZE 19
#define NEIGHBOURS_NUM 4

enum Field { NONE, BLACK, WHITE, OTHER };

const int neighbours[4][2];
typedef int Move[2];

typedef struct Game {
    struct Client *black_player;
    struct Client *white_player;
    struct Client *next_player;
    enum Field board[BOARD_SIZE][BOARD_SIZE];
    int passes;
    int black_captured;
    int white_captured;
} Game;

Move *move(Game *game, int row, int column);
bool pass(Game *game, struct Client *player);
void set_board(Game *game, int row, int column, struct Client *client);
bool is_valid_move(Game *game, int row, int column, struct Client *player);
bool valid_coordinates(int row, int column);
void init_game(Game *game, struct Client *black, struct Client *white);
void init_board(Game *game);
struct Client *other_player(struct Client *player);
enum Field player_color(struct Client *player);
void copy_board(enum Field dest_board[BOARD_SIZE][BOARD_SIZE], enum Field source_board[BOARD_SIZE][BOARD_SIZE]);
int compute_liberties(Game *game, int row, int column);
int compute_liberties_util(enum Field board[BOARD_SIZE][BOARD_SIZE], int row, int column);
void capture_group(Game *game, int row, int column, Move *captured_stones);
struct Client *get_winner(Game *game);
int get_score(Game *game);
int get_score_util(enum Field board[BOARD_SIZE][BOARD_SIZE], int row, int column, enum Field *owner);

#endif
