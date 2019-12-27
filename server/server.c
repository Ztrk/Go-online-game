#include <errno.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "server.h"

#define SERVER_PORT 1234
#define LISTEN_QUEUE_SIZE 5

void handle_message(Client *client, Server *server, const char *message) {
    if (client->game != NULL && strncmp(message, "MOVE", 4) == 0) {
        int row = -1, column = -1;
        sscanf(message, "MOVE %d %d", &row, &column);
        printf("row: %d, column %d\n", row, column);
        if (is_valid_move(client->game, row, column)) {
            if (client == client->game->black_player) {
                move(client->game, row, column, BLACK);
            }
            else {
                move(client->game, row, column, WHITE);
            }
            send_data(client, server->epoll_fd, "MOVE OK\n");

            char response[100];
            sprintf(response, "MOVE %d %d\n", row, column);
            if (client->game->white_player == client) {
                send_data(client->game->black_player, server->epoll_fd, response);
            }
            else {
                send_data(client->game->white_player, server->epoll_fd, response);
            }
        }
        else {
            send_data(client, server->epoll_fd, "MOVE INVALID\n");
        }
    }
    else if (client->game == NULL && client != server->waiting && strncmp(message, "NEW GAME", 8) == 0) {
        if (server->waiting == NULL) {
            server->waiting = client;
        }
        else {
            Game *game = malloc(sizeof game);
            init_board(game);
            game->black_player = server->waiting;
            game->white_player = client;
            client->game = game;
            server->waiting->game = game;
            server->waiting = NULL;
            send_data(game->black_player, server->epoll_fd, "GAME CREATED BLACK\n");
            send_data(game->white_player, server->epoll_fd, "GAME CREATED WHITE\n");
        }
    }
    else {
        send_data(client, server->epoll_fd, "INVALID MESSAGE\n");
    }
}

void send_data(Client *client, int epoll_fd, const char *data) {

    strcat(client->send_buffer, data);

    struct epoll_event events;
    events.events = EPOLLIN | EPOLLOUT;
    events.data.ptr = client;

    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, client->fd, &events);
}

void disconnect(Client *client, Server *server) {
    printf("Disconnecting, fd: %d\n", client->fd);
    epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, client->fd, NULL);
    close(client->fd);

    if (server->waiting == client) {
        server->waiting = NULL;
    }

    // Free game object
    if (client->game != NULL) {
        if (client == client->game->white_player) {
            client->game->white_player = NULL;
        }
        else {
            client->game->black_player = NULL;
        }
        if (client->game->white_player == NULL && client->game->black_player == NULL) {
            free(client->game);
        }
    }

    free(client);
}

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data) {
    pthread_detach(pthread_self());
    Server *th_data = (Server*) t_data;

    while (1) {
        struct epoll_event events;
        epoll_wait(th_data->epoll_fd, &events, 1, -1);

        Client *client = events.data.ptr;
        printf("Thread awoken, fd: %d\n", client->fd);
        if (events.events & EPOLLIN) {
            int read_bytes = recv(client->fd, client->receive_buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
            if (read_bytes < 0) {
                fprintf(stderr, "Receive error, errno: %d\n", errno);
            }
            else if (read_bytes == 0) {
                disconnect(client, th_data);
            }
            else {
                client->receive_buffer[read_bytes] = 0;
                printf("%s\n", client->receive_buffer);
                handle_message(client, th_data, client->receive_buffer);
            }
        }
        if (events.events & EPOLLOUT) {
            printf("Sending data, fd: %d\n", client->fd);
            int send_bytes = send(client->fd, client->send_buffer, strlen(client->send_buffer), MSG_DONTWAIT);

            client->send_buffer[0] = 0;

            events.events = EPOLLIN;
            epoll_ctl(th_data->epoll_fd, EPOLL_CTL_MOD, client->fd, &events);
        }
    }

    pthread_exit(NULL);
}


int main(int argc, char* argv[]) {
    char reuse_addr_val = 1;
    struct sockaddr_in server_address;

    //inicjalizacja gniazda serwera
    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(SERVER_PORT);

    int server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_descriptor < 0) {
        fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda.\n", argv[0]);
        exit(1);
    }
    setsockopt(server_socket_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_addr_val, sizeof(reuse_addr_val));

    int bind_result = bind(server_socket_descriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr));
    if (bind_result < 0) {
        fprintf(stderr, "%s: Błąd przy próbie dowiązania adresu IP i numeru portu do gniazda.\n", argv[0]);
        exit(1);
    }

    int listen_result = listen(server_socket_descriptor, LISTEN_QUEUE_SIZE);
    if (listen_result < 0) {
        fprintf(stderr, "%s: Błąd przy próbie ustawienia wielkości kolejki.\n", argv[0]);
        exit(1);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        fprintf(stderr, "%s: Error while creating epoll instance.\n", argv[0]);
        exit(1);
    }

    int create_result = 0;
    Server server_data;
    server_data.epoll_fd = epoll_fd;
    server_data.waiting = NULL;
    pthread_t thread1;
    create_result = pthread_create(&thread1, NULL, ThreadBehavior, &server_data);
    if (create_result) {
        fprintf(stderr, "%s: Błąd przy próbie utworzenia wątku, kod błędu: %d\n", argv[0], create_result);
        exit(1);
    }

    int connection_socket_descriptor = 1;
    int should_close = 0;
    while (!should_close) {
        connection_socket_descriptor = accept(server_socket_descriptor, NULL, NULL);
        if (connection_socket_descriptor < 0) {
            fprintf(stderr, "%s: Błąd przy próbie utworzenia gniazda dla połączenia.\n", argv[0]);
            exit(1);
        }
        printf("Connection accepted, fd: %d\n", connection_socket_descriptor);

        Client *client = malloc(sizeof(Client));
        client->fd = connection_socket_descriptor;
        client->game = NULL;

        struct epoll_event events;
        events.events = EPOLLIN;
        events.data.ptr = client;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket_descriptor, &events);
    }

    close(server_socket_descriptor);
    return(0);
}
