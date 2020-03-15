#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <stdbool.h>

void error(const char *msg) { perror(msg); } // Error function used for reporting issues

bool wait_for_success(int socketFD) {
	int charsRead;
	char buffer[16];
	printf("Client: Waiting for success\n");
	memset(buffer, '\0', 16);
	charsRead = recv(socketFD, buffer, 16, 0);
	if (charsRead < 1 || strcmp(buffer, "success"))
	{
		fprintf(stderr, "Client: Server did not respond. It instead said: %s with %d\n", buffer, charsRead);
		return false;
	}
	printf("Client: Got for success\n");
	return true;
}

int main(int argc, char *argv[])
{
	int socketFD, portNumber, charsWritten, charsRead;
	struct sockaddr_in serverAddress;
	struct hostent* serverHostInfo;
	char *buffer;
    
	if (argc < 4) { fprintf(stderr,"USAGE: %s plainttext key port\n", argv[0]); exit(0); } // Check usage & args

	// Set up the server address struct
	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
	if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) error("CLIENT: ERROR opening socket");
	
	// Connect to server
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to address
		error("CLIENT: ERROR connecting");

	// Get return message from server
	buffer = malloc(16*sizeof(char));
	memset(buffer, '\0', 4*sizeof(char)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, buffer, 256, 0); // Read data from the socket, leaving \0 at end
	if (charsRead < 3) error("CLIENT: ERROR reading from socket");
	if(!strcmp(buffer,"dec")){
		// Send message to server
		int keynum,txnum;
		char temp;
		FILE *mytext;
		FILE *mykey;
		mytext = fopen(argv[1],"r");
		if(mytext==NULL){
			fprintf(stderr,"Unable to read plaintext\n");
			return 1;
		}
		txnum = 0;
		while(fgetc(mytext)!=EOF){
			txnum++;
		}
		mykey = fopen(argv[2],"r");
		if(mykey==NULL){
			fprintf(stderr,"Unable to read plaintext\n");
			return 1;
		}
		keynum = 0;
		while(fgetc(mykey)!=EOF){
			keynum++;
		}
		if(keynum<txnum){
			fprintf(stderr, "text cannot be longer than text\n");
			return 1;
		}
		char input[24];
		sprintf(input,"%d",keynum);
		//do{
		charsWritten = send(socketFD, input, 24, 0);
		//}while(charsWritten < keynum);
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		memset(buffer,'\0',16);
		charsRead = recv(socketFD, buffer, 16, 0);
		if(strcmp(buffer,"success")){
			fprintf(stderr,"Client: Server did not respond\n");
			return 1;
		}
		free(buffer);
		buffer = calloc(keynum,(keynum+1)*sizeof(char));
		rewind(mykey);
		char *textbuff;
		textbuff = malloc((keynum+1)*sizeof(char));
		fgets(textbuff,keynum,mykey);
		//printf("sending: %s\n",textbuff);
		charsWritten = send(socketFD, textbuff, keynum, 0);
		//printf("Client: waiting for success");
		memset(buffer,'\0',16);
		charsRead = recv(socketFD, buffer, 16, 0);
		if(strcmp(buffer,"success")){
			fprintf(stderr,"Client: Server did not respond\n");
			return 1;
		}
		//send text
		memset(input, '\0', sizeof(input));
		sprintf(input,"%d",txnum);
		//printf("sending: %s\n",input);
		charsWritten = send(socketFD, input, 24, 0);
		if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
		memset(buffer,'\0',16);
		//printf("Client: waiting for success");
		charsRead = recv(socketFD, buffer, 16, 0);
		if(strcmp(buffer,"success")){
			fprintf(stderr,"Client: Server did not respond\n");
			return 1;
		}
		rewind(mytext);
		char *pbuff;
		pbuff = malloc((txnum+1)*sizeof(char));
		fgets(pbuff,txnum,mytext);
		//printf("sending: %s\n",pbuff);
		charsWritten = send(socketFD, pbuff, txnum, 0);
		memset(pbuff,'\0',txnum);
		charsRead = recv(socketFD, pbuff, txnum-1, 0);
		if(charsRead < 0) error("CLIENT: ERROR retrieving cyphertext");
		pbuff[txnum-1] = '\n';
		fprintf(stdout,pbuff);
		fclose(mytext);
		fclose(mykey);
		//free(pbuff);
		//free(textbuff);
		free(buffer);
	}
	else{
		fprintf(stderr,"This program connects exlusively with otp_enc_d, port: %d\n",portNumber);
		return 1;
	}
	//printf("\nclient gang out\n");
	close(socketFD); // Close the socket
	return 0;
}