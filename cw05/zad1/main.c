#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


int wc(const char* word){
    int no_words = 0;
    char prev = ' ';
    int i = 0;
    while(word[i] != '\0' ){
        if(word[i] != ' ' && word[i]!= '\n' && (prev == ' ' || prev == '\n')){
            no_words++;
        }
        prev = word[i];
        i++;
    }
    return no_words;
}

int lc(const char* word){
    int no_lines = 0;
    int i = 0;
    while(word[i] != '\n' && word[i] != '\0'){
        i++;
        if(word[i] == '|'){
            no_lines++;
        }
    }
    return ++no_lines;
}


void str_repair(char ** array, int len){
    int j,l;
    char* temp_string;
    for(int i = 0; i < len; i++){
        j = strlen(array[i]);
        l = 0;
        for(int k = 0; k < j; k++){
            if(array[i][k]!=' ' && array[i][k]!='\n' ){
                l++;}}
        temp_string = calloc(l+1,sizeof(char));
        l = 0;
        for(int k = 0; k < j; k++){
            if(array[i][k]!=' ' && array[i][k]!='\n' ){
                temp_string[l] = array[i][k];
                l++;
            }}
        free(array[i]);
        array[i] = calloc(l+1,sizeof(char));
        strcpy(array[i],temp_string);
        free(temp_string);
    }}




int get_to_exec(int no_subtasks, char **subtasks, char ***cmds,
                    char **cmd_names,int no_cmds, const int *no_subcmds, char ***result){
    int result_len = 0;
    for(int i = 0; i < no_subtasks; i++){
        for(int j = 0; j<no_cmds;j++){
            if (strcmp(subtasks[i],cmd_names[j])==0){
                result_len += no_subcmds[j];
                break;
            }}}
    int l = 0;
    *result = calloc(result_len,sizeof(char*));
    for(int i = 0; i < no_subtasks; i++){
        for(int j = 0; j<no_cmds;j++){
            if (strcmp(subtasks[i],cmd_names[j])==0){
                for(int k = 0; k < no_subcmds[j];k++){
                    (*result)[l] = calloc(strlen(cmds[j][k])+1, sizeof(char));
                    strcpy((*result)[l],cmds[j][k]);
                    l++;}
                break;
            }}}
    return result_len;
}




int parse_command(char* string, char *** args){
    int no_words = wc(string);
    *args = calloc(no_words+1,sizeof(char*));

    char* name;
    char* last;
    name = strtok_r(string, " ", &last);
    (*args)[0] = calloc(strlen(name) + 1, sizeof(char*));
    strcpy((*args)[0],name);
    int i = 1;
    while(1){
        name = strtok_r(NULL, " ", &last);
        if(name == NULL){break;}
        (*args)[i] = calloc(strlen(name) + 1, sizeof(char*));
        strcpy((*args)[i], name);
        i++;
    }
    args[i] = NULL;
    str_repair(*args,no_words);

    return no_words;
}

void execute(char ** array, int len){

    char **args=NULL;
    int no_args;
    no_args = parse_command(array[0],&args);
    int fd[2];
    pipe(fd);


    pid_t pid = fork();
    pid_t last_pid = pid;

    if(pid == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        execvp(args[0], args);
        for(int j = 0; j < no_args;j++){
            free(args[j]);
        }
        free(args);
        exit(0);
    }

    for(int j = 0; j < no_args;j++){
        free(args[j]);
    }
    free(args);

    int fd_prev[2];
    for(int i = 1; i < len; i++){
        args = NULL;
        no_args = parse_command(array[i],&args);
        fd_prev[0]=fd[0];
        fd_prev[1]=fd[1];
        pipe(fd);
        pid = fork();
        if(pid>0){
            last_pid=pid;
        }
        else if(pid==0){
            dup2(fd_prev[0], STDIN_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd_prev[0]);
            close(fd_prev[1]);
            close(fd[0]);
            close(fd[1]);
            execvp(args[0], args);
            for(int j = 0; j < no_args;j++){
                free(args[j]);
            }
            free(args);
            exit(0);
        }
        close(fd_prev[0]);
        close(fd_prev[1]);
        for(int j = 0; j < no_args;j++){
            free(args[j]);
        }
        free(args);
    }
    close(fd_prev[0]);
    close(fd_prev[1]);
    close(fd[0]);
    close(fd[1]);
    waitpid(last_pid,NULL,0);

}




