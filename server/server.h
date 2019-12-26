#ifndef SERVER_H_GUARD
#define SERVER_H_GUARD

#define BUFFER_SIZE 100

typedef struct Thread_data {
    int epoll_fd;
} Thread_data;

typedef struct Client {
    int fd;
    char receive_buffer[BUFFER_SIZE];
    char send_buffer[BUFFER_SIZE];
} Client;

#endif
