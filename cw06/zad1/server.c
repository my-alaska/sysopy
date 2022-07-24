#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>

#include "data.h"

int looping = 1;
int players[MAX_CLIENTS];
int server_qid;
key_t server_key;

FILE* file;
time_t rcv_time;


int handle_stop(msg_data * message){
    if(players[message->in_server_id] != -1){
        players[message->in_server_id] = -1;
    }
    return 0;
}


int handle_list(msg_data * message){
    int client = message->in_server_id;

    message->text[0] = '\0';
    char * buffer = calloc(10,sizeof(char));
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(players[i] != -1){
            sprintf(buffer, "%d ",i);
            strcat(message->text, buffer);
        }
    }


    message->sender_id = server_qid;
    message->receiver_id = players[client];
    message->queueu_key = server_key;
    message->in_server_id = client;

    msgsnd(players[client], message, MSG_SIZE, 0);
    free(buffer);
    return 0;
}


void handle_toall(msg_data *message) {
    int client = message->in_server_id;

    message->sender_id = server_qid;
    message->receiver_id = players[client];
    message->queueu_key = server_key;
    message->in_server_id = client;

    for(int i = 0; i < MAX_CLIENTS; i++){
        if(players[i] != -1 ){ //&& i != client
            msgsnd(players[i], message, MSG_SIZE, 0);
        }}
}


int handle_toone(msg_data *message) {
    int receiver_client = message->receiver_id;

    message->sender_id = server_qid;
    message->receiver_id = players[receiver_client];
    message->queueu_key = server_key;
    message->in_server_id = receiver_client;

    msgsnd(players[receiver_client], message, MSG_SIZE, 0);
    return 0;
}


int handle_init(msg_data * message) {
    int client = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (players[i] == -1) {
            client = i;
            break;
        }
    }

    if (client == -1) { return 1; }


    players[client] = msgget(message->queueu_key, 0);

    char string[1050];
    sprintf(string,"%d %s %s \n",message->in_server_id, message->text, ctime(&rcv_time));
    fwrite(string,sizeof(char),strlen(string),file);

    message->type = INIT;
    message->sender_id = server_qid;
    message->receiver_id = players[client];
    message->queueu_key = server_key;
    message->in_server_id = client;
    msgsnd(players[client], message, MSG_SIZE, 0); //MSG_SIZE???


    return 0;

}

void quit(int k){
    fclose(file);
    msg_data * message = calloc(1,sizeof(msg_data));
    message->type = STOP;

    for(int i = 0; i < MAX_CLIENTS; i++){
        if (players[i] != -1){
            message->sender_id = server_qid;
            message->receiver_id = players[i];
            message->queueu_key = server_key;
            message->in_server_id = i;
            msgsnd(players[i], message, MSG_SIZE, 0);
        }
    }

    msgctl(server_qid, IPC_RMID, NULL);
    free(message);

    exit(0);
}

int proc_msg(msg_data* message){
    if(message->type != INIT){
        char string[1050];
        sprintf(string,"%d %s %s \n",message->in_server_id, message->text, ctime(&rcv_time));
        fwrite(string,sizeof(char),strlen(string),file);
    }

    switch(message->type){
        case STOP:
            handle_stop(message);
            break;
        case LIST:
            handle_list(message);
            break;
        case TOALL:
            handle_toall(message);
            break;
        case TOONE:
            handle_toone(message);
            break;
        case INIT:
            handle_init(message);
            break;
        default:
            return 1;
    }
    return 0;
}

int main() {
    file = fopen("file.txt", "w+");

    server_key = ftok(SERVER_FILE, PROJ_ID);
    server_qid = msgget(server_key, IPC_CREAT | 0666);


    for(int i = 0; i < MAX_CLIENTS; i++) players[i] = -1;

    signal(SIGINT,quit);

    msg_data message;
    while(looping){

        msgrcv(server_qid, &message, MSG_SIZE, -6, 0);
        time(&rcv_time);
        proc_msg(&message);

    }

    quit(0);
    return 0;
}