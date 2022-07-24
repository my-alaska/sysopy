#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <sys/epoll.h>

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

struct player_type {
    bool in_server;
    bool playing;
    char name[20];
    int socket_fd;
    pthread_mutex_t mutex;
};

#define NO_PLAYERS 32

int local_socket_fd;
int net_socket_fd;
int epoll_fd;
char* path;
u_int16_t port;

pthread_mutex_t players_mutex = PTHREAD_MUTEX_INITIALIZER;
struct player_type players[NO_PLAYERS];


void disconnect_client(int id) {
    players[id].in_server = false;
    close(players[id].socket_fd);
    printf("Disconnecting player\n");
}

void* pinging_function() {
    int pinging_epoll_fd = epoll_create1(0);
    struct message msg;
    msg.type = PING;
    struct epoll_event event;
    while (true) {
        usleep(3000000);
        for (int i = 0; i < NO_PLAYERS; ++i) {
            pthread_mutex_lock(&players[i].mutex);
            if (players[i].in_server) {
                event.data.fd = players[i].socket_fd;
                event.events = EPOLLIN;
                epoll_ctl(pinging_epoll_fd, EPOLL_CTL_ADD, players[i].socket_fd, &event);
                write(players[i].socket_fd, &msg, sizeof(msg));
                int n = epoll_wait(pinging_epoll_fd, &event, 1, 1000);
                event.data.fd = players[i].socket_fd;
                event.events = EPOLLIN;
                epoll_ctl(pinging_epoll_fd, EPOLL_CTL_DEL, players[i].socket_fd, &event);
                if (n == -1) {
                    printf("No response from player %s", players[i].name);
                    disconnect_client(i);
                } else {
                    read(players[i].socket_fd, &msg, sizeof(msg));
                }
            }
            pthread_mutex_unlock(&players[i].mutex);
        }
    }
}

void init_sockets() {
    struct sockaddr_un lcoal_address;
    lcoal_address.sun_family = AF_UNIX;
    sprintf(lcoal_address.sun_path, "%s", path);

    struct sockaddr_in net_address;
    net_address.sin_family = AF_INET;
    net_address.sin_port = htons(port);
    net_address.sin_addr.s_addr = INADDR_ANY;

    local_socket_fd= socket(AF_UNIX, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(local_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval));
    bind(local_socket_fd, (struct sockaddr*)&lcoal_address, sizeof(lcoal_address));

    net_socket_fd= socket(AF_INET, SOCK_STREAM, 0);
    int optval2 = 1;
    setsockopt(net_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval2, sizeof(optval));
    bind(net_socket_fd, (struct sockaddr*)&net_address, sizeof(net_address));

    listen(local_socket_fd, 12);
    listen(net_socket_fd, 12);

    printf("local listening: %s\n", path);
    printf("net istening: %d\n", port);
}

void init_epoll() {
    epoll_fd = epoll_create1(0);

    struct epoll_event local_event;
    local_event.events = EPOLLIN;
    local_event.data.fd = local_socket_fd;

    struct epoll_event net_event;
    net_event.events = EPOLLIN;
    net_event.data.fd = net_socket_fd;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, local_socket_fd, &local_event);
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, net_socket_fd, &net_event);
}



void start_msg(struct message* msg, int player1, int player2) {
    msg->type = START;

    msg->data.player_data.XO = 'O';
    strncpy(msg->data.player_data.name, players[player2].name, 20);
    write(players[player1].socket_fd, msg, sizeof(*msg));

    msg->data.player_data.XO = 'X';
    strncpy(msg->data.player_data.name, players[player1].name, 20);
    write(players[player2].socket_fd, msg, sizeof(*msg));
}

void update_msg(struct message* msg, int player1, int player2, char* board) {
    msg->type = UPDATE;
    strncpy(msg->data.board, board, 9);
    write(players[player1].socket_fd, msg, sizeof(*msg));
    write(players[player2].socket_fd, msg, sizeof(*msg));
}

void move_msg(struct message* msg, int player) {
    msg->type = MOVE;
    pthread_mutex_lock(&players[player].mutex);
    write(players[player].socket_fd, msg, sizeof(*msg));
    read(players[player].socket_fd, msg, sizeof(*msg));
    pthread_mutex_unlock(&players[player].mutex);
}

void force_disconnect(struct message* msg, int player) {
    msg->type = DISCONNECT;
    write(players[player].socket_fd, msg, sizeof(*msg));
    disconnect_client(player);
}

void turn_msg(struct message* msg, int player) {
    msg->type = TURN;
    write(players[player].socket_fd, msg, sizeof(*msg));
}

