#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define delim " \t\r\n\a"
int globstat = 0;

void debug(char **input, int num){
    int i;
    for(i=0;i<num;i++){
        printf("%s\n", input[i]);
    }
}

int mystatus(char **input){
    printf("%s %d\n","exit value",globstat);
    return 0;
}

int mycd(char **input){
    if(chdir(input[1])){
        printf("Error changing directories\n");
    }
    return 0;
}

int myexit(char **input){
    exit(0);
    return 1;
}

int (*natives[])(char**) = {&mycd,&myexit,&mystatus};
char *native[] = {"cd","exit","status"};

int process(char **input){
    //similar to fork and execlp example in lecture
    int pstat = -5;
    pid_t mypid = -5;
    pid_t myotherpid = -5;
    mypid = fork();
    switch(mypid){
        case -1: {
            perror("Something went wrong\n");
            return 1;
            break;
        }
        case 0: {
            globstat = execvp(input[0], input);
            perror("-There was a problem with that command-\n");
            return 0;
            break;
        }
        default: {
            myotherpid = waitpid(mypid, &pstat, WUNTRACED);
            return 0;
            break;
        }
    }
}

int mountain_doit(char **input, int num){
    //array kept getting filled with trash after freeing not sure why
    //used the index created by strtok and a loop in main
    if(num == 0){
        printf("Please give an argument\n");
        return 0;
    }
    int i;
    for(i = 0; i < 3; i++){
        if(!strcmp(input[0], native[i])){
            return (*natives[i])(input);
        }
    }
    return process(input);
}

void main(){
    int stat = 0;
    do{
        char **output = malloc(sizeof(char*));
        char *input;
        ssize_t buf = 0;
        int index = 0;
        char *arg = NULL;
        printf(": ");
        //get raw user input
        getline(&input, &buf, stdin);
        //parse user input
        arg = strtok(input, delim);
        while(arg != NULL){
            output[index] = arg;
            index += 1;
            arg = strtok(NULL, delim);
            //allocate more memory as the tokens are read
            output = realloc(output, index *sizeof(char*));
        }
        if(!output){
            printf("Something went wrong parsing commands\n");
        }
        //if everything seems ok we'll go ahead and try to run it
        else{
            stat = mountain_doit(output, index);
        }
        //free our variables
        free(arg);
        free(input);
        free(output);
        int i;
        for(i = 0;i<index;i++){
            output[i]=NULL;
        }
   }while(stat == 0);
   return;
}