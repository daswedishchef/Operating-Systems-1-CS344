#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char path[PATH_MAX];

void makeroom(char *name, char *pth){
    char pthtmp[PATH_MAX] = "";
    sprintf(pthtmp,"%s%s",pthtmp,pth);
    strcpy(path,pthtmp);
    sprintf(pthtmp,"%s/%s_room.txt",pthtmp,name);
    FILE *newroom = fopen(pthtmp, "w");
    char namecpy[8];
    char towrt[18] = "ROOM NAME: ";
    strcpy(namecpy, name);
    sprintf(towrt,"%s%s\n",towrt, namecpy);
    fputs(towrt, newroom);
    fputs("\n", newroom);
    fclose(newroom);
}

void writeconnect(int num, char *loc, char *dest){
    char pthtmp[PATH_MAX] = "";
    sprintf(pthtmp,"%s%s",pthtmp,path);
    sprintf(pthtmp,"%s/%s_room.txt",pthtmp,loc);
    FILE *cnrm = fopen(pthtmp, "a");
    fputs("CONNECTION ", cnrm);
    fprintf(cnrm,"%d", (num+1));
    fputs(": ", cnrm);
    fputs(dest, cnrm);
    fputs("\n", cnrm);
    fclose(cnrm);
}

void writetype(char *type, char *room){
    char pthtmp[PATH_MAX] = "";
    sprintf(pthtmp,"%s%s",pthtmp,path);
    sprintf(pthtmp,"%s/%s_room.txt",pthtmp,room);
    FILE *cnrm = fopen(pthtmp, "a");
    fputs("ROOM TYPE: ",cnrm);
    fputs(type, cnrm);
    fputs("\n", cnrm);
    fclose(cnrm);
}

void connect(char *rooms[]){
    int cnnct[8];
    int count[8];
    int cnns[8][8];
    int i,j,idx1,idx2,temp;
    int full = 0;
    for(i = 0; i < 7; i++){
         temp = rand() % 5 + 3;
         cnnct[i] = temp;
         count[i] = temp;
    }
    while(full!=1){
        idx1 = 7;
        idx2 = 7;
        for (i = 0; i < 6; i++){
            if (cnnct[i] > 0){
                //room A
                idx1 = i;
                for (j = 0; j < 6; j++){
                    if ((cnnct[j] > 0)&&(j != i)){
                        //room B
                        idx2 = j;
                        cnns[i][cnnct[i]] = j;
                        cnnct[i] -= 1;
                        cnns[j][cnnct[j]] = i;
                        cnnct[j] -= 1;
                    }
                }
            }
        }
        //all rooms have max connections
        if(idx1 == 7 || idx2 == 7){
            full = 1;
        }
    }
    for(i = 0; i < 7; i++){
        for(j = 0; j < count[i]; j++){
            int dest = cnns[i][j];
            
            writeconnect(j, rooms[i], rooms[dest]);
        }
        if(i==0){
            writetype("START_ROOM",rooms[i]);
        }
        else if(i==6){
            writetype("END_ROOM",rooms[i]);
        }
        else{
            writetype("MID_ROOM",rooms[i]);
        }
    }
}

int main(){
    int pid = getpid();
    char lis[50] = "carlsosp.rooms.";
    sprintf(lis,"%s%d",lis,pid);
    if(mkdir(lis, 0755) == 0){
    }
    else{
        printf("Directory creation issue");
        return 0;
    }
    char *names[10] = {"XXYZY", "Dungeon", "Library", "Chapel", "Cube", "Beaver", "Subway", "Shop", "Torture", "Attic"};
    //could use dynamic array here but eh
    int ops[10] = {0,1,2,3,4,5,6,7,8,9};
    int index = 9;
    char *selec[7];
    int i,j,temp,temp2;
    int unique = 0;
    srand(time(0));
    for(i = 0; i < 7; i++){
        temp = rand() % (index+1);
        temp2 = ops[temp];
        makeroom(names[temp2], lis);
        selec[i] = names[temp2];
        //generate random number in range of ops array, shift all values left and decrement index.
        for(j = temp; j < index; j++){
            ops[j] = ops[j+1];
        }
        index = index - 1;
    }
    connect(selec);
}
