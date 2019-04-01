#include <unistd.h>

int main(){
    int fd[2];
    pipe(fd);
    if(fork() != 0){    //parent process
        close(fd[1]);   //close write end
        dup2(fd[0],0);  //duplicate read to stdin
        close(fd[0]);   //close original read
        while(1){
            read();
        }
    }
    else{           //child process
        close(fd[0]);   //close read
        dup2(fd[1],1);  //duplicate write to stdout
        close(fd[1]);   //close original write
        cout<<"Enter a line of text\n";
    }
    return 0;
}
