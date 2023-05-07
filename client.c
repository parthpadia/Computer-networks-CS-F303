// Clinet implementation
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h> 
#include <netinet/in.h>  
#include <sys/types.h> 
#include <sys/socket.h> 


char rem[1024*8 +1];
char out_string[2*(1024*8+1)];


char* stringToBinary(char* s) {
    if(s == NULL) return 0; /* no input string */
    size_t len = strlen(s);
    char *binary = malloc(len*8 + 1); // each char is one byte (8 bits) and + 1 at the end for null terminator
    binary[0] = '\0';
    for(size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
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
	
	// printf("\nRemainder is ");
	// for (i=0;i<keylen-1;i++)
	//  printf("%c",rem[i]);
	
	// printf("\nFinal data is: ");
	for (i=0;i<msglen;i++)
		out_string[i] = input[i];
	
	for (j=0;j<keylen-1;j++)
	{
		out_string[i] = rem[j];
		i++;
	} 	


	 out_string[i] = '\0';
	// printf("\n");
}



// Driver code 
int main(int argc, char *argv[]) { 
    int PORT=atoi(argv[2]);
	int sockfd; 
	char buffer[1024]; 
	char generator[19];
	char *bin_message;
	struct sockaddr_in	 servaddr; 

	// Creating socket file descriptor 
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
		perror("socket creation failed"); 
		exit(EXIT_FAILURE); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = INADDR_ANY; 
	
	int n, len; 

	printf("Enter the text message to be sent: ");
    bzero(buffer,1024);
    fgets(buffer,1023,stdin);
	
    bin_message = stringToBinary(buffer);
	printf("\nMessage after converted to binary:\n %s\n\n", bin_message);

	bzero(generator, 19);
    printf("Enter the CRC-16 generator polynomial: ");
    fgets(generator, 18, stdin);

    sendto(sockfd, (const char *)generator, strlen(generator), 
		0, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 

    CRC(bin_message, generator);
	printf("\nCRC: %s\n\n", rem);

    printf("Message sent to the server in binary (with CRC at the end) :\n%s\n\n", out_string);

	sendto(sockfd, (const char *)out_string, strlen(out_string), 
		0, (const struct sockaddr *) &servaddr, 
			sizeof(servaddr)); 

	bzero(buffer, 1024);
	n = recvfrom(sockfd, (char *)buffer, 1024, 
				MSG_WAITALL, (struct sockaddr *) &servaddr, 
				&len); 
	buffer[n] = '\0'; 
	printf("Message from the Server : %s\n", buffer); 

	close(sockfd); 
	return 0; 
} 
