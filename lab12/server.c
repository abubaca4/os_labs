#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <signal.h>

#define MAX_FILENAME_SIZE 256

long fsize(FILE *fp)
{
    long prev = ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fseek(fp, prev, SEEK_SET);
    return sz;
}

void command(int fork_pid)
{
    printf("Ready to recive commands\n");
    char command[MAX_FILENAME_SIZE];
    while (1)
    {
        if (kill(fork_pid, 0) == -1)
            exit(-1);
        scanf("%s", command);
        if (strcmp(command, "exit") == 0)
        {
            kill(fork_pid, SIGKILL);
            exit(0);
        }
        else if (strcmp(command, "help") == 0)
        {
            printf("Avalible commands:\n");
            printf("exit - closes app\n");
            printf("help - shows avalible commands\n");
        }
        else
        {
            printf("Unknown command, please use help to get list of avalible commands\n");
        }
    }
}

void client(int sockfd)
{
    while (1)
    {
        char filename[MAX_FILENAME_SIZE];
        int n = read(sockfd, filename, MAX_FILENAME_SIZE);
        if (n == 0)
        {
            close(sockfd);
            exit(0);
        }
        FILE *fin = fopen(filename, "r");
        if (fin == NULL)
        {
            long statusmsg = -1;
            write(sockfd, &statusmsg, sizeof(statusmsg));
        }
        else
        {
            long filesize = fsize(fin);
            write(sockfd, &filesize, sizeof(filesize));
            char msg[filesize];
            fread(msg, sizeof(char), filesize, fin);
            fclose(fin);
            write(sockfd, msg, sizeof(msg));
        }
    }
}

void lisener(int port)
{
    int sockfd;
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    sockfd = socket(PF_INET, SOCK_STREAM, 0);
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        servaddr.sin_port = 0;
        if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
    }
    socklen_t servlen = sizeof(servaddr);
    listen(sockfd, 5);
    getsockname(sockfd, (struct sockaddr *)&servaddr, &servlen);
    printf("Listening on port: %d\n", ntohs(servaddr.sin_port));
    while (1)
    {
        struct sockaddr_in cliaddr;
        socklen_t clilen = sizeof(cliaddr);
        int newsockfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
        if (fork() == 0)
            client(newsockfd);
    }
}

int main(int argv, char *argc[])
{
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

    int fork_pid = fork();

    if (fork_pid == 0)
        lisener(port);
    else
        command(fork_pid);
}