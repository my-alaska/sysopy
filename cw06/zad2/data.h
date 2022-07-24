#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>

#define PROJ_ID 2137
#define MSG_SIZE 1024
#define MAX_CLIENTS 32
#define SERVER_FILE "/server"

typedef enum m_type {
    STOP = 1, LIST = 2, TOALL = 3, TOONE = 4, INIT = 5
} m_type;

typedef struct msg_data {
    long type;
    int sender_id;
    int receiver_id;
    int in_server_id;
    key_t queueu_key;
    char text[MSG_SIZE];
} msg_data;