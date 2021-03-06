#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

//TO THE GRADER: this segfaults sometimes and I believe it's an issue connecting them at the inner loop in the connect() funciton

char path[PATH_MAX];

struct room{
    int id;
    int numcn;
    int curnm;
    char name[19];
    char type[19];
    int adj[7];
};

void makeroom(char *name, char *pth){
    char pthtmp[PATH_MAX] = "";
    sprintf(pthtmp,"%s%s",pthtmp,pth);
    strcpy(path,pthtmp);
    sprintf(pthtmp,"%s/%s_room.txt",pthtmp,name);
    FILE *newroom = fopen(pthtmp, "w");
    char namecpy[9];
    char towrt[19] = "ROOM NAME: ";
    strcpy(namecpy, name);
    sprintf(towrt,"%s%s\n",towrt, namecpy);
    fputs(towrt, newroom);
    fclose(newroom);
}

void writeconnect(int num, char *loc, char *dest){
    char pthtmp[PATH_MAX] = "";
    char desttmp[30];
    //printf(loc);
    //printf(dest);
    strcpy(pthtmp,path);
    sprintf(pthtmp,"%s/%s_room.txt",pthtmp,loc);
    FILE *cnrm = fopen(pthtmp, "a");
    fputs("CONNECTION ", cnrm);
    fprintf(cnrm,"%d", (num+1));
    fputs(": ", cnrm);
    strcpy(desttmp,dest);
    fputs(desttmp, cnrm);
    fputs("\n", cnrm);
    fclose(cnrm);
    return;
}

void writetype(char *type, char *room){
    char pthtmp[PATH_MAX] = "";
    sprintf(pthtmp,"%s%s",pthtmp,path);
    sprintf(pthtmp,"%s/%s_room.txt",pthtmp,room);
    FILE *cnrm = fopen(pthtmp, "a");
    fputs("ROOM TYPE: ",cnrm);
    fputs(type, cnrm);
    fclose(cnrm);
    return;
}

void connect(struct room rooms[]){
    int i,j,idx1,idx2;
    int temp = 0;
    int full = 0;
    for(i = 0; i < 7; i++){
         temp = rand() % 5 + 3;
         rooms[i].numcn = temp;
         rooms[i].curnm = 0;
    }
    printf("yo");
    while(temp == 0){
        int notempty = 0;
        idx1 = rand() % 7;
        if(rooms[idx1].curnm < rooms[idx1].numcn){
            while(notempty == 0){
                idx2 = rand() % 7;
                if(rooms[idx2].curnm < rooms[idx2].numcn){
                    i = rooms[idx1].curnm;
                    j = rooms[idx2].curnm;
                    rooms[idx1].adj[i] = idx2;
                    rooms[idx2].adj[j] = idx1;
                    rooms[idx1].curnm += 1;
                    rooms[idx2].curnm += 1;
                    printf(rooms[idx1].name);
                    printf(rooms[idx2].name);
                    writeconnect(rooms[idx1].curnm, rooms[idx1].name, rooms[idx2].name);
                    writeconnect(rooms[idx2].curnm, rooms[idx2].name, rooms[idx1].name);
                    for(i = 0; i < 7; i++){
                        if(rooms[i].curnm == rooms[i].numcn){
                            printf("all full");
                            temp = 1;
                            notempty = 1;
                        }
                        else{
                            temp = 0;
                        }
                    }
                }
            }
        }
        
    }
    for(i = 0; i < 7; i++){
        if(i==0){
            writetype("START_ROOM",rooms[i].name);
        }
        else if(i==6){
            writetype("END_ROOM",rooms[i].name);
        }
        else{
            writetype("MID_ROOM",rooms[i].name);
        }
    }
    return;
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
    struct room selec[8];
    int i,j,temp,temp2;
    int unique = 0;
    srand(time(0));
    for(i = 0; i < 7; i++){
        temp = rand() % (index+1);
        temp2 = ops[temp];
        makeroom(names[temp2], lis);
        selec[i].id = i;
        strcpy(selec[i].name,names[temp2]);
        //generate random number in range of ops array, shift all values left and decrement index.
        for(j = temp; j < index; j++){
            ops[j] = ops[j+1];
        }
        index = index - 1;
    }
    connect(selec);
    return 0;
}
