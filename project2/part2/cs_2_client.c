/*Written by Jacob Handley for CS 4420, using some of the code provided in the
example on Blackboard*/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUF 1024
#define MSG_LENGTH 100

typedef enum{
    REGULAR,
    COMMAND,
    TIME,
    STATUS
} msg_type_t;

typedef struct msg {
    msg_type_t type;
    char message_text[MSG_LENGTH];
} msg_t;

typedef struct initial_msg{
    char pid[MSG_LENGTH];
} initial_msg_t;

int main(){
    int fd, ret;
    initial_msg_t initial_msg;
    char buf[MAX_BUF];
    //Formatting strings in C...
    sprintf(buf, "%d", getpid());
    strcpy(initial_msg.pid,buf);
    char writepipe[MAX_BUF] = "./";
    strcat(writepipe,buf);
    char tmp[MAX_BUF] = "_send";
    strcat(writepipe,tmp);
    char readpipe[MAX_BUF] = "./";
    strcat(readpipe,buf);
    char tmp2[MAX_BUF] = "_receive";
    strcat(readpipe,tmp2);
    char * myfifo = "./server_np";

    /* write PID to the FIFO */
    fd = open(myfifo, O_WRONLY);
    if( write(fd, (char *) &initial_msg, sizeof(initial_msg)) < 0)
        fprintf(stderr, "Error\n");

    close(fd);

    return 0;
}
