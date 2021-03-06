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

#define LISTEN_QUEUE_SIZE 5

void handle_message(Client *client, Server *server, const char *message) {
    if (client->game != NULL && strncmp(message, "MOVE", 4) == 0) {
        if (strncmp(message, "MOVE PASS", 9) == 0) {
            if (pass(client->game, client)) {
                send_data(client, server->epoll_fd, "MOVE OK\n");
                send_data(other_player(client), server->epoll_fd, "MOVE PASS\n");
                Client *winner = get_winner(client->game);
                if (winner != NULL) {
                    end_game(server, winner);
                }
            }
            else {
                send_data(client, server->epoll_fd, "MOVE INVALID\n");
            }
            return;
        }
        int row = -1, column = -1;
        sscanf(message, "MOVE %d %d", &row, &column);
        if (is_valid_move(client->game, row, column, client)) {
            do_move(client, server, row, column);
        }
        else {
            send_data(client, server->epoll_fd, "MOVE INVALID\n");
        }
    }
    else if (client->game == NULL && client != server->waiting && strncmp(message, "NEW GAME", 8) == 0) {
        if (server->waiting == NULL) {
            server->waiting = client;
            send_data(client, server->epoll_fd, "IN QUEUE\n");
        }
        else {
            create_game(server, server->waiting, client);
        }
    }
    else {
        send_data(client, server->epoll_fd, "INVALID MESSAGE\n");
    }
}

void do_move(Client *client, Server *server, int row, int column) {
    Move *captured_stones = move(client->game, row, column);

    // Convert captured stones to string
    char captured_field[2600];
    captured_field[0] = 0;
    if (captured_stones[0][0] != -1) {
        int length = sprintf(captured_field, " CAPTURED %d %d", captured_stones[0][0], captured_stones[0][1]);
        for (int i = 1; captured_stones[i][0] != -1; ++i) {
            length += sprintf(captured_field + length, ", %d %d", captured_stones[i][0], captured_stones[i][1]);
        }
    }
    free(captured_stones);

    // Send response to current player
    char player_response[2620] = "MOVE OK";
    strcat(player_response, captured_field);
    strcat(player_response, "\n");
    send_data(client, server->epoll_fd, player_response);

    // Send response to next player
    char response[2620];
    sprintf(response, "MOVE %d %d%s\n", row, column, captured_field);
    send_data(other_player(client), server->epoll_fd, response);
}

void end_game(Server *server, Client *winner) {
    Client *loser = other_player(winner);
    send_data(winner, server->epoll_fd, "WIN\n");
    send_data(loser, server->epoll_fd, "LOSE\n");
    free(winner->game);
    winner->game = NULL;
    loser->game = NULL;
}

void create_game(Server *server, Client *black, Client *white) {
    Game *game = malloc(sizeof *game);
    init_game(game, black, white);
    black->game = game;
    white->game = game;

    server->waiting = NULL;
    send_data(game->black_player, server->epoll_fd, "GAME CREATED BLACK\n");
    send_data(game->white_player, server->epoll_fd, "GAME CREATED WHITE\n");
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
    else if (client->game != NULL) {
        end_game(server, other_player(client));
    }

    free(client);
}

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *event_loop(void *thread_data) {
    pthread_detach(pthread_self());
    Server *server = (Server*) thread_data;

    while (1) {
        struct epoll_event events;
        epoll_wait(server->epoll_fd, &events, 1, -1);

        Client *client = events.data.ptr;
        printf("Thread awoken, fd: %d\n", client->fd);
        if (events.events & EPOLLIN) {
            int length = strlen(client->receive_buffer);
            int read_bytes = recv(client->fd, client->receive_buffer + length, BUFFER_SIZE - length - 1, MSG_DONTWAIT);
            if (read_bytes < 0) {
                perror("Receive error");
            }
            else if (read_bytes == 0) {
                disconnect(client, server);
                client = NULL;
            }
            else {
                client->receive_buffer[read_bytes + length] = '\0';
                printf("%s\n", client->receive_buffer);

                char *begin = client->receive_buffer;
                char *next;
                while ((next = strchr(begin, '\n')) != NULL) {
                    handle_message(client, server, begin);
                    begin = next + 1;
                }
                int i = 0;
                for ( ; *(begin + i) != '\0'; ++i) {
                    client->receive_buffer[i] = *(begin + i);
                }
                client->receive_buffer[i] = '\0';
            }
        }
        if (client != NULL && events.events & EPOLLOUT) {
            printf("Sending data, fd: %d\n", client->fd);
            printf("%s\n", client->send_buffer);
            int length = strlen(client->send_buffer);
            int send_bytes = send(client->fd, client->send_buffer, length, MSG_DONTWAIT);

            if (length <= send_bytes) {
                client->send_buffer[0] = '\0';
                events.events = EPOLLIN;
                epoll_ctl(server->epoll_fd, EPOLL_CTL_MOD, client->fd, &events);
            }
            else {
                for (int i = 0; i <= length - send_bytes; ++i) {
                    client->send_buffer[i] = client->send_buffer[i + send_bytes];
                }
            }
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    int server_port = 1234;
    if (argc > 1) {
        if (strlen(argv[1]) <= 5) {
            server_port = atoi(argv[1]);
            if (server_port <= 0 || server_port > 65535) {
                fprintf(stderr, "%s: Port number should be an integer between 1 and 65535", argv[0]);
                exit(1);
            }
        }
        else {
            fprintf(stderr, "%s: Port number should be an integer between 1 and 65535", argv[0]);
            exit(1);
        }
    }

    char reuse_addr_val = 1;
    struct sockaddr_in server_address;

    //inicjalizacja gniazda serwera
    memset(&server_address, 0, sizeof(struct sockaddr));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(server_port);

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
    create_result = pthread_create(&thread1, NULL, event_loop, &server_data);
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
        client->receive_buffer[0] = '\0';
        client->send_buffer[0] = '\0';

        struct epoll_event events;
        events.events = EPOLLIN;
        events.data.ptr = client;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket_descriptor, &events);
    }

    close(server_socket_descriptor);
    return(0);
}
