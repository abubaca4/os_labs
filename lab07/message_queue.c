#include <stdio.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>  

struct Strmsg 
{
    long mtype;
    char str[256];     
    int islast; 
};  

void parentMainCode(int msgId) 
{     
    struct Strmsg localmsg;     
    int i;          
    printf("Parent: 15 messages will be send.\n");     
    for (i = 1; i <= 15; ++i)     
    {         
        //prepare message         
        localmsg.mtype = 7;         
        sprintf(localmsg.str, "this is message number %i", i);         
        localmsg.islast = !(i < 15);                  
        //send message         
        msgsnd(msgId, &localmsg, sizeof(localmsg), 0);     
    }          
    printf("Parent: wait until child is finished.\n");     
    waitpid(0, 0, 0);          
    printf("Parent: releasing the message queue.\n");     
    msgctl(msgId, IPC_RMID, NULL);          
    printf("Parent: Process is finished.\n"); 
}

void childMainCode(int msgId) 
{     
    struct Strmsg childlocalmsg;      
    int islast = 0;     
    do     
    {         
        //read next message         
        ssize_t len = msgrcv(msgId, &childlocalmsg, sizeof(childlocalmsg), 0, 0);         
        printf("Child: message with len = %i incoming.\n", len);         
        islast = childlocalmsg.islast;         
        printf("Child: the following string was received: %s\n", childlocalmsg.str);     
    } 
    while(!islast);          
    printf("Child: the last message was read.\n");     
    printf("Child: Process is finished.\n"); 
}  

int main() 
{     
    const size_t semCount = 10;     
    int msgId = msgget(IPC_PRIVATE, 0600|IPC_CREAT);     
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
    else 
        if (childId > 0)     
        {         
            parentMainCode(msgId);     
        }     
        else    
        {         
            childMainCode(msgId);     
        }          
    return 0; 
}