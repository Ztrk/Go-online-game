#include "go.h"
#include <stdio.h>
#include <stdlib.h>

const int neighbours[4][2] = {{1, 0}, {0, -1}, {-1, 0}, {0, 1}};

Move *move(Game *game, int row, int column) {
    set_board(game, row, column, game->next_player);
    game->next_player = other_player(game->next_player);
    game->passes = 0;

    // Capture enemy groups
    enum Field enemy_color = player_color(game->next_player);
    Move *captured_stones = malloc(BOARD_SIZE * BOARD_SIZE * sizeof(Move));
    captured_stones[0][0] = -1;
    captured_stones[0][1] = -1;

    for (int i = 0; i < NEIGHBOURS_NUM; ++i) {
        int next_row = row + neighbours[i][0];
        int next_column = column + neighbours[i][1];
        if (valid_coordinates(next_row, next_column)
            && game->board[next_row][next_column] == enemy_color
            && compute_liberties(game, next_row, next_column) == 0) {
                capture_group(game, next_row, next_column, captured_stones);
        }
    }

    printf("Captured: ");
    for (int i = 0; captured_stones[i][0] != -1; ++i) {
        printf("%d %d, ", captured_stones[i][0], captured_stones[i][1]);
    }
    printf("\n");
    return captured_stones;
}

bool pass(Game *game, struct Client *player) {
    if (player == game->next_player) {
        game->next_player = other_player(player);
        ++game->passes;
        return true;
    }
    return false;
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
        // Check if move captures enemy group
        bool is_capturing_move = false;
        enum Field enemy_color = NONE;
        if (player == game->black_player)
            enemy_color = WHITE;
        else
            enemy_color = BLACK;

        for (int i = 0; i < 4; ++i) {
            int next_row = row + neighbours[i][0];
            int next_column = column + neighbours[i][1];
            if (valid_coordinates(next_row, next_column)
                && game->board[next_row][next_column] == enemy_color
                && compute_liberties(game, next_row, next_column) == 0) {
                    is_capturing_move = true;
                    break;
            }
        }
        if (!is_capturing_move) {
            game->board[row][column] = NONE;
            printf("Suicide move %d %d\n", row, column);
            return false;
        }
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

void init_game(Game *game, struct Client *black, struct Client *white) {
    game->black_player = black;
    game->white_player = white;
    game->next_player = black;
    game->passes = 0;
    game->black_captured = 0;
    game->white_captured = 0;
    init_board(game);
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

enum Field player_color(struct Client *player) {
    if (player == player->game->black_player)
        return BLACK;
    else
        return WHITE;
}

void copy_board(enum Field dest_board[BOARD_SIZE][BOARD_SIZE], enum Field source_board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            dest_board[i][j] = source_board[i][j];
        }
    }
}

int compute_liberties(Game *game, int row, int column) {
    enum Field new_board[BOARD_SIZE][BOARD_SIZE];
    copy_board(new_board, game->board);
    int result = compute_liberties_util(new_board, row, column);
    printf("Liberties: %d\n", result);
    return result;
}

int compute_liberties_util(enum Field board[BOARD_SIZE][BOARD_SIZE], int row, int column) {
    printf("Visiting %d %d\n", row, column);
    if (board[row][column] == NONE) {
        board[row][column] = OTHER;
        return 1;
    }
    enum Field color = board[row][column];
    board[row][column] = OTHER;
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

void capture_group(Game *game, int row, int column, Move *captured_stones) {
    enum Field color = game->board[row][column];
    game->board[row][column] = NONE;

    if (color == BLACK) {
        ++game->black_captured;
    }
    else {
        ++game->white_captured;
    }

    // Add stone to captured stones - O(n^2)
    int j = 0;
    while (captured_stones[j][0] != -1) {
        ++j;
    }
    captured_stones[j][0] = row;
    captured_stones[j][1] = column;
    captured_stones[j + 1][0] = -1;
    captured_stones[j + 1][1] = -1;

    for (int i = 0; i < NEIGHBOURS_NUM; ++i) {
        int next_row = row + neighbours[i][0];
        int next_column = column + neighbours[i][1];
        if (valid_coordinates(next_row, next_column)
            && game->board[next_row][next_column] == color) {
                capture_group(game, next_row, next_column, captured_stones);
        }
    }
}

struct Client *get_winner(Game *game) {
    if (game->passes >= 2) {
        if (get_score(game) > 0) {
            return game->black_player;
        }
        else {
            return game->white_player;
        }
        
    }
    return NULL;
}

int get_score(Game *game) {
    int black_score = -game->black_captured;
    int white_score = 6 - game->white_captured; // komi = 6.5

    enum Field new_board[BOARD_SIZE][BOARD_SIZE];
    copy_board(new_board, game->board);

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (new_board[i][j] == NONE) {
                enum Field owner = OTHER;
                int size = get_score_util(new_board, i, j, &owner);
                if (owner == BLACK) {
                    black_score += size;
                }
                else if (owner == WHITE) {
                    white_score += size;
                }
            }
        }
    }

    printf("Score(B/W): %d %d\n", black_score, white_score);
    return black_score - white_score;
}

int get_score_util(enum Field board[BOARD_SIZE][BOARD_SIZE], int row, int column, enum Field *owner) {
    board[row][column] = OTHER;

    int size = 1;
    for (int i = 0; i < NEIGHBOURS_NUM; ++i) {
        int next_row = row + neighbours[i][0];
        int next_column = column + neighbours[i][1];

        if (valid_coordinates(next_row, next_column)) {
            if (board[next_row][next_column] == NONE) {
                size += get_score_util(board, next_row, next_column, owner);
            }
            else if (board[next_row][next_column] != OTHER) {
                if (*owner == OTHER) {
                    *owner = board[next_row][next_column];
                }
                else if (*owner != board[next_row][next_column]) {
                    *owner = NONE;
                }
            }
        }
    }
    return size;
}
