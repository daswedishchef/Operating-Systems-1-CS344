#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void main(int argc, char** argv){
    srand(time(NULL));
    if(argc<2){
        fprintf(stderr,"Not enough arguments");
        return;
    }
    else{
        int i;
        char *key;
        char temp;
        int numkey;
        numkey = atoi(argv[1]);
        key = malloc(numkey+1*sizeof(char*));
        char *ops;
        ops = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
        int randkey;
        for(i = 0;i<numkey;i++){
            randkey = rand() % 27;
            temp = ops[randkey];
            key[i] = temp;
        }
        key[numkey] = '\n';
        fprintf(stdout,key);
        free(key);
        return;
    }
}