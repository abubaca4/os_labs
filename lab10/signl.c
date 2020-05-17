#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int sigint_c = 0, sigusr1_c = 0, sigusr2_c = 0;

void sigint(int nsig)
{
    if (nsig == SIGINT)
        sigint_c++;
}

void sigusr1(int nsig)
{
    if (nsig == SIGUSR1)
        sigusr1_c++;
}

void sigusr2(int nsig)
{
    if (nsig == SIGUSR2)
        sigusr2_c++;
}

int main()
{
    signal(SIGTERM, SIG_IGN);
    signal(SIGINT, sigint);
    signal(SIGUSR1, sigusr1);
    signal(SIGUSR2, sigusr2);
    while (1)
    {
        size_t startTime = time(NULL);
        usleep(900);
        while (time(NULL) - startTime < 1 && sigint_c < 5)
            usleep(10);
        printf("Time: %ld\n", time(NULL));
        printf("SIGINT: %d\n", sigint_c);
        printf("SIGUSR1: %d\n", sigusr1_c);
        printf("SIGUSR2: %d\n", sigusr2_c);
        if (sigint_c >= 5)
            break;
    }
}