#include <stdio.h> 
#include <stdlib.h> 
#include <malloc.h>
#include <sys/time.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

int get_random_number(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

long ms_time()
{
  struct timeval t;
  gettimeofday(&t, NULL);
  long mt = (long)t.tv_sec * 1000 + t.tv_usec / 1000;
  return mt;
}
  
// Driver code 
int main(int argv, char *argc[]) 
{ 
    int MAS_SIZE = 10, MIN_R = 0, MAX_R = 1000;
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

    //Set of for make
    printf("Enter count of elements: ");
    scanf("%d", &MAS_SIZE);
    printf("Enter minimum value: ");
    scanf("%d", &MIN_R);
    printf("Enter maximum value: ");
    scanf("%d", &MAX_R);

    char ip[border + 1];
    strncpy(ip, argc[1], border);
    ip[border] = '\0';
    int port = atoi(argc[1] + border + 1);

    int sockfd; 
    int *buffer;  
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
    inet_aton(ip, &(servaddr.sin_addr));
      
    int n, len; 
    long t;

    sendto(sockfd, (int *)&MAS_SIZE, sizeof(MAS_SIZE), 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr));

    int temp;

    recvfrom(sockfd, (int *)&temp, sizeof(temp),  
                0, (struct sockaddr *) &servaddr, 
                &len); 

    if (temp != MAS_SIZE)
    {
        perror("Size sending error");
        return -1;
    }
    
    buffer = calloc(MAS_SIZE, sizeof(int));
    srand(time(NULL));
    for (int i=0; i<MAS_SIZE; i++)
        buffer[i] = get_random_number(MIN_R, MAX_R);
      
    sendto(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE, 
        0, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
    t = ms_time();
          
    n = recvfrom(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,  
                0, (struct sockaddr *) &servaddr, 
                &len); 
    t = ms_time() - t;
    printf("Server : "); 
    for (int i=0; i<MAS_SIZE; i++)
        printf("%d ", buffer[i]);
    printf("\n");   
    printf ("It took %ld milliseconds.\n",t);
  
    close(sockfd); 
    free(buffer);
    return 0; 
} 