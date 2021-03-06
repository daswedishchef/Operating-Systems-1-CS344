#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
//global vars
char ops[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
int numchild = 0;

void chldhand(int signal){
	waitpid((pid_t)(-1),0,WNOHANG);
	numchild--;
}

void send_success(int establishedConnectionFD) {
	printf("SERVER: sending success\n");
	send(establishedConnectionFD, "success", 16, 0);
}

char toOps(int num){
	if(num>27){
		return '0';
	}
	return ops[num];
}

int opsout(char op){
	int i; 
	i=0;
	for(i=0;i<27;i++){
		//printf("%d",i);
		if(ops[i]==op){
			return i;
		}
	}
}

void error(const char *msg) { perror(msg); } // Error function used for reporting issues

int main(int argc, char *argv[])
{
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead, i;
	socklen_t sizeOfClientInfo;
	char *buffer;
	struct sockaddr_in serverAddress, clientAddress;

	if (argc < 1) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) error("ERROR opening socket");

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
		error("ERROR on binding"),exit(0);
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	pid_t mypid = -5;
	pid_t myotherpid = -5;
	int pstat = -5;
	while(1){
		// Accept a connection, blocking if one is not available until one connects
		do{
		}while(numchild>=5);
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		charsRead = send(establishedConnectionFD, "dec", 4, 0);
		if (charsRead < 0) error("ERROR writing to socket");
		mypid = fork();
		if(mypid<0){
			error("Error creating process");
		}
		else if(mypid == 0){
			//get size of key
			buffer = calloc(256,256);
			charsRead = recv(establishedConnectionFD, buffer, 24, 0);
			int len;
			len = atoi(buffer);
			//send success for key size
			charsRead = send(establishedConnectionFD, "success", 16, 0);
			//allocate buffer for key and get it
			free(buffer);
			buffer = calloc(len,(len+1)*sizeof(char));
			charsRead = 0;
			i = 0;
			do{
				charsRead += recv(establishedConnectionFD, &buffer[i], 1, 0);
				i++;
			}while(i < len);
			//copy key from buffer into mykey
			char *mykey;
			mykey = malloc((len+1)*sizeof(char));
			strcpy(mykey,buffer);
			//printf("key %p: %s\n",mykey,mykey);
			//send success for key
			charsRead = send(establishedConnectionFD, "success", 16, 0);

			//get size of text
			int len2;
			free(buffer);
			buffer = calloc(256, 256);
			charsRead = recv(establishedConnectionFD, buffer, 24, 0);
			if(charsRead<0) error("Error receiving data\n");
			len2 = atoi(buffer);
			free(buffer);
			buffer = calloc(len2,(len2+1)*sizeof(char));
			//send success for size
			charsRead = send(establishedConnectionFD, "success", 16, 0);
			//read text
			charsRead = 0;
			i = 0;
			do{
				charsRead += recv(establishedConnectionFD, &buffer[i], 1, 0);
				i++;
			}while(i < len2);
			//copy text into plaintext
			char *plaintext;
			plaintext = malloc((len2+1)*sizeof(char));
			strcpy(plaintext,buffer);
			//printf("SERVER - cyphertext: %s\n",plaintext);
			int i,c;
			int temp,temp2;
			char *cyphertext;
			cyphertext = malloc((len2+1)*sizeof(char));

			//encryption

			int tempc;
			c=0;
			for(i=0;i<len2;i++){
				temp = 0;
				temp2 = 0;
				tempc = 0;
				if(plaintext[i]=='\n'){
					break;
				}
				else{
					temp = (opsout(plaintext[i]) - opsout(mykey[c]));
                    if(temp < 0){
                        temp+=27;
                    }
					//fixed edge case when key and text have space at same index
					if(temp < 0){
                        temp+=27;
                    }
					cyphertext[i] = toOps(temp);
					if(c==len){
						c=0;
					}
				}
				c++;
			}
			cyphertext[len2] = '\0';
			//free and exit child
			//printf("SERVER - plaintext: %s\n",cyphertext);
			charsRead = 0;
			i = 0;
			do{
				charsRead += send(establishedConnectionFD, &cyphertext[i], 1, 0);
				i++;
			}while(i < len2);
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			free(mykey);
			free(buffer);
			free(plaintext);
			exit(0);
		}
		else{
			//increment number of children processes and reap children
			numchild++;
			close(establishedConnectionFD);
			signal(SIGCHLD, chldhand);
			myotherpid = waitpid(mypid, &pstat, WNOHANG);
		}
	}
	close(listenSocketFD); // Close the listening socket
	return 0;
}