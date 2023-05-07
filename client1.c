#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include<stdlib.h> 
#include<math.h> 
#include<string.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include<unistd.h>

#define ull unsigned long long


void error(char *msg)
{
    perror(msg);
    exit(0);
}

ull power(ull x, ull  y, ull n)  
{  
    ull res = 1;     
  
    x = x % n;
 
  
    while (y > 0)  
    {  
        // If y is odd, multiply x with result  
        if (y & 1)  
            res = (res*x) % n;  
  
        // y must be even now  
        y = y>>1; // y = y/2  
        x = (x*x) % n;  
    }  
    return res;  
}  

int encrypter(char* str, ull len, char* encrypted, ull e, ull n)
{
    char* temp = encrypted;
    ull counter = 0;
    for (ull i = 0; i < len; i++)
    {
        ull enc = power(str[i],e, n);
        if(i==0) sprintf(temp+counter, "%lld ", enc);
        else sprintf(temp+counter-1, "%lld ", enc);
        counter += strlen(temp+counter)+1;
    }
    return 0;
}


int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");


    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0)  error("ERROR reading from socket");

    char * temp = strtok(buffer, " ");
    ull N = atoi(temp);

    temp = strtok(NULL, " ");
    ull e = atoi(temp);

    printf("N = %lld\t e = %lld\n",N, e);



    if(fork()==0)
    {
        
    	while(1)
    	{
			bzero(buffer,256);
			fgets(buffer,255,stdin);
            printf("Actual Text:%s", buffer);

            char encrypted[256];
            encrypter(buffer, strlen(buffer), encrypted, e, N);
            printf("Encrypted Text:%s\n\n", encrypted);

			n = write(sockfd,encrypted,strlen(encrypted));
			if (n < 0) 
				 error("ERROR writing to socket");
		}
    }

    wait(NULL);
    return 0;
}