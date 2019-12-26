#ifndef SERVER_H_GUARD
#define SERVER_H_GUARD

#include "go.h"

#define BUFFER_SIZE 100

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

void handle_message(Client *client, Server *server, const char *message);
void send_data(Client *client, int epoll_fd, const char *data);
void disconnect(Client *client, Server *server);

#endif
