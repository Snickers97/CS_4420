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
    msg_t msg;
    int pid_c = fork();
    int pid_p = getpid();
    if(pid_c != 0){    //parent process
        while(1){
            read(fd[0], (char *) &msg, sizeof(msg));
            if(msg.type == REGULAR){
                printf("\nserver (PID:%d) received string:\n",pid_p);
                printf("%s",msg.message_text);
                printf("from client (PID:%d)\n\n",pid_c);
            }
            else if(msg.type == COMMAND){
                printf("server (PID:%d) exits\n",pid_p);
                break;
            }
        }
        exit(0);
    }
    else{           //child process
        char msgtype[3];        //checks what type of message
        printf("Enter a line of text\n");
        printf("Type 'send: <message>' to send a message or 'exit' to exit.\n");
        while(1){
            //printf("Enter a line of text\n");
            fgets(msgtype,7,stdin);     //gets the message type, including colon and space
            //send message
            if(msgtype[0] == 's' && msgtype[1] == 'e' && msgtype[2] == 'n' && msgtype[3] == 'd'){
                //I really hate C sometimes and would have preferred to use the C++ str.substr() method
                //This was the easiest way I could think of to check this in C
                msg.type = REGULAR;
                fgets(msg.message_text,100,stdin);
                write(fd[1], (char *) &msg, sizeof(msg));
            }
            //exit
            else if(msgtype[0] == 'e' && msgtype[1] == 'x' && msgtype[2] == 'i' && msgtype[3] == 't'){
                msg.type = COMMAND;
                write(fd[1], (char *) &msg, sizeof(msg));
                break;
            }
            //User did not enter either send or exit:
            else
                continue;
        }

        _exit(0);
    }
    return 0;
}