int main(int argc, char ** argv) {
    if(argc != 2){
        printf("there must be exactly one argument\n");
        return 1;
    }
    FILE *file = fopen(argv[1], "r");
    if(file == NULL){
        printf("null file\n");
        return 1;
    }

    //counting lines
    int no_cmds = 0;
    int no_tasks = 0;
    int flag = 0;

    int character = getc(file);
    int prev_character = character;
    while(character != EOF){
        if (character == '\n'){
            if(prev_character == '\n'){flag = 1;}
            else if(!flag){no_cmds++;}
            else{no_tasks++;}}
        prev_character = character;
        character = getc(file);
    }
    if(prev_character != '\n'){no_tasks++;}

    //parsing commands
    char ** cmd_names = calloc(no_cmds, sizeof(char*));
    char *** cmd_details = calloc(no_cmds, sizeof(char**));
    int * no_subcmds = calloc(no_cmds, sizeof(int));

    fseek(file, 0, SEEK_SET);
    char *line = NULL;
    size_t len = 0;
    char *cmd_name;
    char *last; //pointer needed for strtok_r as save_ptr
    int no_singles;
    for(int i = 0; i < no_cmds; i++){
        free(line);
        line = NULL;
        getline(&line, &len, file);
        no_singles = lc(line);
        cmd_name = strtok_r(line, "=", &last);
        cmd_names[i] = calloc(strlen(cmd_name) + 1, sizeof(char));
        strcpy(cmd_names[i], cmd_name);
        cmd_details[i] = calloc(no_singles+1, sizeof(char**));
        no_subcmds[i] = no_singles;
        int j = 0;
        while(1){
            cmd_name = strtok_r(NULL, "|", &last);
            if(cmd_name == NULL){break;}
            cmd_details[i][j] = calloc(strlen(cmd_name) + 1, sizeof(char*));
            strcpy(cmd_details[i][j], cmd_name);
            j++;
        }
    }



    // skipping the gap between commands and tasks
    getline(&line, &len, file);





    // parsing tasks AMONG US
    char *** sub_tasks = calloc(no_tasks, sizeof(char**));
    int * no_subtasks = calloc(no_tasks, sizeof(int));

    char * task_name;
    for(int i = 0; i < no_tasks; i++){
        free(line);
        line = NULL;
        getline(&line, &len, file);
        no_singles = lc(line);
        sub_tasks[i] = calloc(no_singles + 1, sizeof(char**));
        no_subtasks[i] = no_singles;

        int j = 0;
        task_name = strtok_r(line, "|", &last);
        while(task_name){
            sub_tasks[i][j] = calloc(strlen(task_name) + 1, sizeof(char*));
            strcpy(sub_tasks[i][j], task_name);
            j++;
            task_name = strtok_r(NULL, "|", &last);
        }
    }
    free(line);
    fclose(file);





    str_repair(cmd_names, no_cmds);
    char **to_exec;
    int no_to_exec;
    for(int i = 0; i < no_tasks; i++){
        str_repair(sub_tasks[i],no_subtasks[i]);
        no_to_exec = get_to_exec(no_subtasks[i], sub_tasks[i],
                                 cmd_details, cmd_names, no_cmds, no_subcmds,&to_exec);

        execute(to_exec, no_to_exec);
        for(int j = 0; j < no_to_exec; j++){
            free(to_exec[j]);
        }
        free(to_exec);
    }






    for (int i=0; i < no_cmds; i++){
        free(cmd_names[i]);
        for(int j=0; j < no_subcmds[i]; j++){
            free(cmd_details[i][j]);
        }
        free(cmd_details[i]);
    }
    free(cmd_names);
    free(cmd_details);
    free(no_subcmds);
    for (int i=0; i < no_tasks; i++){
        for(int j=0; j < no_subtasks[i]; j++){
            free(sub_tasks[i][j]);
        }
        free(sub_tasks[i]);
    }
    free(sub_tasks);
    free(no_subtasks);



    return 0;
}

