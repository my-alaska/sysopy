#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

enum message_type_t {
    CONNECT,
    DISCONNECT,
    END,
    MOVE,
    NAME_TAKEN,
    PING,
    START,
    UPDATE,
    TURN
};

struct message {
    union data {
        struct player_data {
            char name[20];
            char XO;
        } player_data;
        char board[9];
        char c;
        int move;
    } data;

    enum message_type_t type;
};

char unix_path[64];
char address[64];
u_int16_t port;

int socket_fd;
char XO;

void init_local() {
    struct sockaddr_un sockaddr;
    sockaddr.sun_family = AF_UNIX;
    strcpy(sockaddr.sun_path, unix_path);
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    connect(socket_fd, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
}

void init_net() {
    struct sockaddr_in socket_address;
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    inet_pton(AF_INET, address, &socket_address.sin_addr);
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    connect(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address));
}

void end_function(struct message *msg){
    if (msg->data.c == '_') printf("no winner?!\n");
    else if (msg->data.c == XO) printf("You won\n");
    else printf("You lost!\n");

}

void move_function(struct message *msg) {
    printf("Your turn: ");
    scanf("%d", &msg->data.move);
    write(socket_fd, msg, sizeof(msg));
}

void handle_start(struct message *msg) {
    printf("Other player's name is %s\n", msg->data.player_data.name);
    printf("Your XO %c\n", msg->data.player_data.XO);
    XO = msg->data.player_data.XO;
}

void handle_update(struct message *msg) {
    printf("%c|%c|%c\n", msg->data.board[0], msg->data.board[1], msg->data.board[2]);
    printf("%c|%c|%c\n", msg->data.board[3], msg->data.board[4], msg->data.board[5]);
    printf("%c|%c|%c\n\n", msg->data.board[6], msg->data.board[7], msg->data.board[8]);
}

void exit_handler() {
    struct message msg;
    msg.type = DISCONNECT;
    write(socket_fd, &msg, sizeof(struct message));
    shutdown(socket_fd, SHUT_RDWR);
    close(socket_fd);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("there must be 3 arguments\n");
        return 1;
    }

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = exit;
    sigaction(SIGINT, &act, NULL);

    char *connection_type = calloc(64 ,sizeof(char));
    strcpy(connection_type, argv[2]);

    if (!strcmp(connection_type, "local")) {
        strcpy(unix_path,argv[3]);
        init_local();
    } else if (!strcmp(argv[2], "net")) {
        strcpy(address, strtok(argv[3], ":"));
        port = strtol(strtok(NULL, ":"), NULL, 10);
        init_net();
    }

    atexit(exit_handler);

    struct message msg;
    msg.type = CONNECT;
    strncpy(msg.data.player_data.name, argv[1], 16);
    write(socket_fd, &msg, sizeof(msg));

    while (true) {
        read(socket_fd, &msg, sizeof(struct message));
        switch (msg.type) {
            case DISCONNECT:
                printf("No response from the other player\n");
                return 0;
            case END:
                end_function(&msg);
                return 0;
            case MOVE:
                move_function(&msg);
                break;
            case NAME_TAKEN:
                printf("That name is already in_server, exiting...\n");
                return 0;
                break;
            case PING:
                write(socket_fd, &msg, sizeof(msg));
                break;
            case START:
                handle_start(&msg);
                break;
            case UPDATE:
                handle_update(&msg);
                break;
            case TURN:
                printf("Serching for another player\n");
                break;
            default:
                continue;
        }
    }
}
