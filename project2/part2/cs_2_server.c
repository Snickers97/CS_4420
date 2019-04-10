/*Written by Jacob Handley for CS 4420, using some of the code provided in the
example on Blackboard*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_BUF 1024
#define MSG_LENGTH 100

typedef struct initial_msg{
    char pid[MSG_LENGTH];
} initial_msg_t;

int main(){
    int fd;
    char * myfifo = "./server_np";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0660);

    initial_msg_t initial_msg;
    /* open, read, and display the message from the FIFO */
    while(1){
        fd = open(myfifo, O_RDONLY);
        int is_fork = 1;        //Check whether parent or child

        if(read(fd, (char *) &initial_msg, sizeof(initial_msg))>0){
            printf("Received: %s\n", initial_msg.pid);
            is_fork = fork();
        }
        if(is_fork == 0){
            printf("Child server for %s\n",initial_msg.pid);
        }

        close(fd);
    }

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}
