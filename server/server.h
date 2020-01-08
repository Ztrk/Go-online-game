#ifndef SERVER_H_GUARD
#define SERVER_H_GUARD

#include "go.h"

#define BUFFER_SIZE 4096

typedef struct Client {
    int fd;
    char receive_buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];
    struct Game *game;
} Client;

typedef struct Server {
    int epoll_fd;
    Client *waiting;
} Server;

void do_move(Client *client, Server *server, int row, int column);
void create_game(Server *server, Client *white, Client *black);
void handle_message(Client *client, Server *server, const char *message);
void send_data(Client *client, int epoll_fd, const char *data);
void disconnect(Client *client, Server *server);

void *event_loop(void *thread_data);

#endif
