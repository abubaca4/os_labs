#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_FILENAME_SIZE 256

int main(int argv, char *argc[])
{
    /*if (argv != 2)
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
    int port = atoi(argc[1] + border + 1);*/
    char ip[] = "127.0.0.1";
    int port = 8080;

    char path_to_save[] = "downloads";

    struct sockaddr_in servaddr;
    int sockfd = socket(PF_INET, SOCK_STREAM, 0);

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_aton(ip, &(servaddr.sin_addr));

    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        printf("Can't connect to server\n");
        return 1;
    }

    printf("Ready to recive commands\n");
    char command[MAX_FILENAME_SIZE];
    while (1)
    {
        scanf("%s", command);

        if (strcmp(command, "exit") == 0)
        {
            break;
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("Avalible commands:\n");
            printf("exit - closes app\n");
            printf("help - shows avalible commands\n");
            printf("any other command will be interpreted as filename\n");
        }
        else
        {
            write(sockfd, command, strlen(command) + 1);
            long status;
            int n = read(sockfd, &status, sizeof(status));
            if (n == 0)
            {
                close(sockfd);
                printf("Connection lost\n");
                exit(1);
            }
            else if (status == -1)
            {
                printf("File not found by server\n");
            }
            else
            {
                printf("Downloading file\n");
                long filesize = status;

                int comand_len = strlen(command), path_len = strlen(path_to_save);
                char *save_path = calloc(path_len + comand_len + 2, sizeof(char));
                strncpy(save_path, path_to_save, path_len);
                save_path[path_len] = '_'; // здесь должен быть / пути но по абсолютно неясной причине оно не хочет писать в папку без прав администратора
                strncpy(save_path + path_len + 1, command, comand_len);
                save_path[path_len + comand_len + 1] = '\0';
                //здесь должна быть проверка наличия папки и создание при необходимости
                /*struct stat st = {0};
                if (stat(path_to_save, &st) == -1)
                    mkdir(path_to_save, 0666);*/

                int output_file = open(save_path, O_RDWR | O_CREAT, 0666);

                if (output_file == -1)
                {
                    printf("File opening error %d \n", errno);
                    exit(-1);
                }

                ftruncate(output_file, filesize);
                char *output_file_data = (char *)mmap(NULL, filesize, PROT_WRITE | PROT_READ, MAP_SHARED, output_file, 0);
                read(sockfd, output_file_data, filesize);
                munmap(output_file_data, filesize);
                close(output_file);

                free(save_path);

                printf("Downloading finished\n");
            }
        }
    }
    close(sockfd);
}