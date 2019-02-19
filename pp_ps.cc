#include <iostream>
#include <dirent.h>

using namespace std;

int main(int argc, char **argv){
    //Input stuff
    cout<<"PID                    Command   State    %CPU       %MEM       VSZ       RSS       Core"<<endl;
    int pid = 1;
    struct dirent *ep;
    DIR *dp;
    dp = opendir ("/proc");
    if (dp != NULL){
        while (ep = readdir (dp)){
            pid = strtol(ep->d_name, NULL, 10);
            if( ( ep->d_type == DT_DIR ) && ( pid > 0) ){
                printf("directory name: %s\n", ep->d_name);
                // Do something with the proc directory
            }
            closedir(dp);
        }
    }
    else{
        perror("Couldn't open the directory");
        exit(-1);
    }
    return 0;
}
