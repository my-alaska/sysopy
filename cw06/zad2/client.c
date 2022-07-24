#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <mqueue.h>

#include "data.h"


int looping = 1;
int players[MAX_CLIENTS];
int server_qid;
int client_qid;
int in_server_id;
char* file;


int init(){
    server_qid = mq_open(SERVER_FILE, O_WRONLY);

    struct mq_attr queue;
    queue.mq_maxmsg = 10;
    queue.mq_msgsize = MSG_SIZE;

    char * str = getenv("HOME");
    file = calloc(strlen(str),sizeof(char));
    file[0] = '/';
    file[1] = '\0';
    char * last;
    char *piece;
    piece = strtok_r(str, "/", &last);
    do{
        strcat(file, piece);
        piece = strtok_r(NULL, "/", &last);
    }while(piece != NULL);

    mq_unlink(file);
    client_qid = mq_open(file, O_RDONLY | O_CREAT, 0666, &queue);

    char * message = calloc(MSG_SIZE,sizeof(char));
    message[0] = INIT;
    message[1] = '\0';

    strcat(message, file);

    mq_send(server_qid,message,MSG_SIZE,INIT);
    mq_receive(client_qid,message,MSG_SIZE,NULL);

    in_server_id = message[1];

    free(message);
    return client_qid;
}


int list(){
    char * message = calloc(MSG_SIZE,sizeof(char));
    message[0] = LIST;
    message[1] = in_server_id;
    message[2] = '\0';

    mq_send(server_qid, message,MSG_SIZE,LIST);

    mq_receive(client_qid, message, MSG_SIZE, NULL);

    printf("%s\n",message+2);
    free(message);
    return 0;
}


int toall(char * text){
    char * message = calloc(MSG_SIZE,sizeof(char));
    message[0] = TOALL;
    message[1] = in_server_id;
    strcpy(message+2, text);
    mq_send(server_qid, message, MSG_SIZE, TOALL);

    free(message);
    return 0;
}

int toone(char * text, int receiver_id){
    char * message = calloc(MSG_SIZE,sizeof(char));
    message[0] = TOONE;
    message[1] = receiver_id;
    strcpy(message+2, text);
    mq_send(server_qid, message, MSG_SIZE, TOONE);

    free(message);
    return 0;
}


void stop() {
    char * message = calloc(MSG_SIZE,sizeof(char));
    message[0] = STOP;
    message[1] = in_server_id;
    message[2] = '\0';

    mq_send(server_qid, message, MSG_SIZE, STOP);

    free(message);

    mq_close(client_qid);
    mq_close(server_qid);
    mq_unlink(file);

    exit(0);
}


int show_message(char * message){
    printf("%s \n",message);
    return 0;
}


int server_check() {
    char * message = calloc(MSG_SIZE,sizeof(char));

    mq_receive(client_qid, message, MSG_SIZE, NULL);
    switch (message[0]){
        case STOP:
            free(message);
            stop();
            break;
        case TOONE:
        case TOALL:
            show_message(message+2);
            break;
    }



    free(message);
    return 0;
}


int main(int argc, char ** argv) {
    init();

    signal(SIGINT, stop);

    int len = MSG_SIZE+10;
    char cmd[len];
    struct mq_attr queue;

    while(looping){

        mq_getattr(client_qid, &queue);

        if(queue.mq_curmsgs > 0){
            server_check();
            continue;
        }

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

