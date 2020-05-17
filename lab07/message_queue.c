#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int get_random_number(int min, int max)
{
    return rand() % (max - min + 1) + min;
}

struct Strmsg
{
    long mtype;
    int data[4];
    int islast;
};

void parentMainCode(int msgId)
{
    struct Strmsg localmsg;
    for (int i = 0; i < 4; i++)
        localmsg.data[i] = get_random_number(0, 10000);
    localmsg.islast = 1;
    localmsg.mtype = 1;
    msgsnd(msgId, &localmsg, sizeof(localmsg), 0);
    int count_of_r = 0;
    do
    {
        msgrcv(msgId, &localmsg, sizeof(localmsg), 2, 0);
        count_of_r = count_of_r + 1;
        printf("Parent get %i: %i %i %i %i", count_of_r, localmsg.data[0], localmsg.data[1], localmsg.data[2], localmsg.data[3]);
    } while (!localmsg.islast);
    printf("Parent: wait until child is finished.\n");
    waitpid(0, 0, 0);
    printf("Parent: releasing the message queue.\n");
    msgctl(msgId, IPC_RMID, NULL);
    printf("Parent: Process is finished.\n");
}

void childMainCode(int msgId)
{
    struct Strmsg childlocalmsg;
    msgrcv(msgId, &childlocalmsg, sizeof(childlocalmsg), 1, 0);
    int data[4];
    for (int i = 0; i < 3; i++)
        data[i] = childlocalmsg.data[i];
    
    printf("Child: the last message was read.\n");
    printf("Child: Process is finished.\n");
}

int main()
{
    const size_t semCount = 10;
    int msgId = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);
    if (msgId < 0)
    {
        perror("error with msgget()");
        return -1;
    }
    else
    {
        printf("message id = %i\n", msgId);
    }
    pid_t childId = fork();
    if (childId < 0)
    {
        perror("error with fork()\n");
    }
    else if (childId > 0)
    {
        parentMainCode(msgId);
    }
    else
    {
        childMainCode(msgId);
    }
    return 0;
}