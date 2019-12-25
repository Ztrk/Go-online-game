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
#define SERVER_PORT 1234
#define QUEUE_SIZE 5
#define BUFFER_SIZE 100

//struktura zawierająca dane, które zostaną przekazane do wątku
struct thread_data_t {
    int epoll_fd;
};

//funkcja opisującą zachowanie wątku - musi przyjmować argument typu (void *) i zwracać (void *)
void *ThreadBehavior(void *t_data) {
    pthread_detach(pthread_self());
    struct thread_data_t *th_data = (struct thread_data_t*)t_data;

    char buffer[BUFFER_SIZE];
    while (1) {
        struct epoll_event events;
        epoll_wait(th_data->epoll_fd, &events, 1, -1);
        printf("Thread awoken, fd: %d\n", events.data.fd);
        if (events.events & EPOLLIN) {
            int read_bytes = recv(events.data.fd, buffer, BUFFER_SIZE - 1, MSG_DONTWAIT);
            if (read_bytes < 0) {
                fprintf(stderr, "Receive error, errno: %d\n", errno);
            }
            if (read_bytes == 0) {
                printf("End of file, disconnecting fd: %d\n", events.data.fd);
                epoll_ctl(th_data->epoll_fd, EPOLL_CTL_DEL, events.data.fd, NULL);
                close(events.data.fd);
            }
            buffer[read_bytes] = 0;
            printf("%s", buffer);
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

    int listen_result = listen(server_socket_descriptor, QUEUE_SIZE);
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
    struct thread_data_t thread_data;
    thread_data.epoll_fd = epoll_fd;
    pthread_t thread1;
    create_result = pthread_create(&thread1, NULL, ThreadBehavior, &thread_data);
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

        struct epoll_event event;
        epoll_data_t epoll_data;
        epoll_data.fd = connection_socket_descriptor;

        event.events = EPOLLIN;
        event.data = epoll_data;
        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, connection_socket_descriptor, &event);
    }

    close(server_socket_descriptor);
    return(0);
}