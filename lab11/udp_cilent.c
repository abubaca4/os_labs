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

unsigned long long mic_s_time()
{
  struct timeval t;
  gettimeofday(&t, NULL);
  unsigned long long mt = (unsigned long long)t.tv_sec * 1000000 + t.tv_usec;
  return mt;
}
  
#define MAX_SAVE_UDP_DATA_SIZE 508

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
    printf("\nEnter minimum value: ");
    scanf("%d", &MIN_R);
    printf("\nEnter maximum value: ");
    scanf("%d", &MAX_R);
    printf("\n");

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
      
    int len; 
    unsigned long long t;

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
    
    if (MAS_SIZE * sizeof(int) <= MAX_SAVE_UDP_DATA_SIZE) 
    {
        sendto(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE, 
            0, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr)); 
        t = mic_s_time();
                
        recvfrom(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,  
                    0, (struct sockaddr *) &servaddr, 
                    &len); 
        t = mic_s_time() - t;
    }
    else
    {
        int data_tr_len = MAX_SAVE_UDP_DATA_SIZE - sizeof(int);
        for (int i=0; i< (MAS_SIZE * sizeof(int))/data_tr_len + 1; i++)
        {
            int packet_size = 1 + (i == MAS_SIZE * sizeof(int) / data_tr_len) ? (MAS_SIZE % (data_tr_len / sizeof(int))) : (data_tr_len / sizeof(int));
            packet_size = packet_size * sizeof(int);
            //Буфер и номер отправленного куска

            sendto(sockfd, (int *)(buffer + data_tr_len / sizeof(int)), packet_size, 
            0, (const struct sockaddr *) &servaddr,  
                sizeof(servaddr));
        }
        t = mic_s_time();

        for (int i=0; i< (MAS_SIZE * sizeof(int))/data_tr_len + 1; i++)
        {
            int packet_size = 1 + (i == MAS_SIZE * sizeof(int) / data_tr_len) ? (MAS_SIZE % (data_tr_len / sizeof(int))) : (data_tr_len / sizeof(int));
            packet_size = packet_size * sizeof(int);

            recvfrom(sockfd, (int *)(buffer + data_tr_len / sizeof(int)), packet_size,  
                    0, (struct sockaddr *) &servaddr, 
                    &len);
            t = mic_s_time() - t; 
        }
    }
    
    printf("Server : "); 
    for (int i=0; i<MAS_SIZE; i++)
        printf("%d ", buffer[i]);
    printf("\n");   
    printf ("It took %lld microseconds.\n",t);
  
    close(sockfd); 
    free(buffer);
    return 0; 
} 