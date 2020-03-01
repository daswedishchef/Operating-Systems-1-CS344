#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
//global vars
#define delim " \t\r\n\a"
int globstat = 0;
int engv = 0;

void debug(char **input, int num){
    int i;
    for(i=0;i<num;i++){
        printf("%s\n", input[i]);
    }
}
//Built-in commands
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


//Execution function
int process(char **input, int bkfl){
    //similar to fork and execlp example in lecture
    int pstat = -5;
    pid_t mypid = -5;
    pid_t myotherpid = -5;
    mypid = fork();
    switch(mypid){
        case -1: {
            //error case
            return 1;
            break;
        }
        case 0: {
            //child goes here
            globstat = 0;
            globstat = execvp(input[0], input);
            perror("");
            exit(0);
            return 0;
        }
        default: {
            //parent goes here
            if((bkfl==0)||(engv==1)){
                //parent waits for foreground process
                myotherpid = waitpid(mypid, &pstat, WUNTRACED);
                return 0;
            }
            //starts process in background
            else if((bkfl==1)&&(engv==0)){
                //ISSUE - child becomes zombie after completing
                printf("process supposed to start in background\n");
                myotherpid = waitpid(mypid, &pstat, WNOHANG);
                return 0;
            }
        }
    }
}
//This function stages the execution
int mountain_doit(char **input, int num){
    //check to see if user entered anything
    if(num == 0){
        printf("Please give an argument\n");
        return 0;
    }
    //check if user entered built in command
    int i;
    for(i = 0; i < 3; i++){
        if(!strcmp(input[0], native[i])){
            return (*natives[i])(input);
        }
    }
    //check for $$
    pid_t expans;
    for(i = 0;i < num; i++){
        //printf("%s\n", input[i]);
        if(!strcmp(input[i], "$$")){
            sprintf(input[i], "%d", getpid());
        }
    }
    if(!strcmp(input[0], "#")){
        return 0;
    }
    //check for background specification
    if(!strcmp(input[num-1], "&")){
        input[num-1]=NULL;
        input[num]=NULL;
        return process(input,1);
    }
    else{
        return process(input,0);
    }
}
//signal handlers
void inthand(int sig){
    printf(" cntrl+c recieved\n");
}
void stphand(int sig){
    if(engv==1){
        char* msg1 = "\n-Exiting foreground only mode-\n";
        write(STDOUT_FILENO, msg1, 36);
        engv = 0;
    }
    else if(engv==0){
        char* msg2 = "\n-Entering foreground only mode-\n";
        write(STDOUT_FILENO, msg2, 37);
        engv = 1;
    }
}
//Main reads line, calls functions, and frees memory
void main(){
    int stat = 0;
    char **output = malloc(sizeof(char*));
    signal(SIGTSTP, stphand);
    do{
        output = realloc(output, sizeof(char*));
        char *input;
        ssize_t buf = 0;
        int index = 0;
        char *arg = NULL;
        printf(": ");
        //get raw user input
        getline(&input, &buf, stdin);
        //parse user input
        arg = strtok(input, delim);
        index = 0;
        while(arg != NULL){
            output[index] = arg;
            index += 1;
            arg = strtok(NULL, delim);
            //allocate more memory as the tokens are read
            output = realloc(output, (index+1) *sizeof(char*));
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
        int i;
        for(i = 0;i<index+1;i++){
            output[i]=NULL;
        }
   }while(stat == 0);
   return;
}