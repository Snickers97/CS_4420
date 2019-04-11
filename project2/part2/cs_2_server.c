/*Written by Jacob Handley for CS 4420, using some of the code provided in the
example on Blackboard*/

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
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
    int fd;
    char * myfifo = "./server_np";

    /* create the FIFO (named pipe) */
    mkfifo(myfifo, 0660);

    initial_msg_t initial_msg;
    int client_count = 0;
    /* open, read, and display the message from the FIFO */
    while(1){
        fd = open(myfifo, O_RDONLY);
        int is_fork = 1;        //Check whether parent or child

        if(read(fd, (char *) &initial_msg, sizeof(initial_msg))>0){
            printf("Received: %s\n", initial_msg.pid);
            is_fork = fork();
        }
        if(is_fork > 1){        //A fork has ocurred (from the parent's view)
            client_count++;
        }
        if(is_fork == 0){       //Child server
            client_count++;     //The client must count itself
            close(fd);
            int pid_p = getpid();
            int ret;
            fd = open(initial_msg.writepipe, O_RDONLY);
            ret = open(initial_msg.readpipe, O_WRONLY);
            while(1){
                msg_t msg;
                read(fd, (char *) &msg, sizeof(msg));
                if(msg.type == REGULAR){
                    printf("\nserver (PID:%d) received string:\n",pid_p);
                    printf("%s",msg.message_text);
                    printf("from client (PID:%s)\n\n",initial_msg.pid);
                }
                else if(msg.type == TIME){
                    printf("server received time request from client\n");
                    long sec,min,hr;
                    //Find length in seconds of current day and adjust for timezone
                    //This configuration assumes a machine in the US Eastern time zone,
                    //not valid for other machines
                    sec = (time(NULL) - 4*3600) % 86400;
                    hr = (24*sec/86400);        //Convert total secs to hours and minutes
                    sec = sec % 3600;
                    min = (60*sec/3600);
                    sec = sec % 60;
                    char times[MSG_LENGTH];
                    sprintf(times, "%lu",hr);       //Combine times into a string
                    sprintf(times + strlen(times), ":");
                    sprintf(times + strlen(times), "%lu",min);
                    sprintf(times + strlen(times), ":");
                    sprintf(times + strlen(times), "%lu",sec);
                    //printf("Current time: %s\n",times);
                    strcpy(msg.message_text,times);
                    write(ret,(char *) &msg, sizeof(msg));
                }
                else if(msg.type == STATUS){
                    sprintf(msg.message_text, "%d", client_count);
                    printf("server received client count request\n");
                    write(ret,(char *) &msg, sizeof(msg));
                }
                else if(msg.type == COMMAND){
                    printf("server (PID:%d) exits\n",pid_p);
                    close(fd);
                    close(ret);
                    _exit(0);
                }
            }
        }

        close(fd);
    }

    /* remove the FIFO */
    unlink(myfifo);

    return 0;
}
