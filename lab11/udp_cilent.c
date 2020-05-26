#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/time.h>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define MAS_SIZE 10
#define MIN_R 0
#define MAX_R 1000

int get_random_number(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

long mtime()
{
  struct timeval t;
  gettimeofday(&t, NULL);
  long mt = (long)t.tv_sec * 1000 + t.tv_usec / 1000;
  return mt;
}
  
// Driver code 
int main(int argv, char *argc[]) { 
    if (argv != 2)
    {
        perror("No address given");
        return -1;
    }

    int border = strstr(argc[1], ":") - argc[1];

    if (border + argc[1] == NULL)
    {
        perror("Not correct format of address");
        return -1;
    }

    char ip[border + 1];
    strncpy(ip, argc[1], border);
    ip[border] = '\0';
    int port = atoi(argc[1] + border + 1);

    int sockfd; 
    int buffer[MAS_SIZE];  
    struct sockaddr_in     servaddr; 
  
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port); 
    inet_aton(ip, &servaddr.sin_addr.s_addr);
      
    int n, len; 
    long t;
    srand(time(NULL));
    for (int i=0; i<MAS_SIZE; i++)
        buffer[i] = get_random_number(MIN_R, MAX_R);
      
    sendto(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE, 
        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
    t = mtime();
          
    n = recvfrom(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len); 
    t = mtime() - t;
    printf("Server : "); 
    for (int i=0; i<MAS_SIZE; i++)
        printf("%d ", buffer[i]);
    printf("\n");   
    printf ("It took %ld milliseconds.\n",t);
  
    close(sockfd); 
    return 0; 
} 