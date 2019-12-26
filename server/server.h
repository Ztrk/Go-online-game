#ifndef SERVER_H_GUARD
#define SERVER_H_GUARD

#include "go.h"

#define BUFFER_SIZE 100

typedef struct Thread_data {
    int epoll_fd;
} Thread_data;

typedef struct Client {
    int fd;
    char receive_buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];
    struct Game *game;
} Client;

void handle_message(Client *client, int epoll_fd, const char *message);
void send_data(Client *client, int epoll_fd, const char *data);

#endif
