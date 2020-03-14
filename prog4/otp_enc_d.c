#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>

char ops[27] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

char toOps(int num){
	if(num>27){
		fprintf(stderr,"error converting int");
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
	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
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
		error("ERROR on binding");
	listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
	pid_t mypid = -5;
	pid_t myotherpid = -5;
	int pstat = -5;
	int numchild = 0;
	while(1){
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) error("ERROR on accept");
		charsRead = send(establishedConnectionFD, "enc", 4, 0);
		if (charsRead < 0) error("ERROR writing to socket");
		mypid = fork();
		if(mypid<0){
			error("Error creating process");
		}
		else if(mypid == 0){
			//get key
			buffer = calloc(256,256);
			charsRead = recv(establishedConnectionFD, buffer, 24, 0);
			int len;
			len = atoi(buffer);
			printf("\nlen: %d\nchars: %d",len,charsRead);
			printf("SERVER: I received this from the client: \"%s\"\n", buffer);
			charsRead = send(establishedConnectionFD, "success", 16, 0);
			free(buffer);
			//printf("\nlen: %d\n", len);
			buffer = calloc(len,(len+1)*sizeof(char));
			charsRead = recv(establishedConnectionFD, buffer, len, 0);
			if(charsRead<len) error("Error receiving data\n");
			//printf("\nline%d: %d\n",__LINE__,charsRead);
			char *mykey;
			mykey = malloc((len+1)*sizeof(char));
			//printf("\nline%d: %d\n",__LINE__,charsRead);
			strcpy(mykey,buffer);
			printf("key %p: %s\n",mykey,mykey);
			charsRead = send(establishedConnectionFD, "success", 16, 0);

			//get text
			int len2;
			free(buffer);
			buffer = calloc(256, 256);
			//printf("server: im getting size of next thing");
			charsRead = recv(establishedConnectionFD, buffer, 24, 0);
			if(charsRead<0) error("Error receiving data\n");
			//printf("\nline%d: %d\n",__LINE__,charsRead);
			len2 = atoi(buffer);
			printf("SERVER: I received this from the client: \"%s\"\n", buffer);
			//printf("\nfree on: %d\n",__LINE__);
			free(buffer);
			buffer = calloc(len2,(len2+1)*sizeof(char));
			charsRead = send(establishedConnectionFD, "success", 16, 0);
			//printf("server: im getting size of next thing1");
			charsRead = recv(establishedConnectionFD, buffer, len2, 0);
			//printf("\nline%d: %d\n",__LINE__,charsRead);
			//printf("\nerrno: %d\n",errno);
			//printf("\nlen: %d\n",len);
			if(charsRead<len2) error("Error receiving data\n");
			char *plaintext;
			plaintext = malloc((len2+1)*sizeof(char));
			strcpy(plaintext,buffer);
			printf("plaintext: %s\n",plaintext);
			int i,j,c,asc;
			int temp,temp2;
			char *cyphertext;
			cyphertext = malloc((len2+1)*sizeof(char));
			printf("\nlen: %d\n",len2);
			
			int randkey, tempc;
			c=0;
			temp = 0;
			temp2 = 0;
			tempc = 0;
			for(i=0;i<len2;i++){
				if(plaintext[j]=='\n'){
					break;
				}
				else{
					temp = opsout(plaintext[j]);
					temp2 = opsout(mykey[c]);
					temp += temp2;
					if(temp>26){
						temp = temp - 26;
					}
					//printf("\n%d ", temp);
					//printf(" %d %d\n",temp2,i);
					plaintext[j] = toOps(temp);
					if(c==len){
						c=0;
					}
				}
				c++;
			}
			printf("cyphertext: %s\n",plaintext);
			close(establishedConnectionFD); // Close the existing socket which is connected to the client
			//printf("\nfree on: %d\n",__LINE__);
			free(mykey);
			//printf("\nfree on: %d\n",__LINE__);
			free(buffer);
			//printf("\nfree on: %d\n",__LINE__);
			free(plaintext);
			exit(0);
		}
		else{
			numchild++;
			close(establishedConnectionFD);
			signal(SIGCHLD, SIG_IGN);
			myotherpid = waitpid(mypid, &pstat, WNOHANG);
		}
	}
	close(listenSocketFD); // Close the listening socket
    //printf("%s\n","exiting");
	return 0;
}