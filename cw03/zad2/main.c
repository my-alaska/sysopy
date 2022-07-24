#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/times.h>

static clock_t clock_start, clock_end;
static struct tms tms_start, tms_end;

//should print pi
int main(int argc, char ** argv) {
    #ifdef T
    clock_start = times(&tms_start);
    #endif

    long double w = atof(argv[1]);
    int n = atoi(argv[2]);
    long int nrect = round(1/w);

    long int all = nrect/n;
    long int rest = nrect%n;
    long double x = 0;
    long int partn;
    long double result = 0;
    char name[100];
    char txt[] = ".txt";
    FILE *fptr;
    char number[50];
    char * buffer;
    long length;
    char sn[10];

    long int partrecn;
    long double val = 0.0;
    char filename[20];
    char strval[50];
    pid_t child_pid;
    int p_number = 0;
    int flag = 0;
    for(int i = 0; i < n; i++){
        p_number++;
        if(i < rest){
            partn = all+1;
            child_pid = fork();
            if(child_pid==0){
                flag = 1;
                break;
            }
            x+=w*(all+1);
        }else{
            partn = all;
            child_pid = fork();
            if(child_pid==0){
                flag = 2;
                break;
            }
            x+=w*all;
        }
    }



    if(flag == 0){
        for(int i = 0; i < n; i++){
            wait(NULL);
        }
        for(int i; i < n; i++){
            strcpy(name,"results/w");
            sprintf(sn,"%d",i+1);
            strcat(name,sn);
            strcat(name,txt);
            fptr = fopen(name,"r");
            fseek (fptr, 0, SEEK_END);
            length = ftell(fptr);
            fseek (fptr, 0, SEEK_SET);
            buffer = malloc (length);
            fread (buffer, 1, length, fptr);
            fclose (fptr);
            result += atof(buffer);

            free(buffer);
        }

        #ifndef T
        printf("%.30Lf",result);
        #endif

        #ifdef T
        clock_end = times(&tms_end);
        printf("\nwidth:  %s - n: %s\n",argv[1],argv[2]);
        printf("system: %fs\n", ((double) (tms_end.tms_cstime - tms_start.tms_cstime))/(double)sysconf(_SC_CLK_TCK));
        printf("user:   %fs\n", ((double)(tms_end.tms_cutime - tms_start.tms_cutime))/(double)sysconf(_SC_CLK_TCK));
        printf("real:   %fs\n", ((double)(clock_end - clock_start))/(double)sysconf(_SC_CLK_TCK));
        #endif
        return 0;
    }


    if (flag==1){
        partrecn = all+1;
    }else{
        partrecn = all;
    }

    for(int i = 0; i < partrecn; i++){
        val += w*4/(x*x+1);
        x += w;
    }


    sprintf(filename,"./results/w%d.txt",p_number);
    FILE *fp = fopen(filename, "w+");

    sprintf(strval,"%.30Lf",val);

    fwrite(strval,1,strlen(strval),fp);

    return 0;
}

