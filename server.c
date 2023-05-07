// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <sys/types.h> 
#include <sys/socket.h> 



char rem[1024*8 +1];


void binaryToString(char* input, char* output){

    char binary[9] = {0}; // initialize string to 0's

    // copying 8 bits from input string
    for (int i = 0; i < 8; i ++){
        binary[i] = input[i];    
    }

    *output  = strtol(binary,NULL,2); // convert the byte to a long
}

void CRC(char *input, char* key){
	int i,j,keylen,msglen;
	char temp[strlen(key)+1];
	char quotient[strlen(input)+1];
	char key1[strlen(key)+1];

	keylen=strlen(key);
	msglen=strlen(input);
	strcpy(key1,key);
	for (i=0;i<keylen-1;i++) {
		input[msglen+i]='0';
	}
	for (i=0;i<keylen;i++)
	 temp[i]=input[i];
	for (i=0;i<msglen;i++) {
		quotient[i]=temp[0];

		if(quotient[i]=='0')
		 for (j=0;j<keylen;j++)
		 key[j]='0'; else
		 for (j=0;j<keylen;j++)
		 key[j]=key1[j];

		for (j=keylen-1;j>0;j--) {
			if(temp[j]==key[j])
			 rem[j-1]='0'; else
			 rem[j-1]='1';
		}
		rem[keylen-1]=input[i+keylen];
		strcpy(temp,rem);
	}
	strcpy(rem,temp);

	rem[keylen-1] = '\0';
}

// Driver code

int main(int argc, char* argv[]) { 
    int PORT=atoi(argv[1]);
	int sockfd; 
	char buffer[2*(1024*8+1)]; 
	char bin_message[1024*8+1];
	char message[1024];

	char generator[19];
	char *reply; 
	struct sockaddr_in servaddr, cliaddr; 
	
	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	memset(&servaddr, 0, sizeof(servaddr)); 
	memset(&cliaddr, 0, sizeof(cliaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; // IPv4 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	servaddr.sin_port = htons(PORT); 
	
	// Bind the socket with the server address 
	if ( bind(sockfd, (const struct sockaddr *)&servaddr, 
			sizeof(servaddr)) < 0 ) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	int len, n; 

	len = sizeof(cliaddr); //len is value/resuslt 

	n = recvfrom(sockfd, (char *)generator, 18, 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 

	generator[n] = '\0'; 
	
	n = recvfrom(sockfd, (char *)buffer, sizeof(buffer), 
				MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
				&len); 
	buffer[n] = '\0'; 
	printf("Received Message in binary from the client (with CRC at the end):\n%s\n\n", buffer); 

	int i=0;
	for(i=0; i <= strlen(buffer)-strlen(generator); i++){
		bin_message[i] = buffer[i];
	}

	bin_message[i] = '\0';

	for (i = 0; i < (strlen(bin_message)-1); i++){
        binaryToString(&bin_message[i*8], &message[i]);
    }

	message[i] = '\0';

	printf("Received generator polynomial from the client: %s\n\n", generator); 

	CRC(buffer, generator);
	printf("Remainder: %s\n\n", rem);

	printf("Decoded Message in ASCII text: %s\n", message);


	if(!strcmp(rem, "0000000000000000")){
		printf("Sending response to client : No Error in transmission\n");
		reply = "No Error in transmission";
	}
	else{
		reply = "Error in transmission"; 
		printf("Error in transmission\n");
	}
	sendto(sockfd, (const char *)reply, strlen(reply), 
		0, (const struct sockaddr *) &cliaddr, 
			len); 
	
	return 0; 
} 
