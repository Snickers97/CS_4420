#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <unistd.h>

using namespace std;

string input(string piddir);

int main(int argc, char **argv){
    //Input stuff
    cout<<"PID           Command                 State    %CPU       %MEM       VSZ       RSS       Core"<<endl;
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
                string piddir = string(ep->d_name);
                string str = input(piddir);
                cout<<str<<endl;
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

string input(string piddir){
    ifstream in, in2;
    in.open("/proc/" + piddir + "/stat");
    if(!in){
        cout<<"Error: unable to open stat file\n";
        exit(-1);
    }
    string str[52], outstring;
    in>>str[0];
    in>>str[1];
    if(str[1][str[1].size()-1] == ')'){
        str[1] = str[1].substr(1,str[1].size()-2);
    }
    else{
        in>>str[2];
        str[1] = str[1]+ ' '+ str[2];
        str[1] = str[1].substr(1,str[1].size()-2);
    }
    for(int i = 2; i < 52; i++){
        in>>str[i];
    }
    double utime, stime, process_time, starttime, uptime, realtime, percent;
    utime = stod(str[13]);
    stime = stod(str[14]);
    process_time = utime/sysconf(_SC_CLK_TCK) + stime/sysconf(_SC_CLK_TCK);
    starttime = stod(str[21]);
    in2.open("/proc/uptime");
    if(!in2){
        cout<<"Error: unable to open uptime file\n";
        exit(-1);
    }
    in2>>uptime;
    in2.close();
    realtime = uptime - (starttime/sysconf(_SC_CLK_TCK));
    percent = process_time*100/realtime;
    outstring = str[0];
    for(int i = 0; i < 14-str[0].size(); i++){
        outstring += ' ';
    }
    outstring += str[1];
    for(int i = 0; i < 24-str[1].size(); i++){
        outstring += ' ';
    }
    outstring += str[2] + "        " + to_string(percent).substr(0,4);
    return outstring;
}
