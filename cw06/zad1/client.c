#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "data.h"


int looping = 1;
int players[MAX_CLIENTS];
int server_qid;
int client_qid;
int in_server_id;
key_t server_key;
key_t client_key;


int init(char * file){

    server_key = ftok(SERVER_FILE, PROJ_ID);
    server_qid = msgget(server_key, 0);

    char * str = getenv("HOME");
    client_key = ftok(str, PROJ_ID);

    client_qid = msgget(client_key, IPC_CREAT | 0666);

    msg_data *message = calloc(1,sizeof(msg_data));
    message->type = INIT;
    message->sender_id = client_qid;
    message->receiver_id = server_qid;
    message->queueu_key = client_key;
    strcpy(message->text, "NO-MESSAGE");


    msgsnd(server_qid, message, MSG_SIZE, 0);

    msgrcv(client_qid, message, MSG_SIZE, INIT,  0);
    in_server_id = message->in_server_id;

    free(message);
    return client_qid;
}


int list(){
    msg_data * message = calloc(1,sizeof(msg_data));
    message->type = LIST;
    message->in_server_id = in_server_id;
    message->sender_id = client_qid;
    message->receiver_id = server_qid;
    message->queueu_key = client_key;
    strcpy(message->text, "NO-MESSAGE");

    msgsnd(server_qid, message,MSG_SIZE,0);

    msgrcv(client_qid, message, MSG_SIZE, LIST,  0);
    printf("%s\n",message->text);
    free(message);
    return 0;
}


int toall(char * text){
    msg_data * message = calloc(1,sizeof(msg_data));
    message->type = TOALL;
    message->in_server_id = in_server_id;
    message->sender_id = client_qid;
    message->receiver_id = server_qid;
    message->queueu_key = client_key;
    strcpy(message->text, text);

    msgsnd(server_qid, message, MSG_SIZE, 0);

    free(message);
    return 0;
}

int toone(char * text, int receiver_id){
    msg_data * message = calloc(1,sizeof(msg_data));
    message->type = TOONE;
    message->in_server_id = in_server_id;
    message->sender_id = client_qid;
    message->receiver_id = receiver_id;
    message->queueu_key = client_key;
    strcpy(message->text, text);

    msgsnd(server_qid, message, MSG_SIZE, 0);

    free(message);
    return 0;
}


void stop() {
    msg_data * message = calloc(1,sizeof(msg_data));
    message->type = STOP;
    message->in_server_id = in_server_id;
    message->sender_id = client_qid;
    message->receiver_id = server_qid;
    message->queueu_key = client_key;
    strcpy(message->text, "NO-MESSAGE");

    msgsnd(server_qid, message, MSG_SIZE, 0);

    free(message);

    msgctl(client_qid, IPC_RMID, NULL);

    exit(0);
}


int show_message(msg_data * message){
    printf("%s \n",message -> text);
    return 0;
}

int server_check() {
    msg_data* message = calloc(1,sizeof(msg_data));

    if(msgrcv(client_qid, message, MSG_SIZE, 0, IPC_NOWAIT) >= 0) {
        switch (message->type){
            case STOP:
                free(message);
                stop();
                break;
            case TOONE:
            case TOALL:
                show_message(message);
                break;
        }
        return 1;
    }

    free(message);
    return 0;
}


int main(int argc, char ** argv) {
    init(argv[1]);

    signal(SIGINT, stop);

    int len = MSG_SIZE+10;
    char cmd[len];
    while(looping){
        if(server_check() == 1) continue;

        fgets(cmd, len, stdin);
        if(strcmp(cmd,"") == 0) continue;

        if (cmd[strlen(cmd)-1] == '\n'){
            cmd[strlen(cmd)-1] = '\0';
        }

        char *ptr;
        char *token = strtok_r(cmd, " ", &ptr);
        if (strcmp(token, "LIST") == 0) {
            list();
        }
        else if (strcmp(token, "STOP") == 0) {
            stop();
        }
        else if (strcmp(token, "TOALL") == 0) {
            toall(ptr);
        }
        else if (strcmp(token, "TOONE")==0) {
            char *token2 = strtok_r(NULL, " ", &ptr);
            toone(ptr,atoi(token2));
        }else if (strcmp(token, "GETMSG")==0) {
            continue;
        }else{
            printf("invalid command\n");
        }

        usleep(1000);

    }

    stop();


    return 0;
}

