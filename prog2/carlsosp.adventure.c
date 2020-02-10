#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

char pth[PATH_MAX];
char *where[30];
int steps = 0;

struct rooms{
    int numcon;
    char *connections[6];
    char name[30];
    char type[18];
};

void begin(char path[]);

int checkname(){
    char start[100];
    scanf("%s", start);
    struct dirent *fileInDir;
    struct stat dirAttributes;
    char pthtmp[PATH_MAX];
    DIR* roomdir = opendir(pth);
    if(roomdir > 0){
        while ((fileInDir = readdir(roomdir)) != NULL){
            stat(fileInDir->d_name, &dirAttributes);
                strcpy(pthtmp, pth);
                sprintf(pthtmp,"%s/%s_room.txt",pthtmp,start);
                FILE * isfile = fopen(pthtmp, "r");
                if(isfile == NULL){
                    printf("HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.");
                    printf("\n");
                    return 1;
                }
                else{
                    printf("\n");
                    where[steps] = start;
                    begin(pthtmp);
                    return 0;
                }
        }
    }
    else{
        printf("directory fail");
        return 2;
    }
}

void turn(){
    do{
        printf("WHERE TO? >");
    }while(checkname()>0);
}

int haswon(char path[]){
    char end[4] = "END";
    char output[30];
    char *vld;
    FILE * myfile = fopen(path, "r");
    while(fgets(output, 30, myfile)){
        vld = strstr(output, end);
        if(vld != NULL){
            fclose(myfile);
            return 1;
        }
    }
    return 0;
}

void begin(char path[]){
    int i;
    if(haswon(path) == 1){
        printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK ");
        printf("%d",steps);
        printf(" STEPS. YOUR PATH TO VICTORY WAS:");
        for(i = 0; i < steps+1; i++){
            printf(where[i]);
            printf("\n");
        }
        return;
    }
    steps += 1;
    FILE *ptr = fopen(path, "r");
    char output[30];
    char test[11] = "CONNECTION";
    int iscon = 1;
    fgets(output, 30, ptr);
    printf("CURRENT LOCATION: ");
    size_t szl = strlen(output);
    memmove(output, output+11, szl - 10);
    printf(output);
    strcpy(output,"");
    printf("POSSIBLE CONNECTIONS: ");
    while(fgets(output, 30, ptr)){
        //printf(output);
        if(strstr(output,test)){
            szl = strlen(output);
            memmove(output, output + 14, szl - 13);
            output[strlen(output)-1] = '\0';
            printf(output);
            printf(", ");
        }
    }
    printf("\n");
    turn();
}

int main(){
    char *path[30];
    int steps = 0;
    int newestDirTime = -1; // Modified timestamp of newest subdir examined

    //Using code from example to find newest directory, hope this is ok I was unsure about what exactly it meant in the syllabus but piazza said it was ok

    char targetDirPrefix[32] = "carlsosp.rooms."; // Prefix we're looking for
    char newestDirName[256]; // Holds the name of the newest dir that contains prefix
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR* dirToCheck; // Holds the directory we're starting in
    struct dirent *fileInDir; // Holds the current subdir of the starting dir
    struct stat dirAttributes; // Holds information we've gained about subdir

    dirToCheck = opendir("."); // Open up the directory this program was run in

    if (dirToCheck > 0) // Make sure the current directory could be opened
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
        {
        if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
        {
            //printf("Found the prefex: %s\n", fileInDir->d_name);
            stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

            if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
            {
            newestDirTime = (int)dirAttributes.st_mtime;
            memset(newestDirName, '\0', sizeof(newestDirName));
            strcpy(newestDirName, fileInDir->d_name);
            //printf("Newer subdir: %s, new time: %d\n",
            //        fileInDir->d_name, newestDirTime);
            }
        }
        }
    }
    char pthtmp[PATH_MAX];
    char output[30];
    char start[5] = "START";
    char *vld;
    DIR* roomdir = opendir(newestDirName);
    if(roomdir > 0){
        while ((fileInDir = readdir(roomdir)) != NULL){
            stat(fileInDir->d_name, &dirAttributes);
                strcpy(pthtmp, newestDirName);
                strcpy(pth,pthtmp);
                sprintf(pthtmp,"%s/%s",pthtmp,fileInDir->d_name);
                FILE * myfile = fopen(pthtmp, "r");
                while(fgets(output, 30, myfile)){
                    vld = strstr(output, start);
                    if(vld != NULL){
                        fclose(myfile);
                        closedir(roomdir);
                        begin(pthtmp);
                        closedir(dirToCheck);
                        return 0;
                    }
                }
                strcpy(output,"");
        }
    }
     // Close the directory we opened
    //printf("Newest entry found is: %s\n", newestDirName);
}