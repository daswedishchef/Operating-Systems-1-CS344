#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>

#define delim " \t\r\n\a"
int globstat = 0;

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
    return 1;
}

int (*natives[])(char**) = {&mycd,&myexit,&mystatus};

int mountain_doit(char **input, int num){
    int i;
    char *native[] = {"cd","exit","status"};
    if(num == 0){
        printf("Please give an argument\n");
        return 1;
    }
    for(i = 0; i < 3; i++){
        if(!strcmp(input[0], native[i])){
            return (*natives[i])(input);
        }
    }
    return 0;
}

void main(){
    int stat = 0;
    do{
        char *input;
        ssize_t buf = 0;
        int buffer = 64;
        int index = 0;
        char **output = malloc(buffer * sizeof(char*));
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
            //allocate more memory if needed
            if(index == buffer){
                buffer += 64;
                output = realloc(output, buffer *sizeof(char*));
            }
        }
        if(!output){
            printf("Something went wrong parsing commands");
            exit(EXIT_FAILURE);
        }
        //if everything seems ok we'll go ahead and try to run it
        else{
            stat = mountain_doit(output, index);
        }
        //free our variables
        free(arg);
        free(input);
        free(output);
   }while(stat == 0);
   exit(EXIT_SUCCESS);
}