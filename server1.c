#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
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
    exit(1);
}


int initialize_keys(ull *p, ull *q, ull *n, ull *e, ull *d)
{
    // Two random prime numbers 
    *p = 151; 
    *q = 101; 

    // First part of public key: 
    *n = *p**q; 


    // Finding other part of public key. 
    // e stands for encrypt 
    *e = 2; 
    ull phi = (*p-1)*(*q-1); 
    while (*e < phi) 
    { 
        // e must be co-prime to phi and 
        // smaller than phi. 
        if (gcd(*e, phi)==1) 
            break; 
        else
            (*e)++; 
    } 

    // Private key (d stands for decrypt) 
    // choosing d such that it satisfies 
    // d*e = 1 + k * totient 

    ull cphi;

    gcdExtended(*e,phi,d,&cphi);

    if(*d<0)
        *d+=phi;


    return 0;
}


int gcd(int a, int h) 
{ 
    int temp; 
    while (1) 
    { 
        temp = a%h; 
        if (temp == 0) 
        return h; 
        a = h; 
        h = temp; 
    } 
} 

int gcdExtended(ull a, ull b, ull *x, ull *y)  
{    
    if (a == 0)  
    {  
        *x = 0;  
        *y = 1;  
        return b;  
    }  
  
    ull x1, y1;  
    int gcd = gcdExtended(b%a, a, &x1, &y1);  
  

    *x = y1 - (b/a) * x1;  
    *y = x1;  
  
    return gcd;  
}  

ull power(ull x, ull  y, ull n)  
{  
    ull res = 1;     // Initialize result  
  
    x = x % n; // Update x if it is more than or  
                // equal to p  
  
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





int decrypt(char* str, ull len, char* decrypted, ull d, ull n)
{
    char* temp = decrypted;
    ull counter = 0;
    char *ptr = strtok(str, " ");
    while(ptr != NULL)
    {
        if(counter==0) sprintf(temp, "%c", (char)power(atoi(ptr), d, n));
        else sprintf(temp+counter-1, "%c", (char)power(atoi(ptr), d, n));
        counter += strlen(temp+counter)+1;
        ptr = strtok(NULL, " ");
    }
    return 0;
}



int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
            sizeof(serv_addr)) < 0) 
            error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    
    close(sockfd);
    ull p, q, N, e, d;
    initialize_keys(&p, &q, &N, &e, &d);
    printf("p = %llu, q = %llu, N = %llu, e = %llu, d = %llu\n", p, q, N, e, d);
    
    // Share pulbic key 
    char public_key[1000];
    sprintf(public_key, "%lld %lld", N, e);
    n = write(newsockfd, public_key, strlen(public_key));
    if (n < 0) error("ERROR writing to socket");

    while(1)
    {   
        printf("Listening...\n");
        printf("From Client Encrypted: ");
        sleep(0.5);
        bzero(buffer,256);
        n = read(newsockfd,buffer,255);
        if (n < 0) error("ERROR reading from socket");
        printf("%s\n",buffer);
        char decrypted[256];
        decrypt(buffer, strlen(buffer), decrypted, d, N);
        printf("Decrypted: %s\n",decrypted);
        		 
	}
	
     return 0; 
}