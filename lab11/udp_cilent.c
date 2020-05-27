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
    struct sockaddr_in servaddr;

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
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
           0, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));

    int temp;

    recvfrom(sockfd, (int *)&temp, sizeof(temp),
             0, (struct sockaddr *)&servaddr,
             &len);

    if (temp != MAS_SIZE)
    {
        perror("Size sending error");
        return -1;
    }

    buffer = calloc(MAS_SIZE, sizeof(int));
    srand(time(NULL));
    for (int i = 0; i < MAS_SIZE; i++)
        buffer[i] = get_random_number(MIN_R, MAX_R);

    if (MAS_SIZE * sizeof(int) <= MAX_SAVE_UDP_DATA_SIZE)
    {
        sendto(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,
               0, (const struct sockaddr *)&servaddr,
               sizeof(servaddr));
        t = mic_s_time();

        recvfrom(sockfd, (int *)buffer, sizeof(buffer[0]) * MAS_SIZE,
                 0, (struct sockaddr *)&servaddr,
                 &len);
        t = mic_s_time() - t;
    }
    else
    {
        int data_item_count = (MAX_SAVE_UDP_DATA_SIZE - sizeof(int)) / sizeof(int);
        for (int i = 0; i < (MAS_SIZE / data_item_count + 1); i++)
        {
            int block_size = i == MAS_SIZE / data_item_count ? MAS_SIZE % data_item_count : data_item_count;
            int data_size = sizeof(int) * (1 + block_size);
            int *send_buf = malloc(data_size);
            send_buf[0] = i;
            for (int j = 1; j <= block_size; j++)
                send_buf[j] = buffer[i * data_item_count + j - 1];
            sendto(sockfd, (int *)send_buf, data_size,
                   0, (const struct sockaddr *)&servaddr,
                   sizeof(servaddr));
            free(send_buf);
        }
        t = mic_s_time();

        int *recive_buf = malloc(MAX_SAVE_UDP_DATA_SIZE);
        for (int i = 0; i < (MAS_SIZE / data_item_count + 1); i++)
        {
            recvfrom(sockfd, (int *)recive_buf, MAX_SAVE_UDP_DATA_SIZE,
                     0, (struct sockaddr *)&servaddr,
                     &len);
            int block_number = recive_buf[0];
            int block_size = block_number == MAS_SIZE / data_item_count ? MAS_SIZE % data_item_count : data_item_count;
            for (int j = 1; j <= block_size; j++)
                buffer[block_number * data_item_count + j - 1] = recive_buf[j];
        }
        free(recive_buf);
        t = mic_s_time() - t;
    }

    printf("Server : ");
    for (int i = 0; i < MAS_SIZE; i++)
        printf("%d ", buffer[i]);
    printf("\n");
    printf("It took %lld microseconds.\n", t);

    close(sockfd);
    free(buffer);
    return 0;
}