/*Written by Jacob Handley for CS 4420, using some of the code provided in the
example on Blackboard*/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
    char writepipe[MSG_LENGTH];
    char readpipe[MSG_LENGTH];
} initial_msg_t;

int main(){
    int fd, ret;
    initial_msg_t initial_msg;
    char buf[MAX_BUF];
    //Formatting strings in C...
    sprintf(buf, "%d", getpid());
    strcpy(initial_msg.pid,buf);
    char writepipe[MAX_BUF] = "./";
    sprintf(writepipe + strlen(writepipe),"%s",buf);
    sprintf(writepipe + strlen(writepipe),"_send");
    char readpipe[MAX_BUF] = "./";
    sprintf(readpipe + strlen(readpipe),"%s",buf);
    sprintf(readpipe + strlen(readpipe),"receive");
    char * myfifo = "./server_np";
    strcpy(initial_msg.writepipe, writepipe);
    strcpy(initial_msg.readpipe, readpipe);


    /* write PID to the FIFO */
    fd = open(myfifo, O_WRONLY);
    if( write(fd, (char *) &initial_msg, sizeof(initial_msg)) < 0)
        fprintf(stderr, "Error\n");

    close(fd);
    mkfifo(writepipe, 0660);
    mkfifo(readpipe, 0660);
    fd = open(writepipe, O_WRONLY);
    ret = open(readpipe, O_RDONLY);
    msg_t msg;
    char msgtype[3];        //checks what type of message
    printf("Enter a line of text\n");
    printf("Type 'send: <message>' to send a message or 'exit' to exit.\n");
    while(1){
        fgets(msgtype,7,stdin);     //gets the message type, including colon and space
        //send message
        if(msgtype[0] == 's' && msgtype[1] == 'e' && msgtype[2] == 'n' && msgtype[3] == 'd'){
            //I really hate C sometimes and would have preferred to use the C++ str.substr() method
            //This was the easiest way I could think of to check this in C
            msg.type = REGULAR;
            fgets(msg.message_text,100,stdin);
            write(fd, (char *) &msg, sizeof(msg));
        }
        //return current time to the client
        else if(msgtype[0] == 't' && msgtype[1] == 'i' && msgtype[2] == 'm' && msgtype[3] == 'e'){
            msg.type = TIME;
            write(fd, (char *) &msg, sizeof(msg));
            read(ret, (char *) &msg, sizeof(msg));
            printf("\nCurrent time: %s\n",msg.message_text);
        }
        //return numner of clients to the client
        else if(msgtype[0] == 's' && msgtype[1] == 't' && msgtype[2] == 'a' && msgtype[3] == 't'){
            msg.type = STATUS;
            write(fd, (char *) &msg, sizeof(msg));
            read(ret, (char *) &msg, sizeof(msg));
            printf("Number of clients: %s\n",msg.message_text);
        }
        //exit
        else if(msgtype[0] == 'e' && msgtype[1] == 'x' && msgtype[2] == 'i' && msgtype[3] == 't'){
            msg.type = COMMAND;
            write(fd, (char *) &msg, sizeof(msg));
            close(fd);
            close(ret);
            unlink(writepipe);
            unlink(readpipe);
            exit(0);
        }
        //User did not enter either send or exit:
        else
            continue;
    }

    return 0;
}