void end_msg(struct message* msg, int player1, int player2, char winner) {
    msg->type = END;
    msg->data.c = winner;
    write(players[player1].socket_fd, msg, sizeof(*msg));
    disconnect_client(player1);
    write(players[player2].socket_fd, msg, sizeof(*msg));
    disconnect_client(player2);
}

char check_board(char* board) {
    char r = '_';
    for (int i = 0; i < 3; i++) {
        if (board[i] != '_' && board[i + 3] == board[i] && board[i + 6] == board[i]) {

            return board[i];
        }
        if (board[i * 3] != '_' && board[i * 3 + 1] == board[i * 3] && board[i * 3 + 2] == board[i * 3]) {
            return board[i * 3];
        }
    }
    if (board[4] != '_' &&
        ((board[0] == board[4] && board[8] == board[4]) || (board[2] == board[4] && board[6] == board[4])))
        return board[4];
    return '_';
}







void game_handler(int id1, int id2) {
    printf( "game started\n");

    char board[9] = "_________";
    struct message msg;
    char c;
    int r = 0;
    int p[2] = {id1, id2};

    start_msg(&msg, id1, id2);
    update_msg(&msg, id1, id2, board);
    char symbol[2] = {'X','O'};

    int i = 0;
    while (i < 9) {
        c = check_board(board);
        if(c != '_') break;
        move_msg(&msg, p[r % 2]);
        if (msg.type == DISCONNECT) {
            pthread_mutex_lock(&players[p[r % 2]].mutex);
            disconnect_client(p[r % 2]);
            pthread_mutex_unlock(&players[p[r % 2]].mutex);
            force_disconnect(&msg, p[(r + 1) % 2]);
            turn_msg(&msg, p[(r + 1) % 2]);
            return;
        }
        if (board[msg.data.move - 1] == '_') {
            i++;
            r++;
            r %= 2;
            board[msg.data.move - 1] = symbol[r%2];
            update_msg(&msg, id1, id2, board);
        }
    }
    end_msg(&msg, id1, id2, c);
}

int connect_players(int id, struct message* msg) {
    for (int i = 0; i < NO_PLAYERS; ++i) {
        if (players[i].in_server && !players[i].playing) {
            players[i].playing = true;
            players[id].playing = true;
            pthread_mutex_unlock(&players_mutex);
            game_handler(i, id);
            return -1;
        }
    }
    players[id].playing = false;
    return 0;
}

void* game_function(void* args) {
    int* socket_fd = (int*)args;
    int client_fd = accept(*socket_fd, NULL, NULL);
    struct message msg;
    read(client_fd, &msg, sizeof(struct message));

    pthread_mutex_lock(&players_mutex);
    for (int i = 0; i < NO_PLAYERS; i++) {
        if (players[i].in_server && !strncmp(msg.data.player_data.name, players[i].name, 20)) {
            msg.type = NAME_TAKEN;
            write(client_fd, &msg,
                  sizeof(msg));
            pthread_mutex_unlock(&players_mutex);
            return NULL;
        }
    }
    int id = -1;
    for (int i = 0; i < NO_PLAYERS; i++) {
        if (!players[i].in_server) {
            players[i].in_server = true;
            players[i].playing = true;
            players[i].socket_fd = client_fd;
            strcpy(players[i].name, msg.data.player_data.name);

            printf("Player created %d\n",i);
            id = i;
            break;
        }
    }
    if (id  == -1) {
        printf("Too many players\n");
        return 0;
    }
    if (connect_players(id, &msg) == -1) return NULL;
    pthread_mutex_unlock(&players_mutex);

    msg.type = TURN;
    write(client_fd, &msg, sizeof(msg));
    return NULL;
}

void init_threads() {
    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, pinging_function, NULL);
    pthread_detach(ping_thread);
    while (true) {
        struct epoll_event events[2];
        int n = epoll_wait(epoll_fd, events, 2, -1);
        for (int i = 0; i < n; ++i) {
            pthread_t game_thread;
            pthread_create(&game_thread, NULL, game_function, (void *) &events[i].data.fd);
            pthread_detach(game_thread);
        }
    }
}

void exit_handler() {
    close(local_socket_fd);
    close(net_socket_fd);
    close(epoll_fd);
    shutdown(local_socket_fd, SHUT_RDWR);
    shutdown(net_socket_fd, SHUT_RDWR);
    unlink(path);
}

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("there must be exactly 2 arguments \n");
        return 1;
    }

    atexit(exit_handler);
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = exit;
    sigaction(SIGINT, &act, NULL);

    port = strtol(argv[1], NULL, 10);
    path = argv[2];

    for (int i = 0; i < NO_PLAYERS; ++i) {
        players[i].playing = false;
        players[i].in_server = false;
        pthread_mutex_init(&players[i].mutex, NULL);
    }

    init_sockets();
    init_epoll();
    init_threads();

    return 0;
}

