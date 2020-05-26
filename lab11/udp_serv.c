#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define MAS_SIZE 10

static int comp(const void *a, const void *b)
{
    return *(const int *)a - *(const int *)b;
}
  
// Driver code 
int main(int argv, char *argc[]) { 
    if (argv != 2)
    {
        perror("No port given");
        return -1;
    }

    int port = atoi(argc[1]);

    if (port == 0)
    {
        perror("Not correct port");
        return -1;
    }
    
    int sockfd; 
    int buffer[MAS_SIZE]; 
    struct sockaddr_in servaddr, cliaddr; 
      
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(port); 
      
    // Bind the socket with the server address 
    if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
    { 
        servaddr.sin_port = 0;
        if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) 
        {
            perror("bind failed"); 
            exit(EXIT_FAILURE); 
        }
    } 

    socklen_t servlen = sizeof(servaddr);
    getsockname(sockfd, (struct sockaddr *)&servaddr, &servlen);
    printf("Lisening on port: %d\n", ntohs(servaddr.sin_port));
      
    int len, n; 
  
    len = sizeof(cliaddr);  //len is value/resuslt 
  
    n = recvfrom(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,  
                0, (struct sockaddr *) &cliaddr, 
                &len); 
    
    qsort(buffer, MAS_SIZE, sizeof(buffer[0]), comp);

    sendto(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,  
        0, (const struct sockaddr *) &cliaddr, 
            len); 

    printf("Server off\n");
    return 0; 
} 