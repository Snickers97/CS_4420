#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define MSG_LENGTH 100 // maximum length of message

typedef enum{
    REGULAR,
    COMMAND
} msg_type_t;
typedef struct msg {
    msg_type_t type;
    char message_text[MSG_LENGTH];
} msg_t;

int main(){
    int fd[2];
    pipe(fd);
    struct msg message;
    int pid_c = fork();
    int pid_p = getpid();
    if(pid_c != 0){    //parent process
        while(1){
            read(fd[0], message.message_text, MSG_LENGTH);
            if(message.type == REGULAR){
                printf("server (PID:%d) received string:\n",pid_p);
                printf(message.message_text);
                printf("from client (PID:%d)\n",pid_c);
            }
            else if(message.type == COMMAND){
                printf("server (PID:%d) exits\n",pid_p);
                break;
            }
        }
        exit(0);
    }
    else{           //child process
        char msgtype[3];        //checks what type of message
        printf("Enter a line of text\n");
        while(1){
            fgets(msgtype,6,stdin);
            printf(msgtype);
            printf("\n");
            fgets(message.message_text,100,stdin);
            if(msgtype == "send"){
                message.type = REGULAR;
            }
            else if(msgtype == "exit"){
                message.type = COMMAND;
            }
            write(fd[1], message.message_text, MSG_LENGTH);
            if(message.type == COMMAND){
                break;
            }
        }

        _exit(0);
    }
    return 0;
}
