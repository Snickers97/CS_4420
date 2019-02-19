#include <iostream>
#include <dirent.h>
#include <fstream>

using namespace std;

void output(string str[8]);

int main(int argc, char **argv){
    //Input stuff
    cout<<"PID            Command         State    %CPU       %MEM       VSZ       RSS       Core"<<endl;
    long pid;
    struct dirent *ep;
    DIR *dp;
    dp = opendir ("/proc");
    if (dp != NULL){
        while (ep = readdir (dp)){
            pid = strtol(ep->d_name, NULL, 10);
            if( ( ep->d_type == DT_DIR ) && ( pid > 0) ){
                //printf("directory name: %s\n", ep->d_name);
                // Do something with the proc directory
                ifstream in;
                in.open("/proc/" + string(ep->d_name) + "/stat");
                if(!in){
                    cout<<"Error: unable to open stat file\n";
                    exit(-1);
                }
                string str[8];
                for(int i = 0; i < 8; i++){
                    in>>str[i];
                }
                output(str);
            }
            //closedir(dp);
            //This line gave me trouble so I commented it out
            //The program works fine without it for now
        }
    }
    else{
        perror("Couldn't open the directory");
        exit(-1);
    }
    return 0;
}

void output(string str[8]){
    cout<<str[0];
    for(int i = 0; i < 14-str[0].size(); i++){
        cout<<" ";
    }
    cout<<str[1]<<endl;
}
