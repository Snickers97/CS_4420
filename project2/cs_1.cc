#include <unistd.h>
#include <iostream>
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
    msg *message;
    if(fork() != 0){    //parent process
        close(fd[1]);   //close write end
        dup2(fd[0],0);  //duplicate read to stdin
        close(fd[0]);   //close original read
        while(1){
            read(fd[0], message->message_text, MSG_LENGTH);
        }
    }
    else{           //child process
        close(fd[0]);   //close read
        dup2(fd[1],1);  //duplicate write to stdout
        close(fd[1]);   //close original write
        std::cout<<"Enter a line of text\n";
        while(1){
            std::cin.getline(message->message_text, MSG_LENGTH);
            write(fd[1], message->message_text, MSG_LENGTH);
        }

        //_exit();
    }
    return 0;
}
