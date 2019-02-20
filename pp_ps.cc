#include <iostream>
#include <dirent.h>
#include <fstream>
#include <string.h>
#include <unistd.h>
#include <iomanip>

using namespace std;

void inp(string piddir);
double cpu_calc(string str[52]);
double mem_calc(string str[52]);

int main(int argc, char **argv){
    if(argc != 2){
        cout<<"Usage: ./pp_ps [-cpu|-mem|-pid|-com]\n";
        return 0;
    }
    string input = argv[1];
    if(input != "-cpu" && input != "-mem" && input != "-pid" && input != "-com"){
        cout<<"Usage: ./pp_ps [-cpu|-mem|-pid|-com]\n";
        return 0;
    }
    //Input stuff
    cout<<"PID           Command                 State    %CPU         %MEM       VSZ            RSS            Core"<<endl;
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
                inp(piddir);
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

void inp(string piddir){
    ifstream in;
    in.open("/proc/" + piddir + "/stat");
    if(!in){
        cout<<"Error: unable to open stat file\n";
        exit(-1);
    }
    string str[52];
    in>>str[0];
    in>>str[1];
    //I had touble with strtok so I just did this manually
    //Handles the case where the command contains no spaces
    if(str[1][str[1].size()-1] == ')'){
        str[1] = str[1].substr(1,str[1].size()-2);
    }
    //Handles the case where the command contains a space
    else{
        in>>str[2];
        str[1] = str[1]+ ' '+ str[2];
        str[1] = str[1].substr(1,str[1].size()-2);
    }
    for(int i = 2; i < 52; i++){
        in>>str[i];
    }
    in.close();
    double percent_c = cpu_calc(str);
    double percent_m = mem_calc(str);
    cout<<str[0];
    for(int i = 0; i < 14-str[0].size(); i++){
        cout<<' ';
    }
    cout<<str[1];
    for(int i = 0; i < 24-str[1].size(); i++){
        cout<<' ';
    }
    cout<<str[2]<<"        "<<setprecision(4)<<fixed<<percent_c;
    string length_test = to_string(percent_c);
    int length = 0;
    for(int i = 0; length_test[i] != '.'; i++){
        length++;
    }
    for(int i = 0; i < 8 - length; i++){
        cout<<' ';
    }
    cout<<setprecision(4)<<fixed<<percent_m;
    length_test = to_string(percent_m);
    length = 0;
    for(int i = 0; length_test[i] != '.'; i++){
        length++;
    }
    for(int i = 0; i < 6 - length; i++){
        cout<<' ';
    }
    cout<<str[22];
    for(int i = 0; i < 15 - str[22].size(); i++){
        cout<<' ';
    }
    cout<<str[23];
    for(int i = 0; i < 15 - str[23].size(); i++){
        cout<<' ';
    }
    cout<<str[38]<<endl;
}

double cpu_calc(string str[52]){
    ifstream in;
    double utime, stime, process_time, starttime, uptime, realtime, percent_c;
    utime = stod(str[13]);
    stime = stod(str[14]);
    process_time = utime/sysconf(_SC_CLK_TCK) + stime/sysconf(_SC_CLK_TCK);
    starttime = stod(str[21]);
    in.open("/proc/uptime");
    if(!in){
        cout<<"Error: unable to open uptime file\n";
        exit(-1);
    }
    in>>uptime;
    in.close();
    realtime = uptime - (starttime/sysconf(_SC_CLK_TCK));
    percent_c = process_time*100/realtime;
    return percent_c;
}

double mem_calc(string str[52]){
    double rss, percent_m;
    long phys_pages, pagesize, phys_memsize;
    rss = stod(str[23]);
    phys_pages = sysconf(_SC_PHYS_PAGES);
    pagesize = sysconf(_SC_PAGE_SIZE);
    phys_memsize = phys_pages * pagesize;
    percent_m = (rss * pagesize * 100) / phys_memsize;
    return percent_m;
}
