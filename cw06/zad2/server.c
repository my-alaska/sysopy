#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>

#include "data.h"

int looping = 1;
int players[MAX_CLIENTS];
int server_qid;
key_t server_key;

mqd_t queue_id;

FILE* file;
time_t rcvtime;



int handle_stop(const char * message){
    mq_close(players[message[1]]);
    players[message[1]] = -1;
    return 0;
}


int handle_list(char * message){
    int sender = message[1];
    if (message[1] == 0){
        message[1] = 1;
    }
    message[2] = '\0';

    char * buffer = calloc(10,sizeof(char));
    for(int i = 0; i < MAX_CLIENTS; i++){
        if(players[i] != -1){
            sprintf(buffer, "%d ",i);
            strcat(message, buffer);
        }
    }


    mq_send(players[sender], message, MSG_SIZE, LIST);

    free(buffer);
    return 0;
}


void handle_toall(char * message) {

    for(int i = 0; i < MAX_CLIENTS; i++){
        if(players[i] != -1 ){ //&& i != message[1]
            mq_send(players[i], message, MSG_SIZE, TOALL);
        }
    }
}

int handle_toone(char * message) {
    int receiver_client = message[1];
    printf("%d %s %d \n", receiver_client, message + 2, players[receiver_client]);

    mq_send(players[receiver_client], message, MSG_SIZE, TOONE);
    return 0;
}


int handle_init(char * message) {
    int client = -1;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (players[i] == -1) {
            client = i;
            players[i] = 0;
            break;
        }
    }

    if (client == -1) { return 1; }

    char string[1050];
    sprintf(string,"%d NO-MESSAGE %s",client,ctime(&rcvtime));
    fwrite(string,sizeof(char),strlen(string),file);

    players[client] = mq_open(message + 1, O_WRONLY);
    message[1] = client;

    mq_send(players[client], message, MSG_SIZE, INIT); //MSG_SIZE???

    return 0;

}

void quit(int k){
    fclose(file);

    char * message = calloc(MSG_SIZE,sizeof(char));
    message[0]=STOP;
    message[2]='\0';

    for(int i = 0; i < MAX_CLIENTS; i++){
        if (players[i] != -1){
            mq_send(players[i], message, MSG_SIZE, STOP);
        }
    }

    mq_close(queue_id);
    mq_unlink(SERVER_FILE);
    free(message);

    exit(0);
}

int proc_msg(char* message){
    char string[1050];
    switch(message[0]){
        case STOP:
            sprintf(string,"%d NO-MESSAGE %s",message[1],ctime(&rcvtime));
            fwrite(string,sizeof(char),strlen(string),file);
            handle_stop(message);
            break;
        case LIST:
            sprintf(string,"%d NO-MESSAGE %s",message[1],ctime(&rcvtime));
            fwrite(string,sizeof(char),strlen(string),file);
            handle_list(message);
            break;
        case TOALL:
            sprintf(string,"%d %s %s",message[1],message+2,ctime(&rcvtime));
            fwrite(string,sizeof(char),strlen(string),file);
            handle_toall(message);
            break;
        case TOONE:
            sprintf(string,"%d %s %s",message[1],message+2,ctime(&rcvtime));
            fwrite(string,sizeof(char),strlen(string),file);
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

    struct mq_attr queue;
    queue.mq_maxmsg = 10;
    queue.mq_msgsize = MSG_SIZE;

    mq_unlink(SERVER_FILE);
    queue_id = mq_open(SERVER_FILE, O_RDONLY | O_CREAT, 0666, &queue);

    for(int i = 0; i < MAX_CLIENTS; i++) players[i] = -1;

    signal(SIGINT,quit);


    char message[MSG_SIZE];


    while(looping){
        mq_receive(queue_id, message, MSG_SIZE, NULL);
        time(&rcvtime);
        proc_msg(message);
    }

    quit(0);
    return 0;
}

